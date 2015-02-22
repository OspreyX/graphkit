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

#include "Entity.h"
#include "symbols.h"

GK_CONSTRUCTOR(gk::Entity::constructor_);

gk::Entity::Entity(const std::string&& type) noexcept
	: gk::Node{gk::NodeClass::Entity, std::move(type)} {}

gk::Entity::~Entity() {}

gk::Entity* gk::Entity::Instance(v8::Isolate* isolate, const char* type) noexcept {
	const int argc = 1;
	v8::Local<v8::Value> argv[argc] = {GK_STRING(type)};
	auto cons = GK_FUNCTION(constructor_);
	return node::ObjectWrap::Unwrap<gk::Entity>(cons->NewInstance(argc, argv));
}

GK_INIT(gk::Entity::Init) {
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

GK_METHOD(gk::Entity::New) {
	GK_SCOPE();

	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a Type value.]");
	}

	if (args.IsConstructCall()) {
		v8::String::Utf8Value type(args[0]->ToString());
		auto obj = new Entity{*type};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 1;
		v8::Local<v8::Value> argv[argc] = {args[0]};
		auto cons = GK_FUNCTION(constructor_);
		GK_RETURN(cons->NewInstance(argc, argv));
	}
}

GK_PROPERTY_GETTER(gk::Entity::PropertyGetter) {
	GK_SCOPE();
	v8::String::Utf8Value p(property);
	auto n = node::ObjectWrap::Unwrap<gk::Entity>(args.Holder());
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS)) {
		GK_RETURN(GK_INTEGER(gk::NodeClassToInt(n->nodeClass())));
	}
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_TYPE)) {
		GK_RETURN(GK_STRING(n->type().c_str()));
	}
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_ID)) {
		GK_RETURN(GK_INTEGER(n->id()));
	}
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_INDEXED)) {
		GK_RETURN(GK_BOOLEAN(n->indexed()));
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

GK_PROPERTY_SETTER(gk::Entity::PropertySetter) {
	GK_SCOPE();
	v8::String::Utf8Value p(property);
	v8::String::Utf8Value v(value);
	auto n = node::ObjectWrap::Unwrap<gk::Entity>(args.Holder());
	GK_RETURN(GK_BOOLEAN(n->properties()->insert(std::string{*p}, new std::string{*v})));
}

GK_PROPERTY_DELETER(gk::Entity::PropertyDeleter) {
	GK_SCOPE();
	v8::String::Utf8Value prop(property);
	auto n = node::ObjectWrap::Unwrap<gk::Entity>(args.Holder());
	GK_RETURN(GK_BOOLEAN(n->properties()->remove(*prop, [&](std::string* v) {
		delete v;
	})));
}

GK_PROPERTY_ENUMERATOR(gk::Entity::PropertyEnumerator) {
	GK_SCOPE();
	auto n = node::ObjectWrap::Unwrap<gk::Entity>(args.Holder());
	auto ps = n->properties()->size();
	auto gs = n->groups()->size();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, ps + gs);
	for (auto i = ps - 1; 0 <= i; --i) {
		auto node = n->properties()->node(i + 1);
		array->Set(i, GK_STRING(node->key().c_str()));
	}
	for (auto i = gs - 1; 0 <= i; --i) {
		array->Set(ps++, GK_INTEGER(i));
	}
	GK_RETURN(array);
}