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

#include <string>
#include <uv.h>
#include "RedBlackTree.h"
#include "Cluster.h"
#include "Index.h"
#include "Node.h"

namespace gk {
	class Hub {
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

		// aliases
		using Order = long long;
		using Node = gk::Node;
		using NodeKey = long long;
		using IndexKey = std::string;
		using Index = gk::Index<Node>;
		using ClusterKey = gk::NodeClass;
		using Cluster = gk::Cluster<Index>;
		using Tree = gk::RedBlackTree<Cluster, true, ClusterKey, Order>;

		/**
		* insert
		* Inserts a Node into the Graph.
		* @param		v8::Isolate* isolate
		* @param		gk::Node
		* @return		A boolean if the Node was inserted, or false otherwise.
		*/
		bool insert(v8::Isolate* isolate, Node* node) noexcept;

		/**
		* remove
		* Removes a Node from the Graph.
		* @param		const ClusterKey& cKey
		* @param		const IndexKey& iKey
		* @param		const NodeKey& nKey
		* @return		A boolean if the Node was removed, or false otherwise.
		*/
		bool remove(const ClusterKey& cKey, const IndexKey& iKey, const NodeKey nKey) noexcept;

	private:
		Tree* nodeGraph_;
	};
}

#endif