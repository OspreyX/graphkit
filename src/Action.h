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
*
* Action.h
*
* A relationship Node used to connect a Set of Subject template type T Nodes to a Set of Object template type T Nodes.
*/

#ifndef GRAPHKIT_SRC_ACTION_H
#define GRAPHKIT_SRC_ACTION_H

#include <cstring>
#include <utility>
#include <string>
#include <cassert>
#include <uv.h>
#include "Node.h"
#include "symbols.h"
#include "Set.h"

namespace gk {
	template <typename T>
	class Action : public gk::Node {
	public:

		/**
		* Action
		* Constructor.
		* An explicit constructor that accepts a type value.
		*/
		explicit Action(const std::string&& type) noexcept;

		/**
		* ~Action
		* Destructor.
		* Should never be used directly unless the instance was
		* created using the "new" method not through the node.js
		* environment. Reference errors in v8's garbage collection
		* may try and release the memory and crash due to this.
		*/
		virtual ~Action();

		/**
		* Default declarations.
		*/
		Action(const Action& other) = default;
		Action& operator= (const Action&) = default;
		Action(Action&& other) = default;
		Action& operator= (Action&&) = default;

		/**
		* subjects
		* Retrieves the Subjects Set of template type T Nodes.
		* @param		v8:Isolate* isolate
		* @return		gk::Set<T>*
		*/
		gk::Set<T>* subjects(v8::Isolate* isolate) noexcept;

		/**
		* addSubject
		* Adds a Node of template type T to the Subjects Set.
		* @param		v8::Isolate* isolate
		* @param		T* node
		* @return		bool value of the result. If added, true, false otherwise.
		*/
		bool addSubject(v8::Isolate* isolate, T* node) noexcept;

		/**
		* removeSubject
		* Removes a Node of template type T from the Subjects Set.
		* @param		v8::Isolate* isolate
		* @param		T* node
		* @return		bool value of the result. If removed, true, false otherwise.
		*/
		bool removeSubject(v8::Isolate* isolate, T* node) noexcept;

		/**
		* objects
		* Retrieves the Objects Set of template type T Nodes.
		* @param		v8:Isolate* isolate
		* @return		gk::Set<T>*
		*/
		gk::Set<T>* objects(v8::Isolate* isolate) noexcept;


		/**
		* addObject
		* Adds a Node of template type T to the Objects Set.
		* @param		v8::Isolate* isolate
		* @param		T* node
		* @return		bool value of the result. If added, true, false otherwise.
		*/
		bool addObject(v8::Isolate* isolate, T* node) noexcept;


		/**
		* removeObject
		* Removes a Node of template type T from the Objects Set.
		* @param		v8::Isolate* isolate
		* @param		T* node
		* @return		bool value of the result. If removed, true, false otherwise.
		*/
		bool removeObject(v8::Isolate* isolate, T* node) noexcept;

		/**
		* toJSON
		* Outputs a JSON string of the Action<T> instance.
		* @return		std::string
		*/
		virtual std::string toJSON() noexcept;

		/**
		* persist
		* Saves the Action<T> instance to disk.
		*/
		virtual void persist() noexcept;

		/**
		* Instance
		* Constructs a new Action<T> instance through the v8 engine.
		* This should be used when creating an Action<T> instances that
		* will exist in the node.js environment.
		* @param		v8::Isolate* isolate
		* @param		const char* type
		* @return		An instance of the Action<T> Node Class.
		*/
		static Action<T>* Instance(v8::Isolate* isolate, const char* type) noexcept;

		/**
		* Init
		* Initializes the Class as an export Object in the node.js environment.
		*/
		static GK_INIT(Init);

	protected:
		gk::Set<T>* subjects_;
		gk::Set<T>* objects_;

		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);

		/**
		* AddSubject
		* Adds a Node of template type T. The Action must be indexed prior to adding
		* the Node.
		*/
		static GK_METHOD(AddSubject);
		static GK_METHOD(RemoveSubject);
		static GK_METHOD(AddObject);
		static GK_METHOD(RemoveObject);
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
	bool gk::Action<T>::addSubject(v8::Isolate* isolate, T* node) noexcept {
		assert(node);
		assert(this->indexed());
		auto result = subjects(isolate)->insert(node);
		if (result) {
			node->actions(isolate)->insert(this);
			persist();
		}
		return result;
	}

	template <typename T>
	bool gk::Action<T>::removeSubject(v8::Isolate* isolate, T* node) noexcept {
		assert(node);
		auto result = subjects(isolate)->remove(node->hash());
		if (result) {
			node->actions(isolate)->remove(this->hash());
			persist();
		}
		return result;
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
	bool gk::Action<T>::addObject(v8::Isolate* isolate, T* node) noexcept {
		assert(node);
		assert(this->indexed());
		auto result = objects(isolate)->insert(node);
		if (result) {
			node->actions(isolate)->insert(this);
			persist();
		}
		return result;
	}

	template <typename T>
	bool gk::Action<T>::removeObject(v8::Isolate* isolate, T* node) noexcept {
		assert(node);
		auto result = objects(isolate)->remove(node->hash());
		if (result) {
			node->actions(isolate)->remove(this->hash());
			persist();
		}
		return result;
	}

	template <typename T>
	std::string gk::Action<T>::toJSON() noexcept {
		std::string json = "{\"id\":" + std::to_string(id()) +
			",\"nodeClass\":" + std::to_string(gk::NodeClassToInt(nodeClass())) +
			",\"type\":\"" + type() + "\"";

		// store properties
		json += ",\"properties\":[";
		for (auto i = properties()->count(); 0 < i; --i) {
			auto q = properties_->node(i);
			json += "[\"" + q->key() + "\",\"" + *q->data() + "\"]";
			if (1 != i) {
				json += ",";
			}
		}

		json += "],\"groups\":[";
		// store groups
		for (auto i = groups()->count(); 0 < i; --i) {
			json += "\"" + *groups_->select(i) + "\"";
			if (1 != i) {
				json += ",";
			}
		}

		json += "],\"subjects\":[";
		if (nullptr != subjects_) {
			for (auto i = subjects_->count(); 0 < i; --i) {
				auto subject = subjects_->select(i);
				json += "{\"id\":" + std::to_string(subject->id()) + ",\"nodeClass\":" + std::to_string(gk::NodeClassToInt(subject->nodeClass())) + ",\"type\":\"" + subject->type() + "\"}";
				if (1 != i) {
					json += ",";
				}
			}
		}

		json += "],\"objects\":[";
		if (nullptr != objects_) {
			for (auto i = objects_->count(); 0 < i; --i) {
				auto object = objects_->select(i);
				json += "{\"id\":" + std::to_string(object->id()) + ",\"nodeClass\":" + std::to_string(gk::NodeClassToInt(object->nodeClass())) + ",\"type\":\"" + object->type() + "\"}";
				if (1 != i) {
					json += ",";
				}
			}
		}

		json += "]}";
		return json;
	}

	template <typename T>
	void gk::Action<T>::persist() noexcept {
		if (indexed()) {
			std::string dir (GK_FS_DB_DIR);
			uv_fs_t open_req;
			uv_fs_open(uv_default_loop(), &open_req, ("./" + dir + "/" + hash() + ".gk").c_str(), O_CREAT | O_RDWR, S_IRWXU, NULL);
			std::string json = toJSON();
			int len = json.length() + 1;
			char buf[len];
			snprintf(buf, len, "%s", json.c_str());
			uv_buf_t iov = uv_buf_init(buf, sizeof(buf));
			uv_fs_t write_req;
			uv_fs_write(uv_default_loop(), &write_req, open_req.result, &iov, 1, 0, NULL);
			uv_fs_t close_req;
			uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
			uv_fs_req_cleanup(&open_req);
			uv_fs_req_cleanup(&write_req);
			uv_fs_req_cleanup(&close_req);
		};
	}

	template <typename T>
	gk::Action<T>* gk::Action<T>::Instance(v8::Isolate* isolate, const char* type) noexcept {
		const int argc = 1;
		v8::Local<v8::Value> argv[argc] = {GK_STRING(type)};
		auto ctor = GK_FUNCTION(constructor_);
		return node::ObjectWrap::Unwrap<gk::Action<T>>(ctor->NewInstance(argc, argv));
	}

	template <typename T>
	GK_INIT(gk::Action<T>::Init) {
		GK_SCOPE();

		auto t = GK_TEMPLATE(New);
		t->SetClassName(GK_STRING(symbol));
		t->InstanceTemplate()->SetInternalFieldCount(1);
		t->InstanceTemplate()->SetIndexedPropertyHandler(IndexGetter, IndexSetter, 0, IndexDeleter, IndexEnumerator);
		t->InstanceTemplate()->SetNamedPropertyHandler(PropertyGetter, PropertySetter, 0, PropertyDeleter, PropertyEnumerator);

		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_ADD_SUBJECT, AddSubject);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_REMOVE_SUBJECT, RemoveSubject);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_ADD_OBJECT, AddObject);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_REMOVE_OBJECT, RemoveObject);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_ADD_GROUP, AddGroup);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_HAS_GROUP, HasGroup);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_REMOVE_GROUP, RemoveGroup);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_GROUP_COUNT, groupCount);
		NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_PROPERTY_COUNT, propertyCount);
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
			auto ctor = GK_FUNCTION(constructor_);
			GK_RETURN(ctor->NewInstance(argc, argv));
		}
	}

	template <typename T>
	GK_METHOD(gk::Action<T>::AddSubject) {
		GK_SCOPE();
		auto a = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());

		// the action must be indexed
		if (!a->indexed()) {
			GK_EXCEPTION("[GraphKit Error: Action must be indexed prior to adding a subject.]");
		}


		if (args[0]->IsObject()) {
			auto n = node::ObjectWrap::Unwrap<T>(args[0]->ToObject());
			if (gk::NodeClass::Entity == n->nodeClass()) {
				GK_RETURN(GK_BOOLEAN(a->addSubject(isolate, n)));
			}
		}
		GK_EXCEPTION("[GraphKit Error: Argument at 0 is expected to be an Entity instance.]");
	}

	template <typename T>
	GK_METHOD(gk::Action<T>::RemoveSubject) {
		GK_SCOPE();
		auto a = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());
		if (args[0]->IsObject()) {
			auto n = node::ObjectWrap::Unwrap<T>(args[0]->ToObject());
			if (gk::NodeClass::Entity == n->nodeClass()) {
				GK_RETURN(GK_BOOLEAN(a->removeSubject(isolate, n)));
			}
		}
		GK_EXCEPTION("[GraphKit Error: Argument at 0 is expected to be an Entity instance.]");
	}

	template <typename T>
	GK_METHOD(gk::Action<T>::AddObject) {
		GK_SCOPE();
		auto a = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());

		// the action must be indexed
		if (!a->indexed()) {
			GK_EXCEPTION("[GraphKit Error: Action must be indexed prior to adding an object.]");
		}

		if (args[0]->IsObject()) {
			auto n = node::ObjectWrap::Unwrap<T>(args[0]->ToObject());
			if (gk::NodeClass::Entity == n->nodeClass()) {
				GK_RETURN(GK_BOOLEAN(a->addObject(isolate, n)));
			}
		}
		GK_EXCEPTION("[GraphKit Error: Argument at 0 is expected to be an Entity instance.]");
	}

	template <typename T>
	GK_METHOD(gk::Action<T>::RemoveObject) {
		GK_SCOPE();
		auto a = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());
		if (args[0]->IsObject()) {
			auto n = node::ObjectWrap::Unwrap<T>(args[0]->ToObject());
			if (gk::NodeClass::Entity == n->nodeClass()) {
				GK_RETURN(GK_BOOLEAN(a->removeObject(isolate, n)));
			}
		}
		GK_EXCEPTION("[GraphKit Error: Argument at 0 is expected to be an Entity instance.]");
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
		if (0 != strcmp(*p, GK_SYMBOL_OPERATION_ADD_SUBJECT) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_REMOVE_SUBJECT) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_ADD_OBJECT) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_REMOVE_OBJECT) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_ADD_GROUP) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_HAS_GROUP) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_REMOVE_GROUP) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_GROUP_COUNT) &&
			0 != strcmp(*p, GK_SYMBOL_OPERATION_PROPERTY_COUNT) &&
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
		auto a = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());
		a->properties()->remove(*p, [](std::string* v) {
			delete v;
		});
		auto result = a->properties()->insert(*p, new std::string{*v});
		if (result) {
			a->persist();
		}
		GK_RETURN(GK_BOOLEAN(result));
	}

	template <typename T>
	GK_PROPERTY_DELETER(gk::Action<T>::PropertyDeleter) {
		GK_SCOPE();
		v8::String::Utf8Value p(property);
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS)) {
			GK_EXCEPTION("[GraphKit Error: Cannot delete nodeClass property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_TYPE)) {
			GK_EXCEPTION("[GraphKit Error: Cannot delete type property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_ID)) {
			GK_EXCEPTION("[GraphKit Error: Cannot delete id property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_HASH)) {
			GK_EXCEPTION("[GraphKit Error: Cannot delete hash property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_INDEXED)) {
			GK_EXCEPTION("[GraphKit Error: Cannot delete indexed property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_SUBJECTS)) {
			GK_EXCEPTION("[GraphKit Error: Cannot delete subjects property.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_OBJECTS)) {
			GK_EXCEPTION("[GraphKit Error: Cannot delete objects property.]");
		}

		auto a = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());
		GK_RETURN(GK_BOOLEAN(a->properties()->remove(*p, [&](std::string* v) {
			delete v;
			a->persist();
		})));
	}

	template <typename T>
	GK_PROPERTY_ENUMERATOR(gk::Action<T>::PropertyEnumerator) {
		GK_SCOPE();
		auto a = node::ObjectWrap::Unwrap<gk::Action<T>>(args.Holder());
		auto ps = a->properties()->count();
		v8::Handle<v8::Array> array = v8::Array::New(isolate, 6 + ps);

		// iterate through the properties
		auto i = ps - 1;
		for (; 0 <= i; --i) {
			auto node = a->properties()->node(i + 1);
			array->Set(i, GK_STRING(node->key().c_str()));
		}

		// then add the subject and object
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_NODE_CLASS));
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_ID));
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_TYPE));
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_INDEXED));
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_SUBJECTS));
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_OBJECTS));

		GK_RETURN(array);
	}
}

#endif
