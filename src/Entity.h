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
* Entity.h
*
* An Entity is a Node that is analogous with a noun -- person, place, or thing.
*/

#ifndef GRAPHKIT_SRC_ENTITY_H
#define GRAPHKIT_SRC_ENTITY_H

#include <string>
#include "exports.h"
#include "Node.h"
#include "Bond.h"
#include "Action.h"
#include "Set.h"

namespace gk {
	class Entity : public gk::Node {
	public:

		/**
		* Entity
		* Constructor.
		* An explicit constructor that accepts a type value.
		*/
		explicit Entity(const std::string&& type) noexcept;

		/**
		* ~Entity
		* Destructor.
		* Should never be used directly unless the instance was
		* created using the "new" method not through the node.js
		* environment. Reference errors in v8's garbage collection
		* may try and release the memory and crash due to this.
		*/
		virtual ~Entity();

		// defaults
		Entity(const Entity& other) = default;
		Entity& operator= (const Entity&) = default;
		Entity(Entity&& other) = default;
		Entity& operator= (Entity&&) = default;

		/**
		* bonds
		* Retrieves the Bonds Set of template type Entity Nodes.
		* This is a reverse mapping of Bonds that point to this Entity
		* Node.
		* @param		v8:Isolate* isolate
		* @return		gk::Set*
		*/
		gk::Set* bonds(v8::Isolate* isolate) noexcept;

		/**
		* actions
		* Retrieves the Actions Set of template type Entity Nodes.
		* This is a reverse mapping of Actions that point to this Entity
		* Node.
		* @param		v8:Isolate* isolate
		* @return		gk::Set*
		*/
		gk::Set* actions(v8::Isolate* isolate) noexcept;

		/**
		* toJSON
		* Outputs a JSON string of the Entity instance.
		* @return		std::string
		*/
		virtual std::string toJSON() noexcept;

		/**
		* persist
		* Saves the Entity instance to disk.
		*/
		virtual void persist() noexcept;

		/**
		* Instance
		* Constructs a new Entity instance through the v8 engine.
		* This should be used when creating an Entity instances that
		* will exist in the node.js environment.
		* @param		v8::Isolate* isolate
		* @param		const char* type
		* @return		An instance of the Entity Node Class.
		*/
		static Entity* Instance(v8::Isolate* isolate, const char* type) noexcept;

		/**
		* Init
		* Initializes the Class as an export Object in the node.js environment.
		*/
		static GK_INIT(Init);

	protected:
		gk::Set* bonds_;
		gk::Set* actions_;

		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
		static GK_PROPERTY_GETTER(PropertyGetter);
		static GK_PROPERTY_SETTER(PropertySetter);
		static GK_PROPERTY_DELETER(PropertyDeleter);
		static GK_PROPERTY_ENUMERATOR(PropertyEnumerator);
	};
}

#endif