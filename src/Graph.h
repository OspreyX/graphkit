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
* Graph.h
*
* A Class used to manage Clusters.
*/

#ifndef GRAPHKIT_SRC_GRAPH_H
#define GRAPHKIT_SRC_GRAPH_H

#include <string>
#include "exports.h"
#include "symbols.h"
#include "NodeClass.h"
#include "ObjectWrapPolicy.h"
#include "Set.h"

namespace gk {
	template <
		typename T,
		typename K = gk::NodeClass,
		typename O = long long
	>
	class Graph : public gk::ObjectWrapPolicy,
					public gk::Set<T, K, O> {
	public:
		Graph() noexcept;
		virtual ~Graph();
		Graph(const Graph&) = default;
		Graph& operator= (const Graph&) = default;
		Graph(Graph&&) = default;
		Graph& operator= (Graph&&) = default;

		using Cluster = T;

		bool insert(v8::Isolate* isolate, typename T::Index::Node* node) noexcept;
		void cleanUp() noexcept;

		static gk::Graph<T, K, O>* Instance(v8::Isolate* isolate) noexcept;
		static GK_INIT(Init);

	private:
		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
		static GK_METHOD(Size);
		static GK_METHOD(Insert);
		static GK_METHOD(Remove);
		static GK_METHOD(Clear);
		static GK_METHOD(Find);
		static GK_INDEX_GETTER(IndexGetter);
		static GK_INDEX_SETTER(IndexSetter);
		static GK_INDEX_QUERY(IndexQuery);
		static GK_INDEX_DELETER(IndexDeleter);
		static GK_PROPERTY_GETTER(PropertyGetter);
		static GK_PROPERTY_SETTER(PropertySetter);
		static GK_PROPERTY_QUERY(PropertyQuery);
		static GK_PROPERTY_DELETER(PropertyDeleter);
		static GK_PROPERTY_ENUMERATOR(PropertyEnumerator);
	};
}

template <typename T, typename K, typename O>
v8::Persistent<v8::Function> gk::Graph<T, K, O>::constructor_;

template <typename T, typename K, typename O>
gk::Graph<T, K, O>::Graph() noexcept
	: gk::ObjectWrapPolicy{},
	  gk::Set<T, K, O>{} {}

template <typename T, typename K, typename O>
gk::Graph<T, K, O>::~Graph() {
	cleanUp();
}

template  <typename T, typename K, typename O>
bool gk::Graph<T, K, O>::insert(v8::Isolate* isolate, typename T::Index::Node* node) noexcept {
	auto c = this->findByKey(node->nodeClass());
	if (!c) {
		auto nc = node->nodeClass();
		c = T::Instance(isolate, nc);
		gk::Set<T, K, O>::insert(c->nodeClass(), c, [&](T* c) {
			c->Ref();
		});
	}
	return c->insert(isolate, node);
}

template  <typename T, typename K, typename O>
void gk::Graph<T, K, O>::cleanUp() noexcept {
	this->clear([&](T* c) {
		c->cleanUp();
		c->Unref();
	});
}

template <typename T, typename K, typename O>
gk::Graph<T, K, O>* gk::Graph<T, K, O>::Instance(v8::Isolate* isolate) noexcept {
	const int argc = 0;
	v8::Local<v8::Value> argv[argc] = {};
	auto cons = v8::Local<v8::Function>::New(isolate, constructor_);
	return node::ObjectWrap::Unwrap<gk::Graph<T, K, O>>(cons->NewInstance(argc, argv));
}

template <typename T, typename K, typename O>
GK_INIT(gk::Graph<T, K, O>::Init) {
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

template <typename T, typename K, typename O>
GK_METHOD(gk::Graph<T, K, O>::New) {
	GK_SCOPE();

	if (args.IsConstructCall()) {
		auto obj = new gk::Graph<T, K, O>{};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 0;
		v8::Local<v8::Value> argv[argc] = {};
		auto cons = v8::Local<v8::Function>::New(isolate, constructor_);
		GK_RETURN(cons->NewInstance(argc, argv));
	}
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Graph<T, K, O>::Size) {
	GK_SCOPE();
	auto g = node::ObjectWrap::Unwrap<gk::Graph<T, K, O>>(args.Holder());
	GK_RETURN(GK_NUMBER(g->size()));
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Graph<T, K, O>::Insert) {
	GK_SCOPE();

	if (0 == args.Length() || !args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<typename T::Index::Node>(args[0]->ToObject());
	if (n->indexed()) {
		GK_RETURN(GK_BOOLEAN(false));
	}

	auto g = node::ObjectWrap::Unwrap<gk::Graph<T, K, O>>(args.Holder());
	GK_RETURN(GK_BOOLEAN(g->insert(isolate, n)));
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Graph<T, K, O>::Remove) {
	GK_SCOPE();

	if (0 == args.Length()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto g = node::ObjectWrap::Unwrap<gk::Graph<T, K, O>>(args.Holder());
	if (args[0]->IsNumber() && args[1]->IsString() && args[2]->IsNumber()) {
		auto nc = gk::NodeClassFromInt(args[0]->IntegerValue());
		auto c = g->findByKey(nc);
		if (!c) {
			GK_RETURN(GK_BOOLEAN(false));
		}
		v8::String::Utf8Value type(args[1]->ToString());
		auto i = c->findByKey(*type);
		if (i) {
			GK_RETURN(GK_BOOLEAN(i->remove(args[2]->IntegerValue())));
		}
		GK_RETURN(GK_BOOLEAN(false));
	}

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<typename T::Index::Node>(args[0]->ToObject());
	auto nc = n->nodeClass();
	auto c = g->findByKey(nc);
	if (!c) {
		GK_RETURN(GK_BOOLEAN(false));
	}
	auto i = c->findByKey(n->type());
	if (i) {
		GK_RETURN(GK_BOOLEAN(i->remove(n)));
	}
	GK_RETURN(GK_BOOLEAN(false));
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Graph<T, K, O>::Clear) {
	GK_SCOPE();
	auto g = node::ObjectWrap::Unwrap<gk::Graph<T, K, O>>(args.Holder());
	g->cleanUp();
	GK_RETURN(GK_UNDEFINED());
}

template <typename T, typename K, typename O>
GK_METHOD(gk::Graph<T, K, O>::Find) {
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

	auto g = node::ObjectWrap::Unwrap<gk::Graph<T, K, O>>(args.Holder());
	auto c = g->findByKey(gk::NodeClassFromInt(args[0]->IntegerValue()));
	if (c && 0 < c->size()) {
		v8::String::Utf8Value type(args[1]->ToString());
		auto i = c->findByKey(*type);
		if (i && 0 < i->size()) {
			auto n = i->findByKey(args[2]->IntegerValue());
			if (n) {
				GK_RETURN(n->handle());
			}
		}
	}
	GK_RETURN(GK_UNDEFINED());
}

template <typename T, typename K, typename O>
GK_INDEX_GETTER(gk::Graph<T, K, O>::IndexGetter) {
	GK_SCOPE();
	auto g = node::ObjectWrap::Unwrap<gk::Graph<T, K, O>>(args.Holder());
	if (++index > g->size()) {
		GK_EXCEPTION("[GraphKit Error: Index out of range.]");
	}
	GK_RETURN(g->select(index)->handle());
}

template <typename T, typename K, typename O>
GK_INDEX_SETTER(gk::Graph<T, K, O>::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Graph values may not be set.]");
}

template <typename T, typename K, typename O>
GK_INDEX_DELETER(gk::Graph<T, K, O>::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Graph values may not be deleted.]");
}

template <typename T, typename K, typename O>
GK_PROPERTY_GETTER(gk::Graph<T, K, O>::PropertyGetter) {
	GK_SCOPE();
	v8::String::Utf8Value p(property);
	if (0 != strcmp(*p, GK_SYMBOL_OPERATION_SIZE) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_INSERT) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_REMOVE) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_CLEAR) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_FIND)) {
		auto g = node::ObjectWrap::Unwrap<gk::Graph<T, K, O>>(args.Holder());
		auto v = g->findByKey(gk::NodeClassFromString(*p));
		if (v) {
			GK_RETURN(v->handle());
		}
	}
}

template <typename T, typename K, typename O>
GK_PROPERTY_SETTER(gk::Graph<T, K, O>::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Graph values may not be set.]");
}

template <typename T, typename K, typename O>
GK_PROPERTY_DELETER(gk::Graph<T, K, O>::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Graph values may not be deleted.]");
}

template <typename T, typename K, typename O>
GK_PROPERTY_ENUMERATOR(gk::Graph<T, K, O>::PropertyEnumerator) {
	GK_SCOPE();
	auto g = node::ObjectWrap::Unwrap<gk::Graph<T, K, O>>(args.Holder());
	auto gs = g->size();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, gs);
	for (auto i = gs - 1; 0 <= i; --i) {
		auto node = g->node(i + 1);
		array->Set(i, GK_STRING(gk::NodeClassToString(node->key()).c_str()));
	}
	GK_RETURN(array);
}

#endif