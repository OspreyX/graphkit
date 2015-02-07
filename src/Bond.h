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
* Bond.h
*
* A Node Class that represents a unique relationship between two Entities.
*/

#ifndef GRAPHKIT_SRC_BOND_H
#define GRAPHKIT_SRC_BOND_H

#include "Node.h"
#include "symbols.h"

namespace gk {
	template <typename T>
	class Bond : public gk::Node {
	public:
		explicit Bond(const std::string& type) noexcept;
		virtual ~Bond();
		Bond(const Bond& other) = default;
		Bond& operator= (const Bond&) = default;
		Bond(Bond&& other) = default;
		Bond& operator= (Bond&&) = default;

		static GK_INIT(Init);

	protected:
		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
	};

	template <typename T>
	GK_CONSTRUCTOR(gk::Bond<T>::constructor_);

	template <typename T>
	gk::Bond<T>::Bond(const std::string& type) noexcept
		: gk::Node{gk::NodeClass::Bond, type} {}

	template <typename T>
	gk::Bond<T>::~Bond() {}

	template <typename T>
	GK_INIT(gk::Bond<T>::Init) {
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
			auto cons = v8::Local<v8::Function>::New(isolate, constructor_);
			GK_RETURN(cons->NewInstance(argc, argv));
		}
	}
}

#endif
