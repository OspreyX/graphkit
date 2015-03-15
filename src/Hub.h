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

#ifndef GRAPHKIT_SRC_HUB_H
#define GRAPHKIT_SRC_HUB_H

#include "exports.h"
#include "Export.h"

namespace gk {
	class Hub : public gk::Export {
	public:

		/**
		* Hub
		* Constructor.
		*/
		Hub() noexcept;

		/**
		* ~Hub
		* Destructor.
		*/
		virtual ~Hub();

		// default constructors
		Hub(const Hub& other) = default;
		Hub& operator= (const Hub&) = default;
		Hub(Hub&& other) = default;
		Hub& operator= (Hub&&) = default;

		static gk::Hub* Instance(v8::Isolate* isolate) noexcept;
		static GK_INIT(Init);

	private:
		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
	};
}

#endif