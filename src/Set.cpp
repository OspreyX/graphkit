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

#include "Set.h"
#include "symbols.h"
#include "NodeClass.h"

GK_CONSTRUCTOR(gk::Set::constructor_);

gk::Set::Set() noexcept
	: gk::Export{},
	  gk::RedBlackTree<gk::Node, true, std::string>{} {}

gk::Set::~Set() {
	cleanUp();
}

bool gk::Set::insert(gk::Node* node) noexcept {
	return gk::RedBlackTree<gk::Node, true, std::string>::insert(node->hash(), node, [](gk::Node* n) {
		n->Ref();
	});
}

bool gk::Set::remove(const std::string& k) noexcept {
	return gk::RedBlackTree<gk::Node, true, std::string>::remove(k, [](gk::Node* n) {
		n->Unref();
	});
}

void gk::Set::cleanUp() noexcept {
	this->clear([](gk::Node *n) {
		n->Unref();
	});
}

gk::Set* gk::Set::Instance(v8::Isolate* isolate) noexcept {
	const int argc = 0;
	v8::Local<v8::Value> argv[argc] = {};
	auto ctor = GK_FUNCTION(constructor_);
	return node::ObjectWrap::Unwrap<gk::Set>(ctor->NewInstance(argc, argv));
}

GK_INIT(gk::Set::Init) {
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

	constructor_.Reset(isolate, t->GetFunction());
	exports->Set(GK_STRING(symbol), t->GetFunction());
}

GK_METHOD(gk::Set::New) {
	GK_SCOPE();

	if (args.IsConstructCall()) {
		auto obj = new gk::Set{};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 0;
		v8::Local<v8::Value> argv[argc] = {};
		auto ctor = GK_FUNCTION(constructor_);
		GK_RETURN(ctor->NewInstance(argc, argv));
	}
}

GK_METHOD(gk::Set::Count) {
	GK_SCOPE();
	auto s = node::ObjectWrap::Unwrap<gk::Set>(args.Holder());
	GK_RETURN(GK_NUMBER(s->count()));
}

GK_METHOD(gk::Set::Insert) {
	GK_SCOPE();

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<gk::Node>(args[0]->ToObject());
	if (!n->indexed()) {
		GK_EXCEPTION("[GraphKit Error: NodeClass has not been indexed.]");
	}

	auto s = node::ObjectWrap::Unwrap<gk::Set>(args.Holder());
	GK_RETURN(GK_BOOLEAN(s->insert(n)));
}

GK_METHOD(gk::Set::Remove) {
	GK_SCOPE();

	if (0 == args.Length()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto s = node::ObjectWrap::Unwrap<gk::Set>(args.Holder());
	if (args[0]->IntegerValue() && args[1]->IsString() && args[2]->IntegerValue()) {
		v8::String::Utf8Value type(args[1]->ToString());
		auto k = std::string{std::string(gk::NodeClassToString(gk::NodeClassFromInt(args[0]->IntegerValue()))) + *type + std::to_string(args[2]->IntegerValue())};
		GK_RETURN(GK_BOOLEAN(s->remove(k)));
	}

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<gk::Node>(args[0]->ToObject());
	GK_RETURN(GK_BOOLEAN(s->remove(n->hash())));
}

GK_METHOD(gk::Set::Clear) {
	GK_SCOPE();
	auto s = node::ObjectWrap::Unwrap<gk::Set>(args.Holder());
	s->cleanUp();
	GK_RETURN(GK_UNDEFINED());
}

GK_METHOD(gk::Set::Find) {
	GK_SCOPE();

	if (GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT > args[0]->IntegerValue() || GK_SYMBOL_NODE_CLASS_BOND_CONSTANT < args[0]->IntegerValue()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct NodeClass value.]");
	}

	if (!args[1]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Type value.]");
	}

	if (!args[2]->IntegerValue()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct ID value.]");
	}

	auto s = node::ObjectWrap::Unwrap<gk::Set>(args.Holder());
	v8::String::Utf8Value type(args[1]->ToString());
	auto k = std::to_string(args[0]->IntegerValue()) + *type + std::to_string(args[2]->IntegerValue());
	auto n = s->findByKey(k);
	if (n) {
		GK_RETURN(n->handle());
	}
	GK_RETURN(GK_UNDEFINED());
}

GK_INDEX_GETTER(gk::Set::IndexGetter) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Set>(args.Holder());
	if (++index > i->count()) {
		GK_EXCEPTION("[GraphKit Error: Set out of range.]");
	}
	GK_RETURN(i->select(index)->handle());
}

GK_INDEX_SETTER(gk::Set::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Set values may not be set.]");
}

GK_INDEX_DELETER(gk::Set::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Set values may not be deleted.]");
}

GK_INDEX_ENUMERATOR(gk::Set::IndexEnumerator) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Set>(args.Holder());
	auto is = i->count();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, is);
	for (auto j = is - 1; 0 <= j; --j) {
		array->Set(j, GK_INTEGER(j));
	}
	GK_RETURN(array);
}

GK_PROPERTY_GETTER(gk::Set::PropertyGetter) {
	GK_SCOPE();
}

GK_PROPERTY_SETTER(gk::Set::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Set values may not be set.]");
}

GK_PROPERTY_DELETER(gk::Set::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Set values may not be deleted.]");
}

GK_PROPERTY_ENUMERATOR(gk::Set::PropertyEnumerator) {
	GK_SCOPE();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, 0);
	GK_RETURN(array);
}