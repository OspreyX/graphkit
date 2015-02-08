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
* Multiset.h
*
* A Class used to manage Nodes.
*/

#ifndef GRAPHKIT_SRC_MULTISET_H
#define GRAPHKIT_SRC_MULTISET_H

#include <string>
#include "exports.h"
#include "symbols.h"
#include "Export.h"
#include "RedBlackTree.h"
#include "NodeClass.h"

namespace gk {
	template <
		typename T,
		typename O = long long
	>
	class Multiset : public gk::Export,
					 public gk::RedBlackTree<T, false, std::string, O> {
	public:
		explicit Multiset() noexcept;
		virtual ~Multiset();
		Multiset(const Multiset&) = default;
		Multiset& operator= (const Multiset&) = default;
		Multiset(Multiset&&) = default;
		Multiset& operator= (Multiset&&) = default;

		bool insert(v8::Isolate* isolate, T* node) noexcept;
		bool remove(const std::string& k) noexcept;
		void cleanUp() noexcept;

		static gk::Multiset<T, O>* Instance(v8::Isolate* isolate) noexcept;
		static GK_INIT(Init);

	private:
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

template <typename T, typename O>
GK_CONSTRUCTOR(gk::Multiset<T, O>::constructor_);

template <typename T, typename O>
gk::Multiset<T, O>::Multiset() noexcept
	: gk::Export{},
	  gk::RedBlackTree<T, false, std::string, O>{} {}

template <typename T, typename O>
gk::Multiset<T, O>::~Multiset() {
	cleanUp();
}

template <typename T, typename O>
bool gk::Multiset<T, O>::insert(v8::Isolate* isolate, T* node) noexcept {
	if (!node->indexed()) {
		return false;
	}
	return gk::RedBlackTree<T, false, std::string, O>::insert(node->hash(), node, [&](T* n) {
		n->Ref();
	});
}

template <typename T, typename O>
bool gk::Multiset<T, O>::remove(const std::string& k) noexcept {
	return gk::RedBlackTree<T, false, std::string, O>::remove(k, [&](T* n) {
		n->Unref();
	});
}

template <typename T, typename O>
void gk::Multiset<T, O>::cleanUp() noexcept {
	this->clear([&](T *n) {
		n->Unref();
	});
}

template <typename T, typename O>
gk::Multiset<T, O>* gk::Multiset<T, O>::Instance(v8::Isolate* isolate) noexcept {
	const int argc = 0;
	v8::Local<v8::Value> argv[argc] = {};
	auto cons = GK_FUNCTION(constructor_);
	return node::ObjectWrap::Unwrap<gk::Multiset<T, O>>(cons->NewInstance(argc, argv));
}

template <typename T, typename O>
GK_INIT(gk::Multiset<T, O>::Init) {
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

	constructor_.Reset(isolate, t->GetFunction());
	exports->Set(GK_STRING(symbol), t->GetFunction());
}

template <typename T, typename O>
GK_METHOD(gk::Multiset<T, O>::New) {
	GK_SCOPE();

	if (args.IsConstructCall()) {
		auto obj = new gk::Multiset<T, O>{};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 0;
		v8::Local<v8::Value> argv[argc] = {};
		auto cons = GK_FUNCTION(constructor_);
		GK_RETURN(cons->NewInstance(argc, argv));
	}
}

template <typename T, typename O>
GK_METHOD(gk::Multiset<T, O>::Size) {
	GK_SCOPE();
	auto s = node::ObjectWrap::Unwrap<gk::Multiset<T, O>>(args.Holder());
	GK_RETURN(GK_NUMBER(s->size()));
}

template <typename T, typename O>
GK_METHOD(gk::Multiset<T, O>::Insert) {
	GK_SCOPE();

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<T>(args[0]->ToObject());
	if (!n->indexed()) {
		GK_EXCEPTION("[GraphKit Error: NodeClass has not been indexed.]");
	}

	auto s = node::ObjectWrap::Unwrap<gk::Multiset<T, O>>(args.Holder());
	GK_RETURN(GK_BOOLEAN(s->insert(isolate, n)));
}

template <typename T, typename O>
GK_METHOD(gk::Multiset<T, O>::Remove) {
	GK_SCOPE();

	if (0 == args.Length()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto s = node::ObjectWrap::Unwrap<gk::Multiset<T, O>>(args.Holder());
	if (args[0]->IsNumber() && args[1]->IsString() && args[2]->IsNumber()) {
		v8::String::Utf8Value type(args[1]->ToString());
		auto k = std::string{std::string(gk::NodeClassToString(gk::NodeClassFromInt(args[0]->IntegerValue()))) + ":" + *type + ":" + std::to_string(args[2]->IntegerValue())};
		bool result = false;
		while (s->remove(k)) {
			result = true;
		}
		GK_RETURN(GK_BOOLEAN(result));
	}

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<T>(args[0]->ToObject());
	bool result = false;
	while (s->remove(n->hash())) {
		result = true;
	}
	GK_RETURN(GK_BOOLEAN(result));
}

template <typename T, typename O>
GK_METHOD(gk::Multiset<T, O>::Clear) {
	GK_SCOPE();
	auto s = node::ObjectWrap::Unwrap<gk::Multiset<T, O>>(args.Holder());
	s->cleanUp();
	GK_RETURN(GK_UNDEFINED());
}

template <typename T, typename O>
GK_METHOD(gk::Multiset<T, O>::Find) {
	GK_SCOPE();

	if (args[0]->IsNumber() && (GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT > args[0]->IntegerValue() || GK_SYMBOL_NODE_CLASS_BOND_CONSTANT < args[0]->IntegerValue())) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct NodeClass value.]");
	}

	if (!args[1]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Type value.]");
	}

	if (!args[2]->IsNumber() || 0 > args[3]->IntegerValue()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct ID value.]");
	}

	auto s = node::ObjectWrap::Unwrap<gk::Multiset<T, O>>(args.Holder());
	v8::String::Utf8Value type(args[1]->ToString());
	auto k = std::string{std::string(gk::NodeClassToString(gk::NodeClassFromInt(args[0]->IntegerValue()))) + ":" + *type + ":" + std::to_string(args[2]->IntegerValue())};
	auto n = s->findByKey(k);
	if (n) {
		GK_RETURN(n->handle());
	}
	GK_RETURN(GK_UNDEFINED());
}

template <typename T, typename O>
GK_INDEX_GETTER(gk::Multiset<T, O>::IndexGetter) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Multiset<T, O>>(args.Holder());
	if (++index > i->size()) {
		GK_EXCEPTION("[GraphKit Error: Multiset out of range.]");
	}
	GK_RETURN(i->select(index)->handle());
}

template <typename T, typename O>
GK_INDEX_SETTER(gk::Multiset<T, O>::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Multiset values may not be set.]");
}

template <typename T, typename O>
GK_INDEX_DELETER(gk::Multiset<T, O>::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Multiset values may not be deleted.]");
}

template <typename T, typename O>
GK_PROPERTY_GETTER(gk::Multiset<T, O>::PropertyGetter) {
	GK_SCOPE();
}

template <typename T, typename O>
GK_PROPERTY_SETTER(gk::Multiset<T, O>::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Multiset values may not be set.]");
}

template <typename T, typename O>
GK_PROPERTY_DELETER(gk::Multiset<T, O>::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Multiset values may not be deleted.]");
}

template <typename T, typename O>
GK_PROPERTY_ENUMERATOR(gk::Multiset<T, O>::PropertyEnumerator) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Multiset<T, O>>(args.Holder());
	auto is = i->size();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, is);
	for (auto j = is - 1; 0 <= j; --j) {
		auto node = i->node(j + 1);
		array->Set(j, GK_INTEGER(node->order()));
	}
	GK_RETURN(array);
}

#endif