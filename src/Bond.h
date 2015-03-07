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

#ifndef GRAPHKIT_SRC_BOND_H
#define GRAPHKIT_SRC_BOND_H

#include <cstring>
#include <cassert>
#include "Node.h"
#include "symbols.h"

namespace gk {
	template <typename T>
	class Bond : public gk::Node {
	public:
		explicit Bond(const std::string&& type) noexcept;
		virtual ~Bond();
		Bond(const Bond& other) = default;
		Bond& operator= (const Bond&) = default;
		Bond(Bond&& other) = default;
		Bond& operator= (Bond&&) = default;

		T* subject() const noexcept;
		bool subject(v8::Isolate* isolate, T* node) noexcept;
		bool removeSubject() noexcept;

		T* object() const noexcept;
		bool object(v8::Isolate* isolate, T* node) noexcept;
		bool removeObject() noexcept;

		virtual std::string toJSON() noexcept;
		virtual void persist() noexcept;

		static Bond<T>* Instance(v8::Isolate* isolate, const char* type) noexcept;
		static GK_INIT(Init);

	protected:
		T* subject_;
		T* object_;

		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
		static GK_PROPERTY_GETTER(PropertyGetter);
		static GK_PROPERTY_SETTER(PropertySetter);
		static GK_PROPERTY_DELETER(PropertyDeleter);
		static GK_PROPERTY_ENUMERATOR(PropertyEnumerator);
	};

	template <typename T>
	GK_CONSTRUCTOR(gk::Bond<T>::constructor_);

	template <typename T>
	gk::Bond<T>::Bond(const std::string&& type) noexcept
		: gk::Node{gk::NodeClass::Bond,std::move(type)},
		  subject_{nullptr},
		  object_{nullptr} {}

	template <typename T>
	gk::Bond<T>::~Bond() {
		if (nullptr != subject_) {
			subject_->bonds(nullptr)->cleanUp();
			subject_->Unref();
		}
		if (nullptr != object_) {
			object_->bonds(nullptr)->cleanUp();
			object_->Unref();
		}
	}

	template <typename T>
	T* gk::Bond<T>::subject() const noexcept {
		return subject_;
	}

	template <typename T>
	bool gk::Bond<T>::subject(v8::Isolate* isolate, T* node) noexcept {
		assert(node);
		removeSubject();
		subject_ = node;
		subject_->Ref();
		subject_->bonds(isolate)->insert(this);
		persist();
		return true;
	}

	template <typename T>
	bool gk::Bond<T>::removeSubject() noexcept {
		if (nullptr != subject_) {
			if (subject_->bonds(nullptr)->remove(this->hash())) {
				subject_->Unref();
				subject_ = nullptr;
				return true;
			}
		}
		return false;
	}

	template <typename T>
	T* gk::Bond<T>::object() const noexcept {
		return object_;
	}

	template <typename T>
	bool gk::Bond<T>::object(v8::Isolate* isolate, T* node) noexcept {
		assert(node);
		removeObject();
		object_ = node;
		object_->Ref();
		object_->bonds(isolate)->insert(this);
		persist();
		return true;
	}

	template <typename T>
	bool gk::Bond<T>::removeObject() noexcept {
		if (nullptr != object_) {
			if (object_->bonds(nullptr)->remove(this->hash())) {
				object_->Unref();
				object_ = nullptr;
				return true;
			}
		}
		return false;
	}

	template <typename T>
	std::string gk::Bond<T>::toJSON() noexcept {
		std::string json = "{\"id\":" + std::to_string(id()) +
			",\"nodeClass\":" + std::to_string(gk::NodeClassToInt(nodeClass())) +
			",\"type\":\"" + type() + "\"";

		// store properties
		json += ",\"properties\":[";
		for (auto i = properties()->size(); 0 < i; --i) {
			auto q = properties_->node(i);
			json += "[\"" + q->key() + "\",\"" + *q->data() + "\"]";
			if (1 != i) {
				json += ",";
			}
		}

		json += "],\"groups\":[";
		// store groups
		for (auto i = groups()->size(); 0 < i; --i) {
			json += "\"" + *groups_->select(i) + "\"";
			if (1 != i) {
				json += ",";
			}
		}

		json += "]";
		if (nullptr != subject_) {
			json += ",\"subject\":{\"id\":" + std::to_string(subject_->id()) + ",\"nodeClass\":" + std::to_string(gk::NodeClassToInt(subject_->nodeClass())) + ",\"type\":\"" + subject_->type() + "\"}";
		}

		if (nullptr != object_) {
			json += ",\"object\":{\"id\":" + std::to_string(object_->id()) + ",\"nodeClass\":" + std::to_string(gk::NodeClassToInt(object_->nodeClass())) + ",\"type\":\"" + object_->type() + "\"}";
		}

		json += "}";
		return json;
	}

	template <typename T>
	void gk::Bond<T>::persist() noexcept {
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
		}
	}

	template <typename T>
	gk::Bond<T>* gk::Bond<T>::Instance(v8::Isolate* isolate, const char* type) noexcept {
		const int argc = 1;
		v8::Local<v8::Value> argv[argc] = {GK_STRING(type)};
		auto ctor = GK_FUNCTION(constructor_);
		return node::ObjectWrap::Unwrap<gk::Bond<T>>(ctor->NewInstance(argc, argv));
	}
	
	template <typename T>
	GK_INIT(gk::Bond<T>::Init) {
		GK_SCOPE();

		auto t = GK_TEMPLATE(New);
		t->SetClassName(GK_STRING(symbol));
		t->InstanceTemplate()->SetInternalFieldCount(1);
		t->InstanceTemplate()->SetIndexedPropertyHandler(IndexGetter, IndexSetter, 0, IndexDeleter, IndexEnumerator);
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
	GK_METHOD(gk::Bond<T>::New) {
		GK_SCOPE();

		if (!args[0]->IsString()) {
			GK_EXCEPTION("[GraphKit Error: Please specify a Type value.]");
		}

		if (args.IsConstructCall()) {
			v8::String::Utf8Value type(args[0]->ToString());
			auto obj = new Bond{*type};
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
	GK_PROPERTY_GETTER(gk::Bond<T>::PropertyGetter) {
		GK_SCOPE();
		v8::String::Utf8Value p(property);
		auto n = node::ObjectWrap::Unwrap<gk::Bond<T>>(args.Holder());
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
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_SUBJECT)) {
			if (nullptr == n->subject()) {
				GK_RETURN(GK_UNDEFINED());
			}
			GK_RETURN(n->subject()->handle());
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_OBJECT)) {
			if (nullptr == n->object()) {
				GK_RETURN(GK_UNDEFINED());
			}
			GK_RETURN(n->object()->handle());
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
	GK_PROPERTY_SETTER(gk::Bond<T>::PropertySetter) {
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

		auto b = node::ObjectWrap::Unwrap<gk::Bond<T>>(args.Holder());
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_SUBJECT)) {
			if (value->IsObject()) {
				auto n = node::ObjectWrap::Unwrap<T>(value->ToObject());
				if (gk::NodeClass::Entity == n->nodeClass()) {
					if (b->subject(isolate, n)) {
						GK_RETURN(n->handle());
					}
					GK_EXCEPTION("[GraphKit Error: Bond must be indexed prior to setting subject property.]");
				}
			}
			GK_EXCEPTION("[GraphKit Error: Expecting Entity instance.]");
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_OBJECT)) {
			if (value->IsObject()) {
				auto n = node::ObjectWrap::Unwrap<T>(value->ToObject());
				if (gk::NodeClass::Entity == n->nodeClass()) {
					if (b->object(isolate, n)) {
						GK_RETURN(n->handle());
					}
					GK_EXCEPTION("[GraphKit Error: Bond must be indexed prior to setting object property.]");
				}
			}
			GK_EXCEPTION("[GraphKit Error: Expecting Entity instance.]");
		}

		v8::String::Utf8Value v(value);
		auto prop = std::string{*p};
		b->properties()->remove(prop, [&](std::string* v) {
			delete v;
		});
		auto result = b->properties()->insert(prop, new std::string{*v});
		if (result) {
			b->persist();
		}
		GK_RETURN(GK_BOOLEAN(result));
	}

	template <typename T>
	GK_PROPERTY_DELETER(gk::Bond<T>::PropertyDeleter) {
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

		auto b = node::ObjectWrap::Unwrap<gk::Bond<T>>(args.Holder());
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_SUBJECT)) {
			if (b->removeSubject()) {
				b->persist();
				GK_RETURN(GK_BOOLEAN(true));
			}
			GK_RETURN(GK_BOOLEAN(false));
		}
		if (0 == strcmp(*p, GK_SYMBOL_OPERATION_OBJECT)) {
			if (b->removeObject()) {
				b->persist();
				GK_RETURN(GK_BOOLEAN(true));
			}
			GK_RETURN(GK_BOOLEAN(false));
		}

		GK_RETURN(GK_BOOLEAN(b->properties()->remove(*p, [&](std::string* v) {
			delete v;
			b->persist();
		})));
	}

	template <typename T>
	GK_PROPERTY_ENUMERATOR(gk::Bond<T>::PropertyEnumerator) {
		GK_SCOPE();
		auto b = node::ObjectWrap::Unwrap<gk::Bond<T>>(args.Holder());
		auto ps = b->properties()->size();
		v8::Handle<v8::Array> array = v8::Array::New(isolate, 6 + ps);

		// iterate through the properties
		auto i = ps - 1;
		for (; 0 <= i; --i) {
			auto node = b->properties()->node(i + 1);
			array->Set(i, GK_STRING(node->key().c_str()));
		}

		// then add the subject and object
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_NODE_CLASS));
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_ID));
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_TYPE));
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_INDEXED));
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_SUBJECT));
		array->Set(ps++, GK_STRING(GK_SYMBOL_OPERATION_OBJECT));

		GK_RETURN(array);
	}
}

#endif
