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
* GraphKit.cpp
*
* Source that is compiled to create the module exports in Node.js.
*/

#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include "exports.h"
#include "symbols.h"
#include "Node.h"
#include "Entity.h"
#include "Action.h"
#include "Bond.h"
#include "Index.h"
#include "Cluster.h"
#include "Graph.h"
#include "Set.h"

GK_EXPORT(GraphKit) {
	// classes
	gk::Entity::Init(exports, GK_SYMBOL_ENTITY);
	gk::Action<gk::Entity>::Init(exports, GK_SYMBOL_ACTION);
	gk::Bond<gk::Entity>::Init(exports, GK_SYMBOL_BOND);
	gk::Index<gk::Node>::Init(exports, GK_SYMBOL_INDEX);
	gk::Cluster<gk::Index<gk::Node>>::Init(exports, GK_SYMBOL_CLUSTER);
	gk::Graph<gk::Cluster<gk::Index<gk::Node>>>::Init(exports, GK_SYMBOL_GRAPH);
	gk::Set<gk::Graph<gk::Cluster<gk::Index<gk::Node>>>, gk::Node>::Init(exports, GK_SYMBOL_SET);

	// constants
	GK_SCOPE();
	exports->Set(GK_STRING(GK_SYMBOL_NODE_CLASS_NODE), GK_INTEGER(GK_SYMBOL_NODE_CLASS_NODE_CONSTANT), v8::ReadOnly);
	exports->Set(GK_STRING(GK_SYMBOL_NODE_CLASS_ENTITY), GK_INTEGER(GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT), v8::ReadOnly);
	exports->Set(GK_STRING(GK_SYMBOL_NODE_CLASS_ACTION), GK_INTEGER(GK_SYMBOL_NODE_CLASS_ACTION_CONSTANT), v8::ReadOnly);
	exports->Set(GK_STRING(GK_SYMBOL_NODE_CLASS_BOND), GK_INTEGER(GK_SYMBOL_NODE_CLASS_BOND_CONSTANT), v8::ReadOnly);
}

NODE_MODULE(graphkit, GraphKit)
