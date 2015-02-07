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
* Set.h
*
* A Data Structure that does not allow for duplicate Nodes to exist within its collection.
*/

#ifndef GRAPHKIT_SRC_SET_H
#define GRAPHKIT_SRC_SET_H

#include "RedBlackTree.h"

namespace gk {
	template <
		typename T,
		typename K = long long,
		typename O = long long
	>
	class Set : public gk::RedBlackTree<T, true, K, O> {
	public:
		Set() noexcept
			: gk::RedBlackTree<T, true, K, O>{} {}
		virtual ~Set() {}
		Set(const Set&) = default;
		Set& operator= (const Set&) = default;
		Set(Set&&) = default;
		Set& operator= (Set&&) = default;
	};
}

#endif