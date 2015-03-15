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

#include <utility>
#include "Cluster.h"
#include "symbols.h"

GK_CONSTRUCTOR(gk::Cluster::constructor_);

gk::Cluster::Cluster(const gk::NodeClass& nodeClass) noexcept
	: gk::Export{},
	  gk::RedBlackTree<gk::Index, true, std::string>{},
	  nodeClass_{std::move(nodeClass)} {}

gk::Cluster::~Cluster() {
	this->clear([](gk::Index* index) {
		index->cleanUp();
		index->Unref();
	});
}

const gk::NodeClass& gk::Cluster::nodeClass() const noexcept {
	return nodeClass_;
}

bool gk::Cluster::insert(v8::Isolate* isolate, gk::Node* node) noexcept {
	auto index = this->findByKey(node->type());
	if (!index) {
		auto nodeClass = node->nodeClass();
		auto type = node->type();
		index = gk::Index::Instance(isolate, nodeClass, type);
		if (!gk::RedBlackTree<gk::Index, true, std::string>::insert(index->type(), index, [](gk::Index* index) {
			index->Ref();
		})) {
			return false;
		}
	}
	return index->insert(node);
}

void gk::Cluster::cleanUp() noexcept {
	for (auto i = this->count(); 0 < i; --i) {
		this->select(i)->cleanUp();
	}
}

GK_METHOD(gk::Cluster::NodeClassToString) {
	GK_SCOPE();
	auto cluster = node::ObjectWrap::Unwrap<gk::Cluster>(args.Holder());
	GK_RETURN(GK_STRING(gk::NodeClassToString(cluster->nodeClass())));
}

gk::Cluster* gk::Cluster::Instance(v8::Isolate* isolate, gk::NodeClass& nodeClass) noexcept {
	const int argc = 1;
	v8::Local<v8::Value> argv[argc] = {GK_INTEGER(gk::NodeClassToInt(nodeClass))};
	auto ctor = GK_FUNCTION(constructor_);
	return node::ObjectWrap::Unwrap<gk::Cluster>(ctor->NewInstance(argc, argv));
}

GK_INIT(gk::Cluster::Init) {
	GK_SCOPE();

	auto t = GK_TEMPLATE(New);
	t->SetClassName(GK_STRING(symbol));
	t->InstanceTemplate()->SetInternalFieldCount(1);
	t->InstanceTemplate()->SetIndexedPropertyHandler(IndexGetter, IndexSetter, 0, IndexDeleter, IndexEnumerator);
	t->InstanceTemplate()->SetNamedPropertyHandler(PropertyGetter, PropertySetter, 0, PropertyDeleter, PropertyEnumerator);

	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_INSERT, Insert);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_REMOVE, Remove);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_CLEAR, Clear);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_FIND, Find);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_NODE_CLASS_TO_STRING, NodeClassToString);

	constructor_.Reset(isolate, t->GetFunction());
	exports->Set(GK_STRING(symbol), t->GetFunction());
}

GK_METHOD(gk::Cluster::New) {
	GK_SCOPE();

	if (GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT > args[0]->IntegerValue() || GK_SYMBOL_NODE_CLASS_BOND_CONSTANT < args[0]->IntegerValue()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct NodeClass value.]");
	}

	if (args.IsConstructCall()) {
		auto nodeClass = gk::NodeClassFromInt(args[0]->IntegerValue());
		auto obj = new gk::Cluster{nodeClass};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 1;
		v8::Local<v8::Value> argv[argc] = {args[0]};
		auto ctor = GK_FUNCTION(constructor_);
		GK_RETURN(ctor->NewInstance(argc, argv));
	}
}

GK_METHOD(gk::Cluster::Insert) {
	GK_SCOPE();

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto node = node::ObjectWrap::Unwrap<gk::Node>(args[0]->ToObject());
	if (node->indexed()) {
		GK_RETURN(GK_BOOLEAN(false));
	}

	auto cluster = node::ObjectWrap::Unwrap<gk::Cluster>(args.Holder());
	if (cluster->nodeClass() != node->nodeClass()) {
		std::string s1 {gk::NodeClassToString(cluster->nodeClass())};
		std::string s2 {gk::NodeClassToString(node->nodeClass())};
		GK_EXCEPTION(("[GraphKit Error: Invalid NodeClass. Expecting " + s1 + " and got " + s2 + ".]").c_str());
	}

	GK_RETURN(GK_BOOLEAN(cluster->insert(isolate, node)));
}

GK_METHOD(gk::Cluster::Remove) {
	GK_SCOPE();

	if (0 == args.Length()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto cluster = node::ObjectWrap::Unwrap<gk::Cluster>(args.Holder());
	if (args[0]->IsString() && args[1]->IntegerValue()) {
		v8::String::Utf8Value type(args[0]->ToString());
		auto index = cluster->findByKey(*type);
		if (index) {
			GK_RETURN(GK_BOOLEAN(index->remove(args[1]->IntegerValue())));
		}
		GK_RETURN(GK_BOOLEAN(false));
	}

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto node = node::ObjectWrap::Unwrap<gk::Node>(args[0]->ToObject());
	if (cluster->nodeClass() != node->nodeClass()) {
		std::string s1 {gk::NodeClassToString(cluster->nodeClass())};
		std::string s2 {gk::NodeClassToString(node->nodeClass())};
		GK_EXCEPTION(("[GraphKit Error: Invalid NodeClass. Expecting " + s1 + " and got " + s2 + ".]").c_str());
	}

	auto index = cluster->findByKey(node->type());
	if (index) {
		GK_RETURN(GK_BOOLEAN(index->remove(node)));
	}
	GK_RETURN(GK_BOOLEAN(false));
}

GK_METHOD(gk::Cluster::Clear) {
	GK_SCOPE();
	auto cluster = node::ObjectWrap::Unwrap<gk::Cluster>(args.Holder());
	cluster->cleanUp();
	GK_RETURN(GK_UNDEFINED());
}

GK_METHOD(gk::Cluster::Find) {
	GK_SCOPE();

	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Type value.]");
	}

	if (args[1]->IntegerValue()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct ID value.]");
	}

	auto cluster = node::ObjectWrap::Unwrap<gk::Cluster>(args.Holder());
	v8::String::Utf8Value type(args[0]->ToString());
	auto index = cluster->findByKey(*type);
	if (index && 0 < index->count()) {
		auto node = index->findByKey(args[1]->IntegerValue());
		if (node) {
			GK_RETURN(node->handle());
		}
	}
	GK_RETURN(GK_UNDEFINED());
}

GK_INDEX_GETTER(gk::Cluster::IndexGetter) {
	GK_SCOPE();
	auto cluster = node::ObjectWrap::Unwrap<gk::Cluster>(args.Holder());
	if (++index > cluster->count()) {
		GK_EXCEPTION("[GraphKit Error: Index out of range.]");
	}
	GK_RETURN(cluster->select(index)->handle());
}

GK_INDEX_SETTER(gk::Cluster::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Cluster values may not be set.]");
}

GK_INDEX_DELETER(gk::Cluster::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Cluster values may not be deleted.]");
}

GK_INDEX_ENUMERATOR(gk::Cluster::IndexEnumerator) {
	GK_SCOPE();
	auto cluster = node::ObjectWrap::Unwrap<gk::Cluster>(args.Holder());
	auto count = cluster->count();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, count);
	for (auto i = count - 1; 0 <= i; --i) {
		array->Set(i, GK_INTEGER(i));
	}
	GK_RETURN(array);
}

GK_PROPERTY_GETTER(gk::Cluster::PropertyGetter) {
	GK_SCOPE();
	v8::String::Utf8Value p(property);
	auto cluster = node::ObjectWrap::Unwrap<gk::Cluster>(args.Holder());
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS)) {
		GK_RETURN(GK_INTEGER(gk::NodeClassToInt(cluster->nodeClass())));
	}
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_COUNT)) {
		GK_RETURN(GK_INTEGER(cluster->count()));
	}
	if (0 != strcmp(*p, GK_SYMBOL_OPERATION_COUNT) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_INSERT) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_REMOVE) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_CLEAR) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_FIND) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS_TO_STRING)) {
		auto index = cluster->findByKey(*p);
		if (index) {
			GK_RETURN(index->handle());
		}
	}
}

GK_PROPERTY_SETTER(gk::Cluster::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Cluster values may not be set.]");
}

GK_PROPERTY_DELETER(gk::Cluster::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Cluster values may not be deleted.]");
}

GK_PROPERTY_ENUMERATOR(gk::Cluster::PropertyEnumerator) {
	GK_SCOPE();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, 0);
	GK_RETURN(array);
}
