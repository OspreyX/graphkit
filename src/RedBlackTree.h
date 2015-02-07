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
* RedBlackTree.h
*
* A Data Structure that implements a balanced Binsary Search Tree using the Red-Black Tree method.
*/

#ifndef GRAPHKIT_SRC_RET_BLACK_TREE_H
#define GRAPHKIT_SRC_RET_BLACK_TREE_H

#include "RedBlackTreePolicy.h"

namespace gk {
	template <
		typename T,
		bool U = false,
		typename K = long long,
		typename O = long long
	>
	class RedBlackTree : public gk::RedBlackTreePolicy<T, U, K, O> {
	public:
		RedBlackTree() noexcept
			: gk::RedBlackTreePolicy<T, U, K, O>{} {}
		virtual ~RedBlackTree() {}
		RedBlackTree(const RedBlackTree&) = default;
		RedBlackTree& operator= (const RedBlackTree&) = default;
		RedBlackTree(RedBlackTree&&) = default;
		RedBlackTree& operator= (RedBlackTree&&) = default;
	};
}

#endif