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
* Export.h
*
* Used to act as a parent Class for Classes that will exist in the Node.js environment.
*/

#ifndef GRAPHKIT_SRC_EXPORT_H
#define GRAPHKIT_SRC_EXPORT_H

#include "exports.h"

namespace gk {
	class Export : public node::ObjectWrap {
	public:

		/**
		* Export
		* Constructor.
		*/
		Export() noexcept;

		/**
		* Default functions.
		*/
		Export(const Export&) = default;
		Export& operator= (const Export&) = default;
		Export(Export&&) = default;
		Export& operator= (Export&&) = default;

		/**
		* Ref
		* Increments the reference count so v8 does not garbarge collect the data.
		*/
		void Ref() noexcept;

		/**
		* Unref
		* Decrements the reference count so v8 may delete the object when garbage collecting.
		*/
		void Unref() noexcept;

		/**
		* refs
		* The current reference count.
		* @return		int
		*/
		int refs() const noexcept;

	protected:

		/**
		* ~Export
		* Destructor.
		*/
		virtual ~Export();
	};
}

#endif