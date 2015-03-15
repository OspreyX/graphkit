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
*
* Index.h
*
* Is responsible for keeping the indexed Nodes managed, ID and file writing.
*/

#ifndef GRAPHKIT_SRC_INDEX_H
#define GRAPHKIT_SRC_INDEX_H

#include <uv.h>
#include "exports.h"
#include "Export.h"
#include "RedBlackTree.h"
#include "NodeClass.h"
#include "Node.h"

namespace gk {

	static const int GK_INDEX_BUF_SIZE = 64;

	class Index : public gk::Export,
				  public gk::RedBlackTree<gk::Node, true> {
	public:

		/**
		* Index
		* Constructor.
		* @param		const gk::NodeClass& nodeClass
		* @param		cons std::string& type
		*/
		Index(const gk::NodeClass& nodeClass, const std::string& type) noexcept;

		/**
		* ~Index
		* Destructor.
		*/
		virtual ~Index();

		// defaults
		Index(const Index&) = default;
		Index& operator= (const Index&) = default;
		Index(Index&&) = default;
		Index& operator= (Index&&) = default;

		/**
		* nodeClass
		* Retrieves the NodeClass being managed.
		* @return 		gk::NodeClass&
		*/
		const gk::NodeClass& nodeClass() const noexcept;

		/**
		* type
		* Retrieves the type being managed.
		* @return		std::string&
		*/
		const std::string& type() const noexcept;

		bool insert(gk::Node* node) noexcept;
		bool remove(gk::Node* node) noexcept;
		bool remove(const int k) noexcept;
		void cleanUp() noexcept;

		static gk::Index* Instance(v8::Isolate* isolate, gk::NodeClass& nodeClass, std::string& type) noexcept;
		static GK_INIT(Init);

	private:
		const gk::NodeClass nodeClass_;
		const std::string type_;
		std::string fs_idx_;
		long long ids_;

		char fs_buf_[GK_INDEX_BUF_SIZE + 1];
		uv_buf_t fs_iov_;
		uv_fs_t open_req_;
		uv_fs_t read_req_;
		uv_fs_t write_req_;
		uv_fs_t unlink_req_;

		/**
		* incrementID
		* Increments the ID value of the next Node to be managed.
		* @return 		Current ID value.
		*/
		long long incrementID() noexcept;

		static GK_CONSTRUCTOR(constructor_);
		static GK_METHOD(New);
		static GK_METHOD(Count);
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