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

#ifndef GRAPHKIT_SRC_ACTION_H
#define GRAPHKIT_SRC_ACTION_H

#include <cstring>
#include <utility>
#include <string>
#include <cassert>
#include "Node.h"
#include "symbols.h"
#include "Set.h"

namespace gk {
	template <typename T>
	class Action : public gk::Node {
	public:
		explicit Action(const std::string&& type) noexcept;
		virtual ~Action();
		Action(const Action& other) = default;
		Action& operator= (const Action&) = default;
		Action(Action&& other) = default;
		Action& operator= (Action&&) = default;

		gk::Set<T>* subjects(v8::Isolate* isolate) noexcept;
		gk::Set<T>* objects(v8::Isolate* isolate) noexcept;

		static Action<T>* Instance(v8::Isolate* isolate, const char* type) noexcept;
		static GK_INIT(Init);

	protected:
		gk::Set<T>* subjects_;
		gk::Set<T>* objects_;

		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
		static GK_PROPERTY_GETTER(PropertyGetter);
		static GK_PROPERTY_SETTER(PropertySetter);
		static GK_PROPERTY_DELETER(PropertyDeleter);
		static GK_PROPERTY_ENUMERATOR(PropertyEnumerator);
	};

	template <typename T>
	GK_CONSTRUCTOR(gk::Action<T>::constructor_);

	template <typename T>
	gk::Action<T>::Action(const std::string&& type) noexcept
		: gk::Node{gk::NodeClass::Action, std::move(type)},
		  subjects_{nullptr},
		  objects_{nullptr} {}

	template <typename T>
	gk::Action<T>::~Action() {
		if (nullptr != subjects_) {
			subjects_->cleanUp();
			subjects_->Unref();
		}
		if (nullptr != objects_) {
			objects_->cleanUp();
			objects_->Unref();
		}
	}

	template <typename T>
	gk::Set<T>* gk::Action<T>::subjects(v8::Isolate* isolate) noexcept {
		if (nullptr == subjects_) {
			subjects_ = gk::Set<T>::Instance(isolate);
			subjects_->Ref();
		}
		return subjects_;
	}

	template <typename T>
	gk::Set<T>* gk::Action<T>::objects(v8::Isolate* isolate) noexcept {
		if (nullptr == objects_) {
			objects_ = gk::Set<T>::Instance(isolate);
			objects_->Ref();
		}
		return objects_;
	}

	template <typename T>
	gk::Action<T>* gk::Action<T>::Instance(v8::Isolate* isolate, const char* type) noexcept {
		const int argc = 1;
		v8::Local<v8::Value> argv[argc] = {GK_STRING(type)};
		auto cons = GK_FUNCTION(constructor_);
		return node::ObjectWrap::Unwrap<gk::Action<T>>(cons->NewInstance(argc, argv));
	}

	template <typename T>
	GK_INIT(gk::Action<T>::Init) {
		GK_SCOPE();

		auto t = GK_TEMPLATE(New);
		t->SetClassName(GK_STRING(symbol));
		t->InstanceTemplate()->SetInternalFieldCount(1);
		t->InstanceTemplate()->SetIndexedPropertyHandler(IndexGetter, IndexSetter, 0, IndexDeleter);
		t->InstanceTemplate()->SetNamedPropertyHandler(PropertyGetter, PropertySetter, 0, PropertyDeleter, PropertyEnumerator);

		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_ADD_GROUP, AddGroup);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_HAS_GROUP, HasGroup);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_REMOVE_GROUP, RemoveGroup);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_GROUP_SIZE, groupSize);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_PROPERTY_SIZE, propertySize);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_NODE_CLASS_TO_STRING, NodeClassToString);

		constructor_.Reset(isolate, t->GetFunction());
		exports->Set(GK_STRING(symbol), t->GetFunction());
	}

	template <typename T>
	GK_METHOD(gk::Action<T>::New) {
		GK_SCOPE();

		if (!args[0]->IsString()) {
			GK_EXCEPTION("[GraphKit Error: Please specify a Type value.]");
		}

		if (args.IsConstructCall()) {
			v8::String::Utf8Value type(args[0]->ToString());
			auto obj = new Action{*type};
			obj->Wrap(args.This());
			GK_RETURN(args.This());
		} else {
			const int argc = 1;
			v8::Local<v8::Value> argv[argc] = {args[0]};
			auto cons = GK_FUNCTION(constructor_);
			GK_RETURN(cons->NewInstance(argc, argv));
		}
	}

	template <typename T>
	GK_PROPERTY_GETTER(gk::Action<T>::PropertyGetter) {
		GK_SCOPE();
		v8::String::Utf8Value p(property);
		auto n = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS)) {
			GK_RETURN(GK_INTEGER(gk::NodeClassToInt(n->nodeClass())));
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_TYPE)) {
			GK_RETURN(GK_STRING(n->type().c_str()));
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_ID)) {
			GK_RETURN(GK_INTEGER(n->id()));
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_HASH)) {
			GK_RETURN(GK_STRING(n->hash().c_str()));
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_INDEXED)) {
			GK_RETURN(GK_BOOLEAN(n->indexed()));
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_SUBJECTS)) {
			GK_RETURN(n->subjects(isolate)->handle());
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_OBJECTS)) {
			GK_RETURN(n->objects(isolate)->handle());
		}
		if (0 != strcmp(*p, GK_SYMBOL_OPERATION_ADD_GROUP) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_HAS_GROUP) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_REMOVE_GROUP) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_GROUP_SIZE) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_PROPERTY_SIZE) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS_TO_STRING)) {
			auto v = n->properties()->findByKey(*p);
			if (v) {
				if (0 == v->compare("true")) {
					GK_RETURN(GK_BOOLEAN(true));
				}
				if (0 == v->compare("false")) {
					GK_RETURN(GK_BOOLEAN(false));
				}
				GK_RETURN(GK_STRING((*v).c_str()));
			}
			GK_RETURN(GK_UNDEFINED());
		}
	}

	template <typename T>
	GK_PROPERTY_SETTER(gk::Action<T>::PropertySetter) {
		GK_SCOPE();
		v8::String::Utf8Value p(property);
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS)) {
			GK_EXCEPTION("[GraphKit Error: Cannot set nodeClass property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_TYPE)) {
			GK_EXCEPTION("[GraphKit Error: Cannot set type property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_ID)) {
			GK_EXCEPTION("[GraphKit Error: Cannot set id property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_HASH)) {
			GK_EXCEPTION("[GraphKit Error: Cannot set hash property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_INDEXED)) {
			GK_EXCEPTION("[GraphKit Error: Cannot set indexed property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_SUBJECTS)) {
			GK_EXCEPTION("[GraphKit Error: Cannot set subjects property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_OBJECTS)) {
			GK_EXCEPTION("[GraphKit Error: Cannot set objects property.]");
		}

		v8::String::Utf8Value v(value);
		auto prop = std::string{*p};
		auto a = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());
		a->properties()->remove(prop, [&](std::string* v) {
			delete v;
		});
		GK_RETURN(GK_BOOLEAN(a->properties()->insert(prop, new std::string{*v})));
	}

	template <typename T>
	GK_PROPERTY_DELETER(gk::Action<T>::PropertyDeleter) {
		GK_SCOPE();
		v8::String::Utf8Value prop(property);
		auto a = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());
		GK_RETURN(GK_BOOLEAN(a->properties()->remove(*prop, [&](std::string* v) {
			delete v;
		})));
	}

	template <typename T>
	GK_PROPERTY_ENUMERATOR(gk::Action<T>::PropertyEnumerator) {
		GK_SCOPE();
		auto a = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());
		auto ps = a->properties()->size();
		auto gs = a->groups()->size();
		v8::Handle<v8::Array> array = v8::Array::New(isolate, ps + gs);
		for (auto i = ps - 1; 0 <= i; --i) {
			auto node = a->properties()->node(i + 1);
			array->Set(i, GK_STRING(node->key().c_str()));
		}
		for (auto i = gs - 1; 0 <= i; --i) {
			array->Set(ps++, GK_INTEGER(i));
		}
		GK_RETURN(array);
	}
}

#endif
