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
* RedBlackNodePolicy.h
*
* A Red-Black Node implementation using generic programming.
*/

#ifndef GRAPHKIT_SRC_RED_BLACK_NODE_POLICY_H
#define GRAPHKIT_SRC_RED_BLACK_NODE_POLICY_H

#include "RedBlackTreePolicy.h"

namespace gk {
	template <typename, bool, typename, typename> class RedBlackTreePolicy;

	template <
		typename T,
		bool U = false,
		typename K = long long,
		typename O = long long
	>
	class RedBlackNodePolicy {
	public:
		constexpr RedBlackNodePolicy() noexcept
			: parent_{this}, left_{this}, right_{this}, colour_{false}, key_{}, order_{}, data_{} {}
		constexpr RedBlackNodePolicy(RedBlackNodePolicy* parent, RedBlackNodePolicy* nil, const K& key, T* data) noexcept
			: parent_{parent}, left_{nil}, right_{nil}, colour_{true}, key_{key}, order_{1}, data_{data} {}
		RedBlackNodePolicy(const RedBlackNodePolicy&) = default;
		RedBlackNodePolicy& operator= (const RedBlackNodePolicy&) = default;
		RedBlackNodePolicy(RedBlackNodePolicy&&) = default;
		RedBlackNodePolicy& operator= (RedBlackNodePolicy&&) = default;

		inline const K key() const noexcept {
			return key_;
		}

		inline const T data() const noexcept {
			return data_;
		}

	protected:
		~RedBlackNodePolicy() {}
		RedBlackNodePolicy* parent_;
		RedBlackNodePolicy* left_;
		RedBlackNodePolicy* right_;
		bool colour_; // false = black, true = red
		const K key_;
		O order_;
		T* data_;

		friend class RedBlackTreePolicy<T, U, K, O>;
	};
}

#endif