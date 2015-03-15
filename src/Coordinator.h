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

#ifndef GRAPHKIT_SRC_COORDINATOR_H
#define GRAPHKIT_SRC_COORDINATOR_H

#include <memory>
#include <string>
#include "RedBlackTree.h"
#include "Cluster.h"
#include "Node.h"
#include "Set.h"

namespace gk {
	class Coordinator {
	public:

		/**
		* Coordinator
		* Constructor.
		*/
		Coordinator() noexcept;

		/**
		* ~Coordinator
		* Destructor.
		*/
		virtual ~Coordinator();

		// defaults
		Coordinator(const Coordinator& other) = default;
		Coordinator& operator= (const Coordinator&) = default;
		Coordinator(Coordinator&& other) = default;
		Coordinator& operator= (Coordinator&&) = default;

		// aliases
		using Node = gk::Node;
		using NodeKey = long long;
		using NodeHash = std::string;
		using IndexKey = std::string;
		using Set = gk::Set;
		using SetKey = std::string;
		using ClusterKey = gk::NodeClass;
		using Cluster = gk::Cluster;
		using NodeGraph = gk::RedBlackTree<Cluster, true, ClusterKey>;
		using GroupGraph = gk::RedBlackTree<Set, true, std::string>;

		/**
		* sync
		* Synchronizes the Graph instance.
		* @param		v8::Isolate* isolate
		*/
		void sync(v8::Isolate* isolate) noexcept;

		/**
		* nodeGraph
		* Lazy loader for a nodeGraph instance.
		* @return		std::shared_ptr<NodeGraph>
		*/
		std::shared_ptr<NodeGraph> nodeGraph() noexcept;

		/**
		* groupGraph
		* Lazy loader for a groupGraph instance.
		* @return		std::shared_ptr<GroupGraph>
		*/
		std::shared_ptr<GroupGraph> groupGraph() noexcept;

		/**
		* insertNode
		* Inserts a Node into the Node Graph.
		* @param		v8::Isolate* isolate
		* @param		gk::Node
		* @return		A boolean if the Node was inserted, or false otherwise.
		*/
		bool insertNode(v8::Isolate* isolate, Node* node) noexcept;

		/**
		* removeNode
		* Removes a Node from the Node Graph.
		* @param		const ClusterKey& cKey
		* @param		const IndexKey& iKey
		* @param		const NodeKey& nKey
		* @return		A boolean if the Node was removed, or false otherwise.
		*/
		bool removeNode(const ClusterKey& cKey, const IndexKey& iKey, const NodeKey& nKey) noexcept;

		/**
		* insertGroup
		* Inserts a Node into the Group Graph.
		* @param		v8::Isolate* isolate
		* @param		std::string& group
		* @param		gk::Node
		* @return		A boolean if the Node was inserted, or false otherwise.
		*/
		bool insertGroup(v8::Isolate* isolate, std::string& group, Node* node) noexcept;

		/**
		* removeGroup
		* Removes a Node from the Group Graph.
		* @param		const SetKey& sKey
		* @param		const NodeHash& nHash
		* @return		A boolean if the Node was removed, or false otherwise.
		*/
		bool removeGroup(const SetKey& sKey, const NodeHash& nHash) noexcept;

	private:
		static bool synched_;
		static std::shared_ptr<NodeGraph> nodeGraph_;
		static std::shared_ptr<GroupGraph> groupGraph_;
	};
}

#endif