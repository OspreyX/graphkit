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

#ifndef GRAPHKIT_SRC_GRAPH_SET_H
#define GRAPHKIT_SRC_GRAPH_SET_H

#include <string>
#include "exports.h"
#include "symbols.h"
#include "Export.h"
#include "RedBlackTree.h"
#include "NodeClass.h"

namespace gk {
	template <
		typename G,
		typename T,
		typename O = long long
	>
	class GraphSet : public gk::Export,
				public gk::RedBlackTree<T, true, std::string, O> {
	public:
		explicit GraphSet(G* graph) noexcept;
		virtual ~GraphSet();
		GraphSet(const GraphSet&) = default;
		GraphSet& operator= (const GraphSet&) = default;
		GraphSet(GraphSet&&) = default;
		GraphSet& operator= (GraphSet&&) = default;

		bool insert(v8::Isolate* isolate, T* node) noexcept;
		bool remove(const std::string& k) noexcept;
		void cleanUp() noexcept;

		static gk::GraphSet<G, T, O>* Instance(v8::Isolate* isolate, G* graph) noexcept;
		static GK_INIT(Init);

	private:
		G* graph_;
		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
		static GK_METHOD(Count);
		static GK_METHOD(Insert);
		static GK_METHOD(Remove);
		static GK_METHOD(Clear);
		static GK_METHOD(Find);
		static GK_METHOD(NodeClassToString);
		static GK_INDEX_GETTER(IndexGetter);
		static GK_INDEX_SETTER(IndexSetter);
		static GK_INDEX_DELETER(IndexDeleter);
		static GK_INDEX_ENUMERATOR(IndexEnumerator);
		static GK_PROPERTY_GETTER(PropertyGetter);
		static GK_PROPERTY_SETTER(PropertySetter);
		static GK_PROPERTY_DELETER(PropertyDeleter);
		static GK_PROPERTY_ENUMERATOR(PropertyEnumerator);
	};
}

template <typename G, typename T, typename O>
GK_CONSTRUCTOR(gk::GraphSet<G, T, O>::constructor_);

template <typename G, typename T, typename O>
gk::GraphSet<G, T, O>::GraphSet(G* graph) noexcept
	: gk::Export{},
	  gk::RedBlackTree<T, true, std::string, O>{},
	  graph_{graph} {
		graph_->Ref();
	}

template <typename G, typename T, typename O>
gk::GraphSet<G, T, O>::~GraphSet() {
	cleanUp();
	graph_->Unref();
}

template <typename G, typename T, typename O>
bool gk::GraphSet<G, T, O>::insert(v8::Isolate* isolate, T* node) noexcept {
	if (!node->indexed()) {
		graph_->coordinator()->insert(isolate, node);
	}
	if (!node->indexed()) {
		return false;
	}
	return gk::RedBlackTree<T, true, std::string, O>::insert(node->hash(), node, [](T* n) {
		n->Ref();
	});
}

template <typename G, typename T, typename O>
bool gk::GraphSet<G, T, O>::remove(const std::string& k) noexcept {
	return gk::RedBlackTree<T, true, std::string, O>::remove(k, [](T* n) {
		n->Unref();
	});
}

template <typename G, typename T, typename O>
void gk::GraphSet<G, T, O>::cleanUp() noexcept {
	this->clear([](T *n) {
		n->Unref();
	});
}

template <typename G, typename T, typename O>
gk::GraphSet<G, T, O>* gk::GraphSet<G, T, O>::Instance(v8::Isolate* isolate, G* graph) noexcept {
	const int argc = 1;
	v8::Local<v8::Value> argv[argc] = {graph->handle()};
	auto ctor = GK_FUNCTION(constructor_);
	return node::ObjectWrap::Unwrap<gk::GraphSet<G, T, O>>(ctor->NewInstance(argc, argv));
}

template <typename G, typename T, typename O>
GK_INIT(gk::GraphSet<G, T, O>::Init) {
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

template <typename G, typename T, typename O>
GK_METHOD(gk::GraphSet<G, T, O>::New) {
	GK_SCOPE();

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument 0 expects a Graph instance.]");
	}

	if (args.IsConstructCall()) {
		auto g = node::ObjectWrap::Unwrap<G>(args[0]->ToObject());
		auto obj = new gk::GraphSet<G, T, O>{g};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 1;
		v8::Local<v8::Value> argv[argc] = {args[0]};
		auto ctor = GK_FUNCTION(constructor_);
		GK_RETURN(ctor->NewInstance(argc, argv));
	}
}

template <typename G, typename T, typename O>
GK_METHOD(gk::GraphSet<G, T, O>::Count) {
	GK_SCOPE();
	auto s = node::ObjectWrap::Unwrap<gk::GraphSet<G, T, O>>(args.Holder());
	GK_RETURN(GK_NUMBER(s->count()));
}

template <typename G, typename T, typename O>
GK_METHOD(gk::GraphSet<G, T, O>::Insert) {
	GK_SCOPE();

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<T>(args[0]->ToObject());
	auto s = node::ObjectWrap::Unwrap<gk::GraphSet<G, T, O>>(args.Holder());
	GK_RETURN(GK_BOOLEAN(s->insert(isolate, n)));
}

template <typename G, typename T, typename O>
GK_METHOD(gk::GraphSet<G, T, O>::Remove) {
	GK_SCOPE();

	if (0 == args.Length()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto s = node::ObjectWrap::Unwrap<gk::GraphSet<G, T, O>>(args.Holder());
	if (args[0]->IntegerValue() && args[1]->IsString() && args[2]->IntegerValue()) {
		v8::String::Utf8Value type(args[1]->ToString());
		auto k = std::string{std::string(gk::NodeClassToString(gk::NodeClassFromInt(args[0]->IntegerValue()))) + *type + std::to_string(args[2]->IntegerValue())};
		GK_RETURN(GK_BOOLEAN(s->remove(k)));
	}

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto n = node::ObjectWrap::Unwrap<T>(args[0]->ToObject());
	GK_RETURN(GK_BOOLEAN(s->remove(n->hash())));
}

template <typename G, typename T, typename O>
GK_METHOD(gk::GraphSet<G, T, O>::Clear) {
	GK_SCOPE();
	auto s = node::ObjectWrap::Unwrap<gk::GraphSet<G, T, O>>(args.Holder());
	s->cleanUp();
	GK_RETURN(GK_UNDEFINED());
}

template <typename G, typename T, typename O>
GK_METHOD(gk::GraphSet<G, T, O>::Find) {
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

	auto s = node::ObjectWrap::Unwrap<gk::GraphSet<G, T, O>>(args.Holder());
	v8::String::Utf8Value type(args[1]->ToString());
	auto k = std::to_string(args[0]->IntegerValue()) + *type + std::to_string(args[2]->IntegerValue());
	auto n = s->findByKey(k);
	if (n) {
		GK_RETURN(n->handle());
	}
	GK_RETURN(GK_UNDEFINED());
}

template <typename G, typename T, typename O>
GK_INDEX_GETTER(gk::GraphSet<G, T, O>::IndexGetter) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::GraphSet<G, T, O>>(args.Holder());
	if (++index > i->count()) {
		GK_EXCEPTION("[GraphKit Error: Set out of range.]");
	}
	GK_RETURN(i->select(index)->handle());
}

template <typename G, typename T, typename O>
GK_INDEX_SETTER(gk::GraphSet<G, T, O>::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Set values may not be set.]");
}

template <typename G, typename T, typename O>
GK_INDEX_DELETER(gk::GraphSet<G, T, O>::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Set values may not be deleted.]");
}

template <typename G, typename T, typename O>
GK_INDEX_ENUMERATOR(gk::GraphSet<G, T, O>::IndexEnumerator) {
	GK_SCOPE();
	auto i = node::ObjectWrap::Unwrap<gk::GraphSet<G, T, O>>(args.Holder());
	auto is = i->count();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, is);
	for (auto j = is - 1; 0 <= j; --j) {
		array->Set(j, GK_INTEGER(j));
	}
	GK_RETURN(array);
}

template <typename G, typename T, typename O>
GK_PROPERTY_GETTER(gk::GraphSet<G, T, O>::PropertyGetter) {
	GK_SCOPE();
}

template <typename G, typename T, typename O>
GK_PROPERTY_SETTER(gk::GraphSet<G, T, O>::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: GraphSet values may not be set.]");
}

template <typename G, typename T, typename O>
GK_PROPERTY_DELETER(gk::GraphSet<G, T, O>::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: GraphSet values may not be deleted.]");
}

template <typename G, typename T, typename O>
GK_PROPERTY_ENUMERATOR(gk::GraphSet<G, T, O>::PropertyEnumerator) {
	GK_SCOPE();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, 0);
	GK_RETURN(array);
}

#endif