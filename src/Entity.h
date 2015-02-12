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

#ifndef GRAPHKIT_SRC_ENTITY_H
#define GRAPHKIT_SRC_ENTITY_H

#include <string>
#include "exports.h"
#include "Node.h"

namespace gk {
	class Entity : public gk::Node {
	public:
		explicit Entity(const std::string& type) noexcept;
		virtual ~Entity();
		Entity(const Entity& other) = default;
		Entity& operator= (const Entity&) = default;
		Entity(Entity&& other) = default;
		Entity& operator= (Entity&&) = default;

		static Entity* Instance(v8::Isolate* isolate, const char* type) noexcept;
		static GK_INIT(Init);
		static GK_CONSTRUCTOR(constructor_);

	protected:
		static GK_METHOD(New);
	};
}

#endif