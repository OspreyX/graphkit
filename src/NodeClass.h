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
* NodeClass.h
*
* Node Class types.
*/

#ifndef GRAPHKIT_SRC_NODE_CLASS_H
#define GRAPHKIT_SRC_NODE_CLASS_H

#include <cstring>
#include <string>
#include "symbols.h"

namespace gk {
	enum class NodeClass {
		Node,
		Entity,
		Action,
		Bond
	};

	inline gk::NodeClass NodeClassFromString(const char* nodeClass) noexcept {
		if (0 == strcmp(GK_SYMBOL_ENTITY, nodeClass)) {
			return gk::NodeClass::Entity;
		}
		if (0 == strcmp(GK_SYMBOL_ACTION, nodeClass)) {
			return gk::NodeClass::Action;
		}
		if (0 == strcmp(GK_SYMBOL_BOND, nodeClass)) {
			return gk::NodeClass::Bond;
		}
		return gk::NodeClass::Node;
	}

	inline const char* NodeClassToString(const NodeClass& nodeClass) noexcept {
		switch (nodeClass) {
			case NodeClass::Entity:
				return GK_SYMBOL_ENTITY;
			case NodeClass::Action:
				return GK_SYMBOL_ACTION;
			case NodeClass::Bond:
				return GK_SYMBOL_BOND;
			default:
				return GK_SYMBOL_NODE;
		};
	}

	inline gk::NodeClass NodeClassFromInt(const short& x) noexcept {
		switch (x) {
			case GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT:
				return gk::NodeClass::Entity;
			case GK_SYMBOL_NODE_CLASS_ACTION_CONSTANT:
				return gk::NodeClass::Action;
			case GK_SYMBOL_NODE_CLASS_BOND_CONSTANT:
				return gk::NodeClass::Bond;
			default:
				return gk::NodeClass::Node;
		};
	}

	inline short NodeClassToInt(const NodeClass& nodeClass) noexcept {
		switch (nodeClass) {
			case NodeClass::Entity:
				return GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT;
			case NodeClass::Action:
				return GK_SYMBOL_NODE_CLASS_ACTION_CONSTANT;
			case NodeClass::Bond:
				return GK_SYMBOL_NODE_CLASS_BOND_CONSTANT;
			default:
				return GK_SYMBOL_NODE_CLASS_NODE_CONSTANT;
		};
	}
}

#endif