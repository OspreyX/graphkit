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

#include "Multiset.h"
#include "symbols.h"
#include "NodeClass.h"

GK_CONSTRUCTOR(gk::Multiset::constructor_);

gk::Multiset::Multiset() noexcept
	: gk::Export{},
	  gk::RedBlackTree<gk::Node, false, std::string>{} {}

gk::Multiset::~Multiset() {
	cleanUp();
}

bool gk::Multiset::insert(gk::Node* node) noexcept {
	return gk::RedBlackTree<gk::Node, false, std::string>::insert(node->hash(), node, [](gk::Node* n) {
		n->Ref();
	});
}

bool gk::Multiset::remove(const std::string& k) noexcept {
	return gk::RedBlackTree<gk::Node, false, std::string>::remove(k, [](gk::Node* n) {
		n->Unref();
	});
}

void gk::Multiset::cleanUp() noexcept {
	this->clear([](gk::Node *n) {
		n->Unref();
	});
}

gk::Multiset* gk::Multiset::Instance(v8::Isolate* isolate) noexcept {
	const int argc = 0;
	v8::Local<v8::Value> argv[argc] = {};
	auto ctor = GK_FUNCTION(constructor_);
	return node::ObjectWrap::Unwrap<gk::Multiset>(ctor->NewInstance(argc, argv));
}

GK_INIT(gk::Multiset::Init) {
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

	constructor_.Reset(isolate, t->GetFunction());
	exports->Set(GK_STRING(symbol), t->GetFunction());
}

GK_METHOD(gk::Multiset::New) {
	GK_SCOPE();

	if (args.IsConstructCall()) {
		auto obj = new gk::Multiset{};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 0;
		v8::Local<v8::Value> argv[argc] = {};
		auto ctor = GK_FUNCTION(constructor_);
		GK_RETURN(ctor->NewInstance(argc, argv));
	}
}

GK_METHOD(gk::Multiset::Insert) {
	GK_SCOPE();

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto node = node::ObjectWrap::Unwrap<gk::Node>(args[0]->ToObject());
	if (!node->indexed()) {
		GK_EXCEPTION("[GraphKit Error: NodeClass has not been indexed.]");
	}

	auto multiset = node::ObjectWrap::Unwrap<gk::Multiset>(args.Holder());
	GK_RETURN(GK_BOOLEAN(multiset->insert(node)));
}

GK_METHOD(gk::Multiset::Remove) {
	GK_SCOPE();

	if (0 == args.Length()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto multiset = node::ObjectWrap::Unwrap<gk::Multiset>(args.Holder());
	if (args[0]->IntegerValue() && args[1]->IsString() && args[2]->IntegerValue()) {
		v8::String::Utf8Value type(args[1]->ToString());
		auto key = std::string{std::string(gk::NodeClassToString(gk::NodeClassFromInt(args[0]->IntegerValue()))) + *type + std::to_string(args[2]->IntegerValue())};
		GK_RETURN(GK_BOOLEAN(multiset->remove(key)));
	}

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto node = node::ObjectWrap::Unwrap<gk::Node>(args[0]->ToObject());
	GK_RETURN(GK_BOOLEAN(multiset->remove(node->hash())));
}

GK_METHOD(gk::Multiset::Clear) {
	GK_SCOPE();
	auto multiset = node::ObjectWrap::Unwrap<gk::Multiset>(args.Holder());
	multiset->cleanUp();
	GK_RETURN(GK_UNDEFINED());
}

GK_METHOD(gk::Multiset::Find) {
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

	auto multiset = node::ObjectWrap::Unwrap<gk::Multiset>(args.Holder());
	v8::String::Utf8Value type(args[1]->ToString());
	auto key = std::to_string(args[0]->IntegerValue()) + *type + std::to_string(args[2]->IntegerValue());
	auto node = multiset->findByKey(key);
	if (node) {
		GK_RETURN(node->handle());
	}
	GK_RETURN(GK_UNDEFINED());
}

GK_INDEX_GETTER(gk::Multiset::IndexGetter) {
	GK_SCOPE();
	auto multiset = node::ObjectWrap::Unwrap<gk::Multiset>(args.Holder());
	if (++index > multiset->count()) {
		GK_EXCEPTION("[GraphKit Error: Multiset out of range.]");
	}
	GK_RETURN(multiset->select(index)->handle());
}

GK_INDEX_SETTER(gk::Multiset::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Multiset values may not be set.]");
}

GK_INDEX_DELETER(gk::Multiset::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Multiset values may not be deleted.]");
}

GK_INDEX_ENUMERATOR(gk::Multiset::IndexEnumerator) {
	GK_SCOPE();
	auto multiset = node::ObjectWrap::Unwrap<gk::Multiset>(args.Holder());
	auto count = multiset->count();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, count);
	for (auto i = count - 1; 0 <= i; --i) {
		array->Set(i, GK_INTEGER(i));
	}
	GK_RETURN(array);
}

GK_PROPERTY_GETTER(gk::Multiset::PropertyGetter) {
	GK_SCOPE();

	v8::String::Utf8Value p(property);
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_COUNT)) {
		auto multiset = node::ObjectWrap::Unwrap<gk::Multiset>(args.Holder());
		GK_RETURN(GK_INTEGER(multiset->count()));
	}
}

GK_PROPERTY_SETTER(gk::Multiset::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Multiset values may not be set.]");
}

GK_PROPERTY_DELETER(gk::Multiset::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Multiset values may not be deleted.]");
}

GK_PROPERTY_ENUMERATOR(gk::Multiset::PropertyEnumerator) {
	GK_SCOPE();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, 0);
	GK_RETURN(array);
}