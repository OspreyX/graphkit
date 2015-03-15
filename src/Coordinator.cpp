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

#include "Coordinator.h"

gk::Coordinator::Coordinator() noexcept
	: nodeGraph_{new gk::Coordinator::Tree{}} {}

gk::Coordinator::~Coordinator() {
	nodeGraph_->clear([](gk::Coordinator::Cluster* cluster) {
		cluster->cleanUp();
		cluster->Unref();
	});
	delete nodeGraph_;
}

bool gk::Coordinator::insert(v8::Isolate* isolate, gk::Coordinator::Node* node) noexcept {
	auto cluster = nodeGraph_->findByKey(node->nodeClass());
	if (!cluster) {
		auto nodeClass = node->nodeClass();
		cluster = Cluster::Instance(isolate, nodeClass);
		if (!nodeGraph_->insert(nodeClass, cluster, [](Cluster* cluster) {
			cluster->Ref();
		})) {
			return false;
		}
	}
	return cluster->insert(isolate, node);
}

bool gk::Coordinator::remove(const ClusterKey& cKey, const IndexKey& iKey, const NodeKey nKey) noexcept {
	auto cluster = nodeGraph_->findByKey(cKey);
	if (cluster) {
		auto index = cluster->findByKey(iKey);
		if (index) {
			return index->remove(nKey);
		}
	}
	return false;
}