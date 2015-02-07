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

#ifndef GRAPHKIT_SRC_SET_H
#define GRAPHKIT_SRC_SET_H

#include <cstring>
#include <string>
#include "exports.h"
#include "symbols.h"
#include "ObjectWrapPolicy.h"
#include "RedBlackTree.h"
#include "NodeClass.h"

namespace gk {
	template <
		typename G,
		typename T,
		typename O = long long
	>
	class Set : public gk::ObjectWrapPolicy,
				public gk::RedBlackTree<T, true, std::string, O> {
	public:
		explicit Set(G* graph) noexcept;
		virtual ~Set();
		Set(const Set&) = default;
		Set& operator= (const Set&) = default;
		Set(Set&&) = default;
		Set& operator= (Set&&) = default;
		
		static gk::Set<G, T, O>* Instance(v8::Isolate* isolate, G* graph) noexcept;
		static GK_INIT(Init);

	private:
		G* graph_;
		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
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

template <typename G, typename T, typename O>
GK_CONSTRUCTOR(gk::Set<G, T, O>::constructor_);

template <typename G, typename T, typename O>
gk::Set<G, T, O>::Set(G* graph) noexcept
	: gk::ObjectWrapPolicy{},
	  gk::RedBlackTree<T, true, std::string, O>{},
	  graph_{graph} {
		graph_->Ref();
	}

template <typename G, typename T, typename O>
gk::Set<G, T, O>::~Set() {
	graph_->Unref();
}

template <typename G, typename T, typename O>
gk::Set<G, T, O>* gk::Set<G, T, O>::Instance(v8::Isolate* isolate, G* graph) noexcept {
	const int argc = 1;
	v8::Local<v8::Value> argv[argc] = {graph->handle()};
	auto cons = v8::Local<v8::Function>::New(isolate, constructor_);
	return node::ObjectWrap::Unwrap<gk::Set<G, T, O>>(cons->NewInstance(argc, argv));
}

template <typename G, typename T, typename O>
GK_INIT(gk::Set<G, T, O>::Init) {
	GK_SCOPE();

	auto t = GK_TEMPLATE(New);
	t->SetClassName(GK_STRING(symbol));
	t->InstanceTemplate()->SetInternalFieldCount(1);
	t->InstanceTemplate()->SetIndexedPropertyHandler(IndexGetter, IndexSetter, 0, IndexDeleter);
	t->InstanceTemplate()->SetNamedPropertyHandler(PropertyGetter, PropertySetter, 0, PropertyDeleter);

	constructor_.Reset(isolate, t->GetFunction());
	exports->Set(GK_STRING(symbol), t->GetFunction());
}

template <typename G, typename T, typename O>
GK_METHOD(gk::Set<G, T, O>::New) {
	GK_SCOPE();

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument 0 expects a Graph instance.]");
	}

	if (args.IsConstructCall()) {
		auto g = node::ObjectWrap::Unwrap<G>(args[0]->ToObject());
		auto obj = new gk::Set<G, T, O>{g};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 1;
		v8::Local<v8::Value> argv[argc] = {args[0]};
		auto cons = v8::Local<v8::Function>::New(isolate, constructor_);
		GK_RETURN(cons->NewInstance(argc, argv));
	}
}

template <typename G, typename T, typename O>
GK_INDEX_GETTER(gk::Set<G, T, O>::IndexGetter) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Set<G, T, O>>(args.Holder());
	if (++index > i->size()) {
		GK_EXCEPTION("[GraphKit Error: Set out of range.]");
	}
	GK_RETURN(i->select(index)->handle());
}

template <typename G, typename T, typename O>
GK_INDEX_SETTER(gk::Set<G, T, O>::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Set values may not be set.]");
}

template <typename G, typename T, typename O>
GK_INDEX_DELETER(gk::Set<G, T, O>::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Set values may not be deleted.]");
}

template <typename G, typename T, typename O>
GK_PROPERTY_GETTER(gk::Set<G, T, O>::PropertyGetter) {
	GK_SCOPE();
//	v8::String::Utf8Value p(property);
//	auto i = node::ObjectWrap::Unwrap<gk::Set<G, T, O>>(args.Holder());
}

template <typename G, typename T, typename O>
GK_PROPERTY_SETTER(gk::Set<G, T, O>::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Set values may not be set.]");
}

template <typename G, typename T, typename O>
GK_PROPERTY_DELETER(gk::Set<G, T, O>::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Set values may not be deleted.]");
}

template <typename G, typename T, typename O>
GK_PROPERTY_ENUMERATOR(gk::Set<G, T, O>::PropertyEnumerator) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::Set<G, T, O>>(args.Holder());
	auto is = i->size();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, is);
	for (auto j = is - 1; 0 <= j; --j) {
		auto node = i->node(j + 1);
		array->Set(j, GK_INTEGER(node->order()));
	}
	GK_RETURN(array);
}

#endif