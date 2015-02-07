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
* Node.h
*
* Base Class for Node Objects.
*/

#ifndef GRAPHKIT_SRC_NODE_H
#define GRAPHKIT_SRC_NODE_H

#include <utility>
#include <string>
#include "exports.h"
#include "NodeClass.h"
#include "ObjectWrapPolicy.h"
#include "Set.h"
#include "Index.h"
#include "Cluster.h"
#include "Graph.h"


namespace gk {
	class Node : public gk::ObjectWrapPolicy {
	public:
		Node(const gk::NodeClass& nodeClass, const std::string& type) noexcept;
		virtual ~Node();
		Node(const Node& other) = default;
		Node& operator= (const Node&) = default;
		Node(Node&& other) = default;
		Node& operator= (Node&&) = default;

		const gk::NodeClass& nodeClass() const noexcept;
		const std::string& type() const noexcept;
		long long id() const noexcept;
		bool indexed() const noexcept;
		gk::Set<std::string, std::string>* groups() noexcept;
		gk::Set<std::string, std::string>* properties() noexcept;

	protected:
		const gk::NodeClass nodeClass_;
		const std::string type_;
		long long id_;
		bool indexed_;
		gk::Set<std::string, std::string>* groups_;
		gk::Set<std::string, std::string>* properties_;

		void id(long long& id) noexcept;
		void id(long long&& id) noexcept;
		void indexed(bool indexed) noexcept;

		static GK_METHOD(New);
		static GK_METHOD(AddGroup);
		static GK_METHOD(HasGroup);
		static GK_METHOD(RemoveGroup);
		static GK_METHOD(groupSize);
		static GK_METHOD(propertySize);
		static GK_INDEX_GETTER(IndexGetter);
		static GK_INDEX_SETTER(IndexSetter);
		static GK_INDEX_DELETER(IndexDeleter);
		static GK_PROPERTY_GETTER(PropertyGetter);
		static GK_PROPERTY_SETTER(PropertySetter);
		static GK_PROPERTY_DELETER(PropertyDeleter);
		static GK_PROPERTY_ENUMERATOR(PropertyEnumerator);

		friend class gk::Index<Node>;
		friend class gk::Cluster<gk::Index<Node>>;
		friend class gk::Graph<gk::Cluster<gk::Index<Node>>>;
	};
}

#endif