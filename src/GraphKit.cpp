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

#include "exports.h"
#include "symbols.h"
#include "Node.h"
#include "Entity.h"
#include "Action.h"
#include "Bond.h"
#include "Index.h"
#include "Cluster.h"
#include "Graph.h"
#include "GraphSet.h"
#include "GraphMultiset.h"
#include "Set.h"
#include "Multiset.h"

GK_EXPORT(GraphKit) {
	// classes
	gk::Entity::Init(exports, GK_SYMBOL_ENTITY);
	gk::Action<gk::Entity>::Init(exports, GK_SYMBOL_ACTION);
	gk::Bond<gk::Entity>::Init(exports, GK_SYMBOL_BOND);
	gk::Index<gk::Node>::Init(exports, GK_SYMBOL_INDEX);
	gk::Cluster<gk::Index<gk::Node>>::Init(exports, GK_SYMBOL_CLUSTER);
	gk::Graph<gk::Cluster<gk::Index<gk::Node>>>::Init(exports, GK_SYMBOL_GRAPH);
	gk::GraphSet<gk::Graph<gk::Cluster<gk::Index<gk::Node>>>, gk::Node>::Init(exports, GK_SYMBOL_GRAPH_SET);
	gk::GraphMultiset<gk::Graph<gk::Cluster<gk::Index<gk::Node>>>, gk::Node>::Init(exports, GK_SYMBOL_GRAPH_MULTISET);
	gk::Set<gk::Node>::Init(exports, GK_SYMBOL_SET);
	gk::Set<gk::Entity>::Init(exports, GK_SYMBOL_ENTITY_SET);
	gk::Set<gk::Action<gk::Entity>>::Init(exports, GK_SYMBOL_ACTION_SET);
	gk::Multiset<gk::Node>::Init(exports, GK_SYMBOL_MULTISET);

	// constants
	GK_SCOPE();
	exports->Set(GK_STRING(GK_SYMBOL_NODE_CLASS_NODE), GK_INTEGER(GK_SYMBOL_NODE_CLASS_NODE_CONSTANT));
	exports->Set(GK_STRING(GK_SYMBOL_NODE_CLASS_ENTITY), GK_INTEGER(GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT));
	exports->Set(GK_STRING(GK_SYMBOL_NODE_CLASS_ACTION), GK_INTEGER(GK_SYMBOL_NODE_CLASS_ACTION_CONSTANT));
	exports->Set(GK_STRING(GK_SYMBOL_NODE_CLASS_BOND), GK_INTEGER(GK_SYMBOL_NODE_CLASS_BOND_CONSTANT));
}

NODE_MODULE(graphkit, GraphKit)
