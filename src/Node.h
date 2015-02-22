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

#ifndef GRAPHKIT_SRC_NODE_H
#define GRAPHKIT_SRC_NODE_H

#include <string>
#include "exports.h"
#include "NodeClass.h"
#include "Export.h"
#include "RedBlackTree.h"

namespace gk {
	class Node : public gk::Export {
	public:
		Node(const gk::NodeClass& nodeClass, const std::string&& type) noexcept;
		virtual ~Node();
		Node(const Node& other) = default;
		Node& operator= (const Node&) = default;
		Node(Node&& other) = default;
		Node& operator= (Node&&) = default;

		const gk::NodeClass& nodeClass() const noexcept;
		const std::string& type() const noexcept;
		long long id() const noexcept;
		bool indexed() const noexcept;

		gk::RedBlackTree<std::string, true, std::string>* groups() noexcept;
		gk::RedBlackTree<std::string, true, std::string>* properties() noexcept;

		void id(long long&& id) noexcept;
		void indexed(bool indexed) noexcept;

		const std::string& hash() noexcept;

	protected:
		const gk::NodeClass nodeClass_;
		const std::string type_;
		long long id_;
		bool indexed_;

		gk::RedBlackTree<std::string, true, std::string>* groups_;
		gk::RedBlackTree<std::string, true, std::string>* properties_;

		std::string hash_;

		static GK_METHOD(New);
		static GK_METHOD(AddGroup);
		static GK_METHOD(HasGroup);
		static GK_METHOD(RemoveGroup);
		static GK_METHOD(groupSize);
		static GK_METHOD(propertySize);
		static GK_METHOD(NodeClassToString);
		static GK_INDEX_GETTER(IndexGetter);
		static GK_INDEX_SETTER(IndexSetter);
		static GK_INDEX_DELETER(IndexDeleter);
	};
}

#endif