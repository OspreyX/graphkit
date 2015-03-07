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

#ifndef GRAPHKIT_SRC_RED_BLACK_TREE_H
#define GRAPHKIT_SRC_RED_BLACK_TREE_H

#include <cassert>
#include <functional>
#include "RedBlackNode.h"

namespace gk {
	template <
		typename T,
		bool U = false,
		typename K = long long,
		typename O = long long
	>
	class RedBlackTree {
	protected:
		using RBNode = gk::RedBlackNode<T, U, K, O>;
		RBNode* nil_;
		RBNode* root_;
		O count_;

		inline void leftRotate(RBNode* x) noexcept {
			auto y = x->right_;

			x->right_ = y->left_;
			if (y->left_ != nil_) {
				y->left_->parent_ = x;
			}

			y->parent_ = x->parent_;

			if (x->parent_ == nil_) {
				root_ = y;
			} else if (x == x->parent_->left_) {
				x->parent_->left_ = y;
			} else {
				x->parent_->right_ = y;
			}

			y->left_ = x;
			x->parent_ = y;
			y->order_ = x->order_;
			x->order_ = x->left_->order_ + x->right_->order_ + 1;
		}

		inline void rightRotate(RBNode* y) noexcept {
			auto x = y->left_;

			y->left_ = x->right_;
			if (x->right_ != nil_) {
				x->right_->parent_ = y;
			}

			x->parent_ = y->parent_;

			if (y->parent_ == nil_) {
				root_ = x;
			} else if (y == y->parent_->right_) {
				y->parent_->right_ = x;
			} else {
				y->parent_->left_ = x;
			}

			x->right_ = y;
			y->parent_ = x;
			x->order_ = y->order_;
			y->order_ = y->left_->order_ + y->right_->order_ + 1;
		}

		inline void transplant(RBNode* u, RBNode* v) noexcept {
			if (u->parent_ == nil_) {
				root_ = v;
			} else if (u == u->parent_->left_) {
				u->parent_->left_ = v;
			} else {
				u->parent_->right_ = v;
			}
			v->parent_ = u->parent_;
		}

		inline RBNode* minimum(RBNode* x) const noexcept {
			auto y = nil_;
			while (x != nil_) {
				y = x;
				x = x->left_;
			}
			return y;
		}

		inline RBNode* maximum(RBNode* x) const noexcept {
			auto y = nil_;
			while (x != nil_) {
				y = x;
				x = x->right_;
			}
			return y;
		}

		inline void insertCleanUp(RBNode* z) noexcept {
			while (z->parent_->colour_) {
				if (z->parent_ == z->parent_->parent_->left_) {
					auto y = z->parent_->parent_->right_;
					// violation 1, parent child relationship red to red
					if (y->colour_) {
						z->parent_->colour_ = false;
						y->colour_ = false;
						z->parent_->parent_->colour_ = true;
						z = z->parent_->parent_;
					} else {
						// case 2, parent is red, uncle is black
						if (z == z->parent_->right_) {
							z = z->parent_;
							leftRotate(z);
						}
						// case 3, balance colours
						z->parent_->colour_ = false;
						z->parent_->parent_->colour_ = true;
						rightRotate(z->parent_->parent_);
					}
				} else { // symetric for left and right
					auto y = z->parent_->parent_->left_;
					if (y->colour_) {
						z->parent_->colour_ = false;
						y->colour_ = false;
						z->parent_->parent_->colour_ = true;
						z = z->parent_->parent_;
					} else {
						if (z == z->parent_->left_) {
							z = z->parent_;
							rightRotate(z);
						}
						z->parent_->colour_ = false;
						z->parent_->parent_->colour_ = true;
						leftRotate(z->parent_->parent_);
					}
				}
			}
			root_->colour_ = false;
		}

		inline void removeCleanUp(RBNode* x) noexcept {
			while (x != root_ && !x->colour_) {
				if (x == x->parent_->left_) {
					auto y = x->parent_->right_;
					if (y->colour_) {
						y->colour_ = false;
						x->parent_->colour_ = true;
						leftRotate(x->parent_);
						y = x->parent_->right_;
					}
					if (!y->left_->colour_ && !y->right_->colour_) {
						y->colour_ = true;
						x = x->parent_;
					} else {
						if (!y->right_->colour_) {
							y->left_->colour_ = false;
							y->colour_ = true;
							rightRotate(y);
							y = x->parent_->right_;
						}
						y->colour_ = x->parent_->colour_;
						x->parent_->colour_ = false;
						y->right_->colour_ = false;
						leftRotate(x->parent_);
						x = root_;
					}
				} else { // symetric for left and right
					auto y = x->parent_->left_;
					if (y->colour_) {
						y->colour_ = false;
						x->parent_->colour_ = true;
						rightRotate(x->parent_);
						y = x->parent_->left_;
					}
					if (!y->right_->colour_ && !y->left_->colour_) {
						y->colour_ = true;
						x = x->parent_;
					} else {
						if (!y->left_->colour_) {
							y->right_->colour_ = false;
							y->colour_ = true;
							leftRotate(y);
							y = x->parent_->left_;
						}
						y->colour_ = x->parent_->colour_;
						x->parent_->colour_ = false;
						y->left_->colour_ = false;
						rightRotate(x->parent_);
						x = root_;
					}
				}
			}
			x->colour_ = false;
		}

		inline RBNode* internalFindByKey(const K& key) const noexcept {
			auto z = root_;
			while (z != nil_) {
				if (key == z->key_) {
					return z;
				}
				z = key < z->key_ ? z->left_ : z->right_;
			}
			return nil_;
		}

		inline RBNode* internalSelect(RBNode *x, const O& order) const noexcept {
			auto r = x->left_->order_ + 1;
			if (order == r) {
				return x;
			} else if (order < r) {
				return internalSelect(x->left_, order);
			}
			return internalSelect(x->right_, order - r);
		}

		inline O& internalOrder(RBNode* x) const noexcept {
			auto r = x->left_->order_ + 1;
			auto y = x;
			while (y != root_) {
				if (y == y->parent_->right_) {
					r += y->parent_->left_->order_ + 1;
				}
				y = y->parent_;
			}
			return r;
		}

		inline RBNode* internalInsert(const K& key, T* data) noexcept {
			auto y = nil_;
			auto x = root_;

			if (U) {
				while (x != nil_) {
					y = x;
					++y->order_;
					if (key == x->key_) {
						while (x != root_) {
							--x->order_;
							x = x->parent_;
						}
						--x->order_;
						return nil_;
					}
					x = key < x->key_ ? x->left_ : x->right_;
				}
			} else {
				while (x != nil_) {
					y = x;
					++y->order_;
					x = key < x->key_ ? x->left_ : x->right_;
				}
			}

			auto z = new RBNode{y, nil_, key, data};

			if (y == nil_) {
				root_ = z;
			} else if (key < y->key_) {
				y->left_ = z;
			} else {
				y->right_ = z;
			}
			insertCleanUp(z);
			++count_;
			return z;
		}

		inline RBNode* internalRemove(const K& key) noexcept {
			auto z = internalFindByKey(key);
			if (z == nil_) {
				return nil_;
			}

			if (z != root_) {
				auto t = z->parent_;
				while (t != root_) {
					--t->order_;
					t = t->parent_;
				}
				--root_->order_;
			}

			auto y = z;
			bool colour {y->colour_};
			RBNode* x;

			if (z->left_ == nil_) {
				x = z->right_;
				transplant(z, z->right_);
			} else if (z->right_ == nil_) {
				x = z->left_;
				transplant(z, z->left_);
			} else {
				y = minimum(z->right_);
				colour = y->colour_;
				x = y->right_;
				if (y->parent_ == z) {
					x->parent_ = y;
				} else {
					transplant(y, y->right_);
					y->right_ = z->right_;
					y->right_->parent_ = y;
					auto t = x->parent_;
					while (t != y) {
						--t->order_;
						t = t->parent_;
					}
					y->order_ = y->left_->order_ + 1;
				}
				transplant(z, y);
				y->left_ = z->left_;
				y->left_->parent_ = y;
				y->colour_ = z->colour_;
				y->order_ = y->left_->order_ + y->right_->order_ + 1;
			}
			if (!colour) {
				removeCleanUp(x);
			}
			--count_;
			return z;
		}

	public:
		RedBlackTree() noexcept
			: nil_{new RBNode{}}, root_{nil_}, count_{} {
		}

		virtual ~RedBlackTree() {
			clear();
			delete nil_;
		}

		using DataCallback = std::function<void(T*)>;
		using Node = RBNode;
		using Type = T;
		using Key = K;
		using Order = O;

		// defaults
		RedBlackTree(const RedBlackTree&) = default;
		RedBlackTree& operator= (const RedBlackTree&) = default;
		RedBlackTree(RedBlackTree&&) = default;
		RedBlackTree& operator= (RedBlackTree&&) = default;

		inline bool insert(const K& key, T* data) noexcept {
			auto z = internalInsert(key, data);
			return nil_ != z;
		}

		inline bool insert(const K& key, T* data, const DataCallback& callback) noexcept {
			assert(callback);
			auto z = internalInsert(key, data);
			if (nil_ == z) {
				return false;
			}
			callback(z->data_);
			return true;
		}

		inline bool remove(const K& key) noexcept {
			auto z = internalRemove(key);
			if (nil_ == z) {
				return false;
			}
			delete z;
			return true;
		}

		inline bool remove(const K& key, const DataCallback& callback) noexcept {
			assert(callback);
			auto z = internalRemove(key);
			if (nil_ == z) {
				return false;
			}
			callback(z->data_);
			delete z;
			return true;
		}

		inline void clear() noexcept {
			for (auto order = count_; 0 < order; --order) {
				remove(node(order)->key_);
			}
		}

		inline void clear(const DataCallback& callback) noexcept {
			for (auto order = count_; 0 < order; --order) {
				remove(node(order)->key_, callback);
			}
		}

		inline T* findByKey(const K& key) const noexcept {
			return root_ == nil_ ? nil_->data_ : internalFindByKey(key)->data_;
		}

		inline O& count() noexcept {
			return count_;
		}

		inline bool empty() const noexcept {
			return 0 == count_;
		}

		inline T* select(const O& order) const noexcept {
			return internalSelect(root_, order)->data_;
		}

		inline T* front() const noexcept {
			return select(1);
		}

		inline T* back() const noexcept {
			return select(count_);
		}

		inline const RBNode* node(const O& order) const noexcept {
			return internalSelect(root_, order);
		}

		inline O& order(const K& key) const noexcept {
			auto x = internalFindByKey(key);
			return nil_ == x ? 0 : internalOrder(x);
		}

		inline bool has(const K& key) const noexcept {
			return root_ != nil_ && nil_ != internalFindByKey(key);
		}
	};
}

#endif