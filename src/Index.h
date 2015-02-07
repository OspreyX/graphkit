/**
* Copyright (C) 2015 GraphKit, Inc. <http://graphkit.io> and Daniel Dahan <daniel@graphkit.io>
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
*
* Index.h
*
* A Class used to manage Nodes.
*/

#ifndef GRAPHKIT_SRC_INDEX_H
#define GRAPHKIT_SRC_INDEX_H

#include <cstring>
#include <utility>
#include <string>
#include "exports.h"
#include "symbols.h"
#include "ObjectWrapPolicy.h"
#include "RedBlackTree.h"
#include "NodeClass.h"

namespace gk {
	template <
		typename T,
		typename K = long long,
		typename O = long long
	>
	class Index : public gk::ObjectWrapPolicy,
				  public gk::RedBlackTree<T, true, K, O> {
	public:
		Index(const gk::NodeClass& nodeClass, const std::string& type) noexcept;
		virtual ~Index();
		Index(const Index&) = default;
		Index& operator= (const Index&) = default;
		Index(Index&&) = default;
		Index& operator= (Index&&) = default;

		using Node = T;

		const gk::NodeClass& nodeClass() const noexcept;
		const std::string& type() const noexcept;

		O incrementId() noexcept;
		O decrementId() noexcept;

		bool insert(T* node) noexcept;
		bool remove(T* node) noexcept;
		bool remove(const int k) noexcept;
		void cleanUp() noexcept;

		static gk::Index<T, K, O>* Instance(v8::Isolate* isolate, gk::NodeClass& nodeClass, std::string& type) noexcept;
		static GK_INIT(Init);

	private:
		const gk::NodeClass nodeClass_;
		const std::string type_;
		O ids_;

		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
		static GK_METHOD(Size);
		static GK_METHOD(Insert);
		static GK_METHOD(Remove);
		static GK_METHOD(Clear);
		static GK_METHOD(Find);
		static GK_METHOD(NodeClassToString);
		static GK_INDEX_GETTER(IndexGetter);
		static GK_INDEX_SETTER(IndexSetter);
		static GK_INDEX_DELETER(IndexDeleter);
		static GK_PROPERTY_GETTER(PropertyGetter);
		static GK_PROPERTY_SETTER(PropertySetter);
		static GK_PROPERTY_DELETER(PropertyDeleter);
		static GK_PROPERTY_ENUMERATOR(PropertyEnumerator);
	};
}

template <typename T, typename K, typename O>
v8::Persistent<v8::Function> gk::Index<T, K, O>::constructor_;

template <typename T, typename K, typename O>
gk::Index<T, K, O>::Index(const gk::NodeClass& nodeClass, const std::string& type) noexcept
	: gk::ObjectWrapPolicy{},
	  gk::RedBlackTree<T, true, K, O>{},
	  nodeClass_{nodeClass},
	  type_{type},
	  ids_{} {}

template <typename T, typename K, typename O>
gk::Index<T, K, O>::~Index() {
	cleanUp();
}

template <typename T, typename K, typename O>
const gk::NodeClass& gk::Index<T, K, O>::nodeClass() const noexcept {
	return nodeClass_;
}

template <typename T, typename K, typename O>
const std::string& gk::Index<T, K, O>::type() const noexcept {
	return type_;
}

template <typename T, typename K, typename O>
O gk::Index<T, K, O>::incrementId() noexcept {
	return ++ids_;
}

template <typename T, typename K, typename O>
O gk::Index<T, K, O>::decrementId() noexcept {
	return --ids_;
}

template  <typename T, typename K, typename O>
bool gk::Index<T, K, O>::insert(T* node) noexcept {
	node->id(incrementId());
	return gk::RedBlackTree<T, true, K, O>::insert(node->id(), node, [&](T* n) {
		n->indexed(true);
		n->Ref();
	});
}

template  <typename T, typename K, typename O>
bool gk::Index<T, K, O>::remove(T* node) noexcept {
	if (!node->indexed()) {
		return false;
	}
	return gk::RedBlackTree<T, true, K, O>::remove(node->id(), [&](T* n) {
		n->indexed(false);
		n->Unref();
	});
}

template  <typename T, typename K, typename O>
bool gk::Index<T, K, O>::remove(const int k) noexcept {
	return gk::RedBlackTree<T, true, K, O>::remove(k, [&](T* n) {
		n->indexed(false);
		n->Unref();
	});
}

template  <typename T, typename K, typename O>
void gk::Index<T, K, O>::cleanUp() noexcept {
	this->clear([&](T *n) {
		n->indexed(false);
		n->Unref();
	});
}

template  <typename T, typename K, typename O>
GK_METHOD(gk::Index<T, K, O>::NodeClassToString) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Index<T, K, O>>(args.Holder());
	GK_RETURN(GK_STRING(gk::NodeClassToString(i->nodeClass())));
}

template <typename T, typename K, typename O>
gk::Index<T, K, O>* gk::Index<T, K, O>::Instance(v8::Isolate* isolate, gk::NodeClass& nodeClass, std::string& type) noexcept {
	const int argc = 2;
	v8::Local<v8::Value> argv[argc] = {GK_INTEGER(gk::NodeClassToInt(nodeClass)), GK_STRING(type.c_str())};
	auto cons = v8::Local<v8::Function>::New(isolate, constructor_);
	return node::ObjectWrap::Unwrap<gk::Index<T, K, O>>(cons->NewInstance(argc, argv));
}

template <typename T, typename K, typename O>
GK_INIT(gk::Index<T, K, O>::Init) {
	GK_SCOPE();

	auto t = GK_TEMPLATE(New);
	t->SetClassName(GK_STRING(symbol));
	t->InstanceTemplate()->SetInternalFieldCount(1);
	t->InstanceTemplate()->SetIndexedPropertyHandler(IndexGetter, IndexSetter, 0, IndexDeleter);
	t->InstanceTemplate()->SetNamedPropertyHandler(PropertyGetter, PropertySetter, 0, PropertyDeleter);

	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_SIZE, Size);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_INSERT, Insert);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_REMOVE, Remove);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_CLEAR, Clear);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_FIND, Find);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_NODE_CLASS_TO_STRING, NodeClassToString);

	constructor_.Reset(isolate, t->GetFunction());
	exports->Set(GK_STRING(symbol), t->GetFunction());
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Index<T, K, O>::New) {
	GK_SCOPE();

	if (!args[1]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a Type value.]");
	}

	if (args[0]->IsNumber() && (GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT > args[0]->IntegerValue() || GK_SYMBOL_NODE_CLASS_BOND_CONSTANT < args[0]->IntegerValue())) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct NodeClass value.]");
	}

	if (args.IsConstructCall()) {
		auto nc = args[0]->IsNumber() ? gk::NodeClassFromInt(args[0]->IntegerValue()) : gk::NodeClass::Entity;
		v8::String::Utf8Value type(args[1]->ToString());
		auto obj = new gk::Index<T, K, O>{nc, *type};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 2;
		v8::Local<v8::Value> argv[argc] = {args[0], args[1]};
		auto cons = v8::Local<v8::Function>::New(isolate, constructor_);
		GK_RETURN(cons->NewInstance(argc, argv));
	}
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Index<T, K, O>::Size) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Index<T, K, O>>(args.Holder());
	GK_RETURN(GK_NUMBER(i->size()));
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Index<T, K, O>::Insert) {
	GK_SCOPE();

	if (0 == args.Length() || !args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<T>(args[0]->ToObject());
	if (n->indexed()) {
		GK_RETURN(GK_BOOLEAN(false));
	}

	auto i = node::ObjectWrap::Unwrap<gk::Index<T, K, O>>(args.Holder());
	if (i->nodeClass() != n->nodeClass()) {
		std::string s1 {gk::NodeClassToString(i->nodeClass())};
		std::string s2 {gk::NodeClassToString(n->nodeClass())};
		GK_EXCEPTION(("[GraphKit Error: Invalid NodeClass. Expecting " + s1 + " and got " + s2 + ".]").c_str());
	}

	if (i->type() != n->type()) {
		GK_EXCEPTION(("[GraphKit Error: Invalid Type. Expecting " + i->type() + " and got " + n->type() + ".]").c_str());
	}
	GK_RETURN(GK_BOOLEAN(i->insert(n)));
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Index<T, K, O>::Remove) {
	GK_SCOPE();

	if (0 == args.Length()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto i = node::ObjectWrap::Unwrap<gk::Index<T, K, O>>(args.Holder());

	if (args[0]->IsNumber()) {
		GK_RETURN(GK_BOOLEAN(i->remove(args[0]->IntegerValue())));
	}

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<T>(args[0]->ToObject());
	if (i->nodeClass() != n->nodeClass()) {
		std::string s1 {gk::NodeClassToString(i->nodeClass())};
		std::string s2 {gk::NodeClassToString(n->nodeClass())};
		GK_EXCEPTION(("[GraphKit Error: Invalid NodeClass. Expecting " + s1 + " and got " + s2 + ".]").c_str());
	}

	if (i->type() != n->type()) {
		GK_EXCEPTION(("[GraphKit Error: Invalid Type. Expecting " + i->type() + " and got " + n->type() + ".]").c_str());
	}
	GK_RETURN(GK_BOOLEAN(i->remove(n)));
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Index<T, K, O>::Clear) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Index<T, K, O>>(args.Holder());
	i->cleanUp();
	GK_RETURN(GK_UNDEFINED());
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Index<T, K, O>::Find) {
	GK_SCOPE();
	if (!args[0]->IsNumber() || 0 > args[0]->IntegerValue()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct ID value.]");
	}
	auto i = node::ObjectWrap::Unwrap<gk::Index<T, K, O>>(args.Holder());
	if (0 < i->size()) {
		auto n = i->findByKey(args[0]->IntegerValue());
		if (n) {
			GK_RETURN(n->handle());
		}
	}
	GK_RETURN(GK_UNDEFINED());
}

template <typename T, typename K, typename O>
GK_INDEX_GETTER(gk::Index<T, K, O>::IndexGetter) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Index<T, K, O>>(args.Holder());
	if (++index > i->size()) {
		GK_EXCEPTION("[GraphKit Error: Index out of range.]");
	}
	GK_RETURN(i->select(index)->handle());
}

template <typename T, typename K, typename O>
GK_INDEX_SETTER(gk::Index<T, K, O>::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be set.]");
}

template <typename T, typename K, typename O>
GK_INDEX_DELETER(gk::Index<T, K, O>::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be deleted.]");
}

template <typename T, typename K, typename O>
GK_PROPERTY_GETTER(gk::Index<T, K, O>::PropertyGetter) {
	GK_SCOPE();
	v8::String::Utf8Value p(property);
	auto i = node::ObjectWrap::Unwrap<gk::Index<T, K, O>>(args.Holder());
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS)) {
		GK_RETURN(GK_INTEGER(gk::NodeClassToInt(i->nodeClass())));
	}
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_TYPE)) {
		GK_RETURN(GK_STRING(i->type().c_str()));
	}
}

template <typename T, typename K, typename O>
GK_PROPERTY_SETTER(gk::Index<T, K, O>::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be set.]");
}

template <typename T, typename K, typename O>
GK_PROPERTY_DELETER(gk::Index<T, K, O>::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be deleted.]");
}

template <typename T, typename K, typename O>
GK_PROPERTY_ENUMERATOR(gk::Index<T, K, O>::PropertyEnumerator) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Index<T, K, O>>(args.Holder());
	auto is = i->size();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, is);
	for (auto j = is - 1; 0 <= j; --j) {
		auto node = i->node(j + 1);
		array->Set(j, GK_INTEGER(node->order()));
	}
	GK_RETURN(array);
}

#endif