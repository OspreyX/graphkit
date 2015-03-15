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

#include <utility>
#include <cassert>
#include "Index.h"
#include "symbols.h"

GK_CONSTRUCTOR(gk::Index::constructor_);

gk::Index::Index(const gk::NodeClass& nodeClass, const std::string& type) noexcept
	: gk::Export{},
	  gk::RedBlackTree<gk::Node, true>{},
	  nodeClass_{std::move(nodeClass)},
	  type_{std::move(type)},
	  fs_idx_{"./" + std::string(GK_FS_DB_DIR)+ "/" + std::to_string(gk::NodeClassToInt(nodeClass_)) + type_ + ".idx"},
	  fs_iov_(uv_buf_init(fs_buf_, sizeof(fs_buf_))) {

	// file writing
	uv_fs_open(uv_default_loop(), &open_req_, fs_idx_.c_str(), O_CREAT | O_RDWR, S_IRWXU, NULL);
	uv_fs_read(uv_default_loop(), &read_req_, open_req_.result, &fs_iov_, 1, -1, NULL);
	ids_ = atol(fs_iov_.base);
	if (!ids_) {
		ids_ = 0;
	}
};

gk::Index::~Index() {
	this->clear([](gk::Node* n) {
		n->Unref();
	});

	uv_fs_t close_req;
	uv_fs_close(uv_default_loop(), &close_req, open_req_.result, NULL);
	uv_fs_req_cleanup(&close_req);
	uv_fs_req_cleanup(&open_req_);
	uv_fs_req_cleanup(&read_req_);
	uv_fs_req_cleanup(&write_req_);
	uv_fs_req_cleanup(&unlink_req_);
}

const gk::NodeClass& gk::Index::nodeClass() const noexcept {
	return nodeClass_;
}

const std::string& gk::Index::type() const noexcept {
	return type_;
}

long long gk::Index::incrementID() noexcept {
	snprintf(fs_buf_, GK_INDEX_BUF_SIZE, "%lld", ++ids_);
	uv_fs_write(uv_default_loop(), &write_req_, open_req_.result, &fs_iov_, 1, 0, NULL);
	return ids_;
}

bool gk::Index::insert(gk::Node* node) noexcept {
	if (0 == node->id()) {
		node->id(incrementID());
	}
	return gk::RedBlackTree<gk::Node, true>::insert(node->id(), node, [](gk::Node* n) {
		n->Ref();

		// Persist the Node, this test is for when the Graph initially loads the persisted data
		// so it doesn't persist it again.
		if (!n->indexed()) {
			n->indexed(true);
			n->persist();
		}
	});
}

bool gk::Index::remove(gk::Node* node) noexcept {
	if (!node->indexed()) {
		return false;
	}
	return gk::RedBlackTree<gk::Node, true>::remove(node->id(), [](gk::Node* node) {
		node->indexed(false);
		node->unlink();
		node->Unref();
	});
}

bool gk::Index::remove(const int k) noexcept {
	return gk::RedBlackTree<gk::Node, true>::remove(k, [](gk::Node* node) {
		node->indexed(false);
		node->unlink();
		node->Unref();
	});
}

void gk::Index::cleanUp() noexcept {
	for (auto i = this->count(); 0 < i; --i) {
		remove(this->select(i));
	}
}

GK_METHOD(gk::Index::NodeClassToString) {
	GK_SCOPE();
	auto index = node::ObjectWrap::Unwrap<gk::Index>(args.Holder());
	GK_RETURN(GK_STRING(gk::NodeClassToString(index->nodeClass())));
}

gk::Index* gk::Index::Instance(v8::Isolate* isolate, gk::NodeClass& nodeClass, std::string& type) noexcept {
	const int argc = 2;
	v8::Local<v8::Value> argv[argc] = {GK_INTEGER(gk::NodeClassToInt(nodeClass)), GK_STRING(type.c_str())};
	auto ctor = GK_FUNCTION(constructor_);
	return node::ObjectWrap::Unwrap<gk::Index>(ctor->NewInstance(argc, argv));
}

GK_INIT(gk::Index::Init) {
	GK_SCOPE();

	auto t = GK_TEMPLATE(New);
	t->SetClassName(GK_STRING(symbol));
	t->InstanceTemplate()->SetInternalFieldCount(1);
	t->InstanceTemplate()->SetIndexedPropertyHandler(IndexGetter, IndexSetter, 0, IndexDeleter, IndexEnumerator);
	t->InstanceTemplate()->SetNamedPropertyHandler(PropertyGetter, PropertySetter, 0, PropertyDeleter, PropertyEnumerator);

	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_INSERT, Insert);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_REMOVE, Remove);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_CLEAR, Clear);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_FIND, Find);
	NODE_SET_PROTOTYPE_METHOD(t, GK_SYMBOL_OPERATION_NODE_CLASS_TO_STRING, NodeClassToString);

	constructor_.Reset(isolate, t->GetFunction());
	exports->Set(GK_STRING(symbol), t->GetFunction());
}

GK_METHOD(gk::Index::New) {
	GK_SCOPE();

	if (GK_SYMBOL_NODE_CLASS_ENTITY_CONSTANT > args[0]->IntegerValue() || GK_SYMBOL_NODE_CLASS_BOND_CONSTANT < args[0]->IntegerValue()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct NodeClass value.]");
	}

	if (!args[1]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a Type value.]");
	}

	if (args.IsConstructCall()) {
		auto nodeClass = gk::NodeClassFromInt(args[0]->IntegerValue());
		v8::String::Utf8Value type(args[1]->ToString());
		auto obj = new gk::Index{nodeClass, *type};
		obj->Wrap(args.This());
		GK_RETURN(args.This());
	} else {
		const int argc = 2;
		v8::Local<v8::Value> argv[argc] = {args[0], args[1]};
		auto ctor = GK_FUNCTION(constructor_);
		GK_RETURN(ctor->NewInstance(argc, argv));
	}
}

GK_METHOD(gk::Index::Insert) {
	GK_SCOPE();

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto node = node::ObjectWrap::Unwrap<gk::Node>(args[0]->ToObject());
	if (node->indexed()) {
		GK_RETURN(GK_BOOLEAN(false));
	}

	auto index = node::ObjectWrap::Unwrap<gk::Index>(args.Holder());
	if (index->nodeClass() != node->nodeClass()) {
		std::string s1 {gk::NodeClassToString(index->nodeClass())};
		std::string s2 {gk::NodeClassToString(node->nodeClass())};
		GK_EXCEPTION(("[GraphKit Error: Invalid NodeClass. Expecting " + s1 + " and got " + s2 + ".]").c_str());
	}

	if (index->type() != node->type()) {
		GK_EXCEPTION(("[GraphKit Error: Invalid Type. Expecting " + index->type() + " and got " + node->type() + ".]").c_str());
	}
	GK_RETURN(GK_BOOLEAN(index->insert(node)));
}

GK_METHOD(gk::Index::Remove) {
	GK_SCOPE();

	if (0 == args.Length()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto index = node::ObjectWrap::Unwrap<gk::Index>(args.Holder());

	if (args[0]->IntegerValue()) {
		GK_RETURN(GK_BOOLEAN(index->remove(args[0]->IntegerValue())));
	}

	if (!args[0]->IsObject()) {
		GK_EXCEPTION("[GraphKit Error: Argument at position 0 must be a NodeClass Object.]");
	}

	auto node = node::ObjectWrap::Unwrap<gk::Node>(args[0]->ToObject());
	if (index->nodeClass() != node->nodeClass()) {
		std::string s1 {gk::NodeClassToString(index->nodeClass())};
		std::string s2 {gk::NodeClassToString(node->nodeClass())};
		GK_EXCEPTION(("[GraphKit Error: Invalid NodeClass. Expecting " + s1 + " and got " + s2 + ".]").c_str());
	}

	if (index->type() != node->type()) {
		GK_EXCEPTION(("[GraphKit Error: Invalid Type. Expecting " + index->type() + " and got " + node->type() + ".]").c_str());
	}
	GK_RETURN(GK_BOOLEAN(index->remove(node)));
}

GK_METHOD(gk::Index::Clear) {
	GK_SCOPE();
	auto index = node::ObjectWrap::Unwrap<gk::Index>(args.Holder());
	index->cleanUp();
	GK_RETURN(GK_UNDEFINED());
}

GK_METHOD(gk::Index::Find) {
	GK_SCOPE();
	if (0 > args[0]->IntegerValue()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct ID value.]");
	}
	auto index = node::ObjectWrap::Unwrap<gk::Index>(args.Holder());
	if (0 < index->count()) {
		auto node = index->findByKey(args[0]->IntegerValue());
		if (node) {
			GK_RETURN(node->handle());
		}
	}
	GK_RETURN(GK_UNDEFINED());
}

GK_INDEX_GETTER(gk::Index::IndexGetter) {
	GK_SCOPE();
	auto idx = node::ObjectWrap::Unwrap<gk::Index>(args.Holder());
	if (++index > idx->count()) {
		GK_EXCEPTION("[GraphKit Error: Index out of range.]");
	}
	GK_RETURN(idx->select(index)->handle());
}

GK_INDEX_SETTER(gk::Index::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be set.]");
}

GK_INDEX_DELETER(gk::Index::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be deleted.]");
}

GK_INDEX_ENUMERATOR(gk::Index::IndexEnumerator) {
	GK_SCOPE();
	auto index = node::ObjectWrap::Unwrap<gk::Index>(args.Holder());
	auto count = index->count();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, count);
	for (auto i = count - 1; 0 <= i; --i) {
		array->Set(i, GK_INTEGER(i));
	}
	GK_RETURN(array);
}

GK_PROPERTY_GETTER(gk::Index::PropertyGetter) {
	GK_SCOPE();
	v8::String::Utf8Value p(property);
	auto index = node::ObjectWrap::Unwrap<gk::Index>(args.Holder());
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS)) {
		GK_RETURN(GK_INTEGER(gk::NodeClassToInt(index->nodeClass())));
	}
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_TYPE)) {
		GK_RETURN(GK_STRING(index->type().c_str()));
	}
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_COUNT)) {
		GK_RETURN(GK_INTEGER(index->count()));
	}
}

GK_PROPERTY_SETTER(gk::Index::PropertySetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be set.]");
}

GK_PROPERTY_DELETER(gk::Index::PropertyDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be deleted.]");
}

GK_PROPERTY_ENUMERATOR(gk::Index::PropertyEnumerator) {
	GK_SCOPE();
	auto index = node::ObjectWrap::Unwrap<gk::Index>(args.Holder());
	auto count = index->count();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, count);
	for (auto i = count - 1; 0 <= i; --i) {
		auto node = index->node(i + 1);
		array->Set(i, GK_INTEGER(node->order() - 1));
	}
	GK_RETURN(array);
}
