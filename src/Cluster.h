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
* Cluster.h
*
* A Class used to manage Indices.
*/

#ifndef GRAPHKIT_SRC_CLUSTER_H
#define GRAPHKIT_SRC_CLUSTER_H

#include <cstring>
#include <string>
#include "exports.h"
#include "symbols.h"
#include "NodeClass.h"
#include "ObjectWrapPolicy.h"
#include "Set.h"

namespace gk {
	template <
		typename T,
		typename K = std::string,
		typename O = long long
	>
	class Cluster : public gk::ObjectWrapPolicy,
					public gk::Set<T, K, O> {
	public:
		Cluster(const gk::NodeClass& nodeClass) noexcept;
		virtual ~Cluster();
		Cluster(const Cluster&) = default;
		Cluster& operator= (const Cluster&) = default;
		Cluster(Cluster&&) = default;
		Cluster& operator= (Cluster&&) = default;

		using Index = T;

		const gk::NodeClass& nodeClass() const noexcept;
		bool insert(v8::Isolate* isolate, typename T::Node* node) noexcept;
		void cleanUp() noexcept;

		static gk::Cluster<T, K, O>* Instance(v8::Isolate* isolate, gk::NodeClass& nodeClass) noexcept;
		static GK_INIT(Init);

	private:
		const gk::NodeClass nodeClass_;

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
v8::Persistent<v8::Function> gk::Cluster<T, K, O>::constructor_;

template <typename T, typename K, typename O>
gk::Cluster<T, K, O>::Cluster(const gk::NodeClass& nodeClass) noexcept
	: gk::ObjectWrapPolicy{},
	  gk::Set<T, K, O>{},
	  nodeClass_{nodeClass} {}

template <typename T, typename K, typename O>
gk::Cluster<T, K, O>::~Cluster() {
	cleanUp();
}

template <typename T, typename K, typename O>
const gk::NodeClass& gk::Cluster<T, K, O>::nodeClass() const noexcept {
	return nodeClass_;
}

template  <typename T, typename K, typename O>
bool gk::Cluster<T, K, O>::insert(v8::Isolate* isolate, typename T::Node* node) noexcept {
	auto i = this->findByKey(node->type());
	if (!i) {
		auto nc = node->nodeClass();
		auto t = node->type();
		i = T::Instance(isolate, nc, t);
		gk::Set<T, K, O>::insert(i->type(), i, [&](T* i) {
			i->Ref();
		});
	}
	return i->insert(node);
}

template  <typename T, typename K, typename O>
void gk::Cluster<T, K, O>::cleanUp() noexcept {
	this->clear([&](T *i) {
		i->cleanUp();
		i->Unref();
	});
}

template  <typename T, typename K, typename O>
GK_METHOD(gk::Cluster<T, K, O>::NodeClassToString) {
	GK_SCOPE();
	auto c = node::ObjectWrap::Unwrap<gk::Cluster<T, K, O>>(args.Holder());
	GK_RETURN(GK_STRING(gk::NodeClassToString(c->nodeClass())));
}

template <typename T, typename K, typename O>
gk::Cluster<T, K, O>* gk::Cluster<T, K, O>::Instance(v8::Isolate* isolate, gk::NodeClass& nodeClass) noexcept {
	const int argc = 1;
	v8::Local<v8::Value> argv[argc] = {GK_INTEGER(gk::NodeClassToInt(nodeClass))};
	auto cons = v8::Local<v8::Function>::New(isolate, constructor_);
	return node::ObjectWrap::Unwrap<gk::Cluster<T, K, O>>(cons->NewInstance(argc, argv));
}

template <typename T, typename K, typename O>
GK_INIT(gk::Cluster<T, K, O>::Init) {
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
GK_METHOD(gk::Cluster<T, K, O>::New) {
	GK_SCOPE();
	
	if (args[0]->IsNumber() && (GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT > args[0]->IntegerValue() || GK_SYMBOL_NODE_CLASS_BOND_CONSTANT < args[0]->IntegerValue())) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct NodeClass value.]");
	}

	if (args.IsConstructCall()) {
		auto nc = args[0]->IsNumber() ? gk::NodeClassFromInt(args[0]->IntegerValue()) : gk::NodeClass::Entity;
		auto obj = new gk::Cluster<T, K, O>{nc};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 1;
		v8::Local<v8::Value> argv[argc] = {args[0]};
		auto cons = v8::Local<v8::Function>::New(isolate, constructor_);
		GK_RETURN(cons->NewInstance(argc, argv));
	}
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Cluster<T, K, O>::Size) {
	GK_SCOPE();
	auto c = node::ObjectWrap::Unwrap<gk::Cluster<T, K, O>>(args.Holder());
	GK_RETURN(GK_NUMBER(c->size()));
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Cluster<T, K, O>::Insert) {
	GK_SCOPE();

	if (0 == args.Length() || !args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<typename T::Node>(args[0]->ToObject());
	if (n->indexed()) {
		GK_RETURN(GK_BOOLEAN(false));
	}

	auto c = node::ObjectWrap::Unwrap<gk::Cluster<T, K, O>>(args.Holder());
	if (c->nodeClass() != n->nodeClass()) {
		std::string s1 {gk::NodeClassToString(c->nodeClass())};
		std::string s2 {gk::NodeClassToString(n->nodeClass())};
		GK_EXCEPTION(("[GraphKit Error: Invalid NodeClass. Expecting " + s1 + " and got " + s2 + ".]").c_str());
	}

	GK_RETURN(GK_BOOLEAN(c->insert(isolate, n)));
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Cluster<T, K, O>::Remove) {
	GK_SCOPE();

	if (0 == args.Length()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto c = node::ObjectWrap::Unwrap<gk::Cluster<T, K, O>>(args.Holder());
	if (args[0]->IsString() && args[1]->IsNumber()) {
		v8::String::Utf8Value type(args[0]->ToString());
		auto i = c->findByKey(*type);
		if (i) {
			GK_RETURN(GK_BOOLEAN(i->remove(args[1]->IntegerValue())));
		}
		GK_RETURN(GK_BOOLEAN(false));
	}

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<typename T::Index::Node>(args[0]->ToObject());
	if (c->nodeClass() != n->nodeClass()) {
		std::string s1 {gk::NodeClassToString(c->nodeClass())};
		std::string s2 {gk::NodeClassToString(n->nodeClass())};
		GK_EXCEPTION(("[GraphKit Error: Invalid NodeClass. Expecting " + s1 + " and got " + s2 + ".]").c_str());
	}

	auto i = c->findByKey(n->type());
	if (i) {
		GK_RETURN(GK_BOOLEAN(i->remove(n)));
	}
	GK_RETURN(GK_BOOLEAN(false));
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Cluster<T, K, O>::Clear) {
	GK_SCOPE();
	auto c = node::ObjectWrap::Unwrap<gk::Cluster<T, K, O>>(args.Holder());
	c->cleanUp();
	GK_RETURN(GK_UNDEFINED());
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Cluster<T, K, O>::Find) {
	GK_SCOPE();

	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Type value.]");
	}

	if (!args[1]->IsNumber() || 0 > args[1]->IntegerValue()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct ID value.]");
	}

	auto c = node::ObjectWrap::Unwrap<gk::Cluster<T, K, O>>(args.Holder());
	v8::String::Utf8Value type(args[0]->ToString());
	auto i = c->findByKey(*type);
	if (i && 0 < i->size()) {
		auto n = i->findByKey(args[1]->IntegerValue());
		if (n) {
			GK_RETURN(n->handle());
		}
	}
	GK_RETURN(GK_UNDEFINED());
}

template <typename T, typename K, typename O>
GK_INDEX_GETTER(gk::Cluster<T, K, O>::IndexGetter) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Cluster<T, K, O>>(args.Holder());
	if (++index > i->size()) {
		GK_EXCEPTION("[GraphKit Error: Index out of range.]");
	}
	GK_RETURN(i->select(index)->handle());
}

template <typename T, typename K, typename O>
GK_INDEX_SETTER(gk::Cluster<T, K, O>::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Cluster values may not be set.]");
}

template <typename T, typename K, typename O>
GK_INDEX_DELETER(gk::Cluster<T, K, O>::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Cluster values may not be deleted.]");
}

template <typename T, typename K, typename O>
GK_PROPERTY_GETTER(gk::Cluster<T, K, O>::PropertyGetter) {
	GK_SCOPE();
	v8::String::Utf8Value p(property);
	auto c = node::ObjectWrap::Unwrap<gk::Cluster<T, K, O>>(args.Holder());
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS)) {
		GK_RETURN(GK_INTEGER(gk::NodeClassToInt(c->nodeClass())));
	}
	if (0 != strcmp(*p, GK_SYMBOL_OPERATION_SIZE) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_INSERT) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_REMOVE) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_CLEAR) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_FIND) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS_TO_STRING)) {
		auto v = c->findByKey(*p);
		if (v) {
			GK_RETURN(v->handle());
		}
	}
}

template <typename T, typename K, typename O>
GK_PROPERTY_SETTER(gk::Cluster<T, K, O>::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Cluster values may not be set.]");
}

template <typename T, typename K, typename O>
GK_PROPERTY_DELETER(gk::Cluster<T, K, O>::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Cluster values may not be deleted.]");
}

template <typename T, typename K, typename O>
GK_PROPERTY_ENUMERATOR(gk::Cluster<T, K, O>::PropertyEnumerator) {
	GK_SCOPE();
	auto c = node::ObjectWrap::Unwrap<gk::Cluster<T, K, O>>(args.Holder());
	auto cs = c->size();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, cs);
	for (auto i = cs - 1; 0 <= i; --i) {
		auto node = c->node(i + 1);
		array->Set(i, GK_STRING(node->key().c_str()));
	}
	GK_RETURN(array);
}

#endif