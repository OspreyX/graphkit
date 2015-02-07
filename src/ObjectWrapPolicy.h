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
* ObjectWrapPolicy.h
*
* Base Class used for all exports for the Node.js environment.
*/

#ifndef GRAPHKIT_SRC_OBJECT_WRAP_POLICY_H
#define GRAPHKIT_SRC_OBJECT_WRAP_POLICY_H

#include "exports.h"

namespace gk {
	class ObjectWrapPolicy : public node::ObjectWrap {
	public:
		ObjectWrapPolicy() noexcept;
		ObjectWrapPolicy(const ObjectWrapPolicy&) = default;
		ObjectWrapPolicy& operator= (const ObjectWrapPolicy&) = default;
		ObjectWrapPolicy(ObjectWrapPolicy&&) = default;
		ObjectWrapPolicy& operator= (ObjectWrapPolicy&&) = default;

		void Ref() noexcept;
		void Unref() noexcept;
		int refs() const noexcept;

	protected:
		virtual ~ObjectWrapPolicy();
	};
}

#endif