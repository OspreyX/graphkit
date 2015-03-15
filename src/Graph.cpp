/**
* Copyright (C) 2015 GraphKit, Inc. <http://graphkit.io> and other GraphKit contributors.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program located at the root of the software package
* in a file called LICENSE.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "Graph.h"
#include "exports.h"
#include "symbols.h"
#include "NodeClass.h"
#include "Export.h"
#include "Node.h"
#include "Entity.h"
#include "Action.h"
#include "Bond.h"

GK_CONSTRUCTOR(gk::Graph::constructor_);

gk::Graph::Graph() noexcept
	: gk::Export{},
	  coordinator_{nullptr} {}

gk::Graph::~Graph() {
	if (0 < coordinator_.use_count()) {
		coordinator_.reset();
	}
}

std::shared_ptr<gk::Coordinator> gk::Graph::coordinator() noexcept {
	if (nullptr == coordinator_) {
		coordinator_ = std::make_shared<gk::Coordinator>();
	}
	return coordinator_;
}

void gk::Graph::cleanUp() noexcept {
	auto cluster = coordinator()->nodeGraph();
	for (auto i = cluster->count(); 0 < i; --i) {
		cluster->select(i)->cleanUp();
	}
}

gk::Graph* gk::Graph::Instance(v8::Isolate* isolate) noexcept {
	const int argc = 0;
	v8::Local<v8::Value> argv[argc] = {};
	auto ctor = GK_FUNCTION(constructor_);
	return node::ObjectWrap::Unwrap<gk::Graph>(ctor->NewInstance(argc, argv));
}

GK_INIT(gk::Graph::Init) {
	GK_SCOPE();

	auto t = GK_TEMPLATE(New);
	t->SetClassName(GK_STRING(symbol));
	t->InstanceTemplate()->SetInternalFieldCount(1);
	t->InstanceTemplate()->SetIndexedPropertyHandler(IndexGetter, IndexSetter, 0, IndexDeleter, IndexEnumerator);
	t->InstanceTemplate()->SetNamedPropertyHandler(PropertyGetter, PropertySetter, 0, PropertyDeleter, PropertyEnumerator);

	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_COUNT, Count);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_INSERT, Insert);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_REMOVE, Remove);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_CLEAR, Clear);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_FIND, Find);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_CREATE_ENTITY, CreateEntity);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_CREATE_ACTION, CreateAction);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_CREATE_BOND, CreateBond);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_GROUP, Group);

	constructor_.Reset(isolate, t->GetFunction());
	exports->Set(GK_STRING(symbol), t->GetFunction());
}

GK_METHOD(gk::Graph::New) {
	GK_SCOPE();

	if (args.IsConstructCall()) {
		auto obj = new gk::Graph{};
		obj->coordinator()->sync(isolate);
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 0;
		v8::Local<v8::Value> argv[argc] = {};
		auto ctor = GK_FUNCTION(constructor_);
		GK_RETURN(ctor->NewInstance(argc, argv));
	}
}

GK_METHOD(gk::Graph::Count) {
	GK_SCOPE();
	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	GK_RETURN(GK_NUMBER(graph->coordinator()->nodeGraph()->count()));
}

GK_METHOD(gk::Graph::Insert) {
	GK_SCOPE();

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto node = node::ObjectWrap::Unwrap<gk::Node>(args[0]->ToObject());
	if (node->indexed()) {
		GK_RETURN(GK_BOOLEAN(false));
	}

	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	GK_RETURN(GK_BOOLEAN(graph->coordinator()->insertNode(isolate, node)));
}

GK_METHOD(gk::Graph::Remove) {
	GK_SCOPE();

	if (0 == args.Length()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	// check if the object is a Node
	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	if (args[0]->IsObject()) {
		auto node = node::ObjectWrap::Unwrap<gk::Node>(args[0]->ToObject());
		GK_RETURN(GK_BOOLEAN(graph->coordinator()->removeNode(node->nodeClass(), node->type(), node->id())));
	}

	// check if granular details are passed
	if (args[0]->IntegerValue() && args[1]->IsString() && args[2]->IntegerValue()) {
		auto nodeClass = gk::NodeClassFromInt(args[0]->IntegerValue());
		v8::String::Utf8Value type(args[1]->ToString());
		auto id = args[1]->IntegerValue();
		GK_RETURN(GK_BOOLEAN(graph->coordinator()->removeNode(nodeClass, *type, id)));
	}

	// throw an exception if we are here
	GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
}

GK_METHOD(gk::Graph::Clear) {
	GK_SCOPE();
	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	graph->cleanUp();
	GK_RETURN(GK_UNDEFINED());
}

GK_METHOD(gk::Graph::Find) {
	GK_SCOPE();

	if ((GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT > args[0]->IntegerValue() || GK_SYMBOL_NODE_CLASS_BOND_CONSTANT < args[0]->IntegerValue())) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct NodeClass value.]");
	}

	if (!args[1]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Type value.]");
	}

	if (!args[2]->IntegerValue()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct ID value.]");
	}

	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	auto cluster = graph->coordinator()->nodeGraph()->findByKey(gk::NodeClassFromInt(args[0]->IntegerValue()));
	if (cluster && 0 < cluster->count()) {
		v8::String::Utf8Value type(args[1]->ToString());
		auto index = cluster->findByKey(*type);
		if (index && 0 < index->count()) {
			auto node = index->findByKey(args[2]->IntegerValue());
			if (node) {
				GK_RETURN(node->handle());
			}
		}
	}
	GK_RETURN(GK_UNDEFINED());
}

GK_INDEX_GETTER(gk::Graph::IndexGetter) {
	GK_SCOPE();
	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	if (++index > graph->coordinator()->nodeGraph()->count()) {
		GK_EXCEPTION("[GraphKit Error: Index out of range.]");
	}
	GK_RETURN(graph->coordinator()->nodeGraph()->select(index)->handle());
}

GK_INDEX_SETTER(gk::Graph::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Graph values may not be set.]");
}

GK_INDEX_DELETER(gk::Graph::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Graph values may not be deleted.]");
}

GK_INDEX_ENUMERATOR(gk::Graph::IndexEnumerator) {
	GK_SCOPE();
	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	auto count = graph->coordinator()->nodeGraph()->count();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, count);
	for (auto i = count - 1; 0 <= i; --i) {
		array->Set(i, GK_INTEGER(i));
	}
	GK_RETURN(array);
}

GK_PROPERTY_GETTER(gk::Graph::PropertyGetter) {
	GK_SCOPE();
	v8::String::Utf8Value p(property);
	if (0 != strcmp(*p, GK_SYMBOL_OPERATION_COUNT) &&
	0 != strcmp(*p, GK_SYMBOL_OPERATION_INSERT) &&
	0 != strcmp(*p, GK_SYMBOL_OPERATION_REMOVE) &&
	0 != strcmp(*p, GK_SYMBOL_OPERATION_CLEAR) &&
	0 != strcmp(*p, GK_SYMBOL_OPERATION_FIND)) {
		auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
		auto cluster = graph->coordinator()->nodeGraph()->findByKey(gk::NodeClassFromString(*p));
		if (cluster) {
			GK_RETURN(cluster->handle());
		}
	}
}

GK_PROPERTY_SETTER(gk::Graph::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Graph values may not be set.]");
}

GK_PROPERTY_DELETER(gk::Graph::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Graph values may not be deleted.]");
}

GK_PROPERTY_ENUMERATOR(gk::Graph::PropertyEnumerator) {
	GK_SCOPE();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, 0);
	GK_RETURN(array);
}

GK_METHOD(gk::Graph::CreateEntity) {
	GK_SCOPE();
	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a Type value.]");
	}
	v8::String::Utf8Value type(args[0]->ToString());
	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	auto node = gk::Entity::Instance(isolate, *type);
	graph->coordinator()->insertNode(isolate, node);
	GK_RETURN(node->handle());
}

GK_METHOD(gk::Graph::CreateAction) {
	GK_SCOPE();
	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a Type value.]");
	}
	v8::String::Utf8Value type(args[0]->ToString());
	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	auto node = gk::Action<gk::Entity>::Instance(isolate, *type);
	graph->coordinator()->insertNode(isolate, node);
	GK_RETURN(node->handle());
}

GK_METHOD(gk::Graph::CreateBond) {
	GK_SCOPE();
	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a Type value.]");
	}
	v8::String::Utf8Value type(args[0]->ToString());
	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	auto node = gk::Bond<gk::Entity>::Instance(isolate, *type);
	graph->coordinator()->insertNode(isolate, node);
	GK_RETURN(node->handle());
}

GK_METHOD(gk::Graph::Group) {
	GK_SCOPE();
	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a Group name.]");
	}
	v8::String::Utf8Value group(args[0]->ToString());
	auto graph = node::ObjectWrap::Unwrap<gk::Graph>(args.Holder());
	auto set = graph->coordinator()->groupGraph()->findByKey(*group);
	GK_RETURN(set->handle());
}