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

#ifndef GRAPHKIT_SRC_MULTISET_H
#define GRAPHKIT_SRC_MULTISET_H

#include <string>
#include "exports.h"
#include "Export.h"
#include "RedBlackTree.h"
#include "Node.h"

namespace gk {
	class Multiset : public gk::Export,
					 public gk::RedBlackTree<gk::Node, false, std::string> {
	public:
		Multiset() noexcept;
		virtual ~Multiset();
		Multiset(const Multiset&) = default;
		Multiset& operator= (const Multiset&) = default;
		Multiset(Multiset&&) = default;
		Multiset& operator= (Multiset&&) = default;

		bool insert(gk::Node* node) noexcept;
		bool remove(const std::string& key) noexcept;
		void cleanUp() noexcept;

		static gk::Multiset* Instance(v8::Isolate* isolate) noexcept;
		static GK_INIT(Init);

	private:
		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
		static GK_METHOD(Insert);
		static GK_METHOD(Remove);
		static GK_METHOD(Clear);
		static GK_METHOD(Find);
		static GK_METHOD(NodeClassToString);
		static GK_INDEX_GETTER(IndexGetter);
		static GK_INDEX_SETTER(IndexSetter);
		static GK_INDEX_DELETER(IndexDeleter);
		static GK_INDEX_ENUMERATOR(IndexEnumerator);
		static GK_PROPERTY_GETTER(PropertyGetter);
		static GK_PROPERTY_SETTER(PropertySetter);
		static GK_PROPERTY_DELETER(PropertyDeleter);
		static GK_PROPERTY_ENUMERATOR(PropertyEnumerator);
	};
}

#endif