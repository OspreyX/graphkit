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
* symbols.h
*
* Symbol constants used throughout GraphKit.
*/

#ifndef GRAPHKIT_SRC_SYMBOLS_H
#define GRAPHKIT_SRC_SYMBOLS_H

// classes
#define GK_SYMBOL_NODE_CLASS_NODE_CONSTANT			0
#define GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT		1
#define GK_SYMBOL_NODE_CLASS_ACTION_CONSTANT		2
#define GK_SYMBOL_NODE_CLASS_BOND_CONSTANT			3
#define GK_SYMBOL_NODE_CLASS_NODE					"NODE"
#define GK_SYMBOL_NODE_CLASS_ENTITY					"ENTITY"
#define GK_SYMBOL_NODE_CLASS_ACTION					"ACTION"
#define GK_SYMBOL_NODE_CLASS_BOND					"BOND"
#define GK_SYMBOL_NODE 								"Node"
#define GK_SYMBOL_ENTITY 							"Entity"
#define GK_SYMBOL_ACTION 							"Action"
#define GK_SYMBOL_BOND 								"Bond"
#define GK_SYMBOL_INDEX 							"Index"
#define GK_SYMBOL_CLUSTER 							"Cluster"
#define GK_SYMBOL_GRAPH 							"Graph"
#define GK_SYMBOL_GRAPH_SET 						"GraphSet"
#define GK_SYMBOL_GRAPH_MULTISET 					"GraphMultiset"
#define GK_SYMBOL_SET 								"Set"
#define GK_SYMBOL_ENTITY_SET 						"EntitySet"
#define GK_SYMBOL_ACTION_SET 						"ActionSet"
#define GK_SYMBOL_MULTISET 							"Multiset"

// operations
#define GK_SYMBOL_OPERATION_NODE_CLASS 				"nodeClass"
#define GK_SYMBOL_OPERATION_NODE_CLASS_TO_STRING 	"nodeClassToString"
#define GK_SYMBOL_OPERATION_TYPE 					"type"
#define GK_SYMBOL_OPERATION_ID						"id"
#define GK_SYMBOL_OPERATION_INDEXED					"indexed"
#define GK_SYMBOL_OPERATION_SIZE					"size"
#define GK_SYMBOL_OPERATION_INSERT					"insert"
#define GK_SYMBOL_OPERATION_REMOVE					"remove"
#define GK_SYMBOL_OPERATION_CLEAR					"clear"
#define GK_SYMBOL_OPERATION_FIND					"find"
#define GK_SYMBOL_OPERATION_ADD_GROUP				"addGroup"
#define GK_SYMBOL_OPERATION_HAS_GROUP				"hasGroup"
#define GK_SYMBOL_OPERATION_REMOVE_GROUP			"removeGroup"
#define GK_SYMBOL_OPERATION_GROUP_SIZE				"groupSize"
#define GK_SYMBOL_OPERATION_PROPERTY_SIZE			"propertySize"
#define GK_SYMBOL_OPERATION_CREATE_ENTITY			"createEntity"
#define GK_SYMBOL_OPERATION_CREATE_ACTION			"createAction"
#define GK_SYMBOL_OPERATION_CREATE_BOND				"createBond"
#define GK_SYMBOL_OPERATION_SUBJECTS				"subjects"
#define GK_SYMBOL_OPERATION_OBJECTS					"objects"
#define GK_SYMBOL_OPERATION_ACTIONS					"actions"

#endif