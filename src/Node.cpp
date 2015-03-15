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
#include <uv.h>
#include "Node.h"
#include "Coordinator.h"

gk::Node::Node(const gk::NodeClass& nodeClass, const std::string&& type) noexcept
	: gk::Export{},
	  nodeClass_{nodeClass},
	  type_{std::move(type)},
	  id_{},
	  indexed_{false},
	  groups_{nullptr},
	  properties_{nullptr},
	  hash_{},
	  coordinator_{nullptr} {}

gk::Node::~Node() {
	if (nullptr != groups_) {
		groups_->clear([](std::string* v) {
			delete v;
		});
		delete groups_;
	}
	if (nullptr != properties_) {
		properties_->clear([](std::string* v) {
			delete v;
		});
		delete properties_;
	}
	if (0 < coordinator_.use_count()) {
		coordinator_.reset();
	}
}

const gk::NodeClass& gk::Node::nodeClass() const noexcept {
	return nodeClass_;
}

const std::string& gk::Node::type() const noexcept {
	return type_;
}

long long gk::Node::id() const noexcept {
	return id_;
}

void gk::Node::id(long long&& id) noexcept {
	hash_.clear();
	id_ = std::move(id);
}

bool gk::Node::indexed() const noexcept {
	return indexed_;
}

void gk::Node::indexed(bool indexed) noexcept {
	indexed_ = indexed;
}

gk::RedBlackTree<std::string, true, std::string>* gk::Node::groups() noexcept {
	if (nullptr == groups_) {
		groups_ = new gk::RedBlackTree<std::string, true, std::string>{};
	}
	return groups_;
}

gk::RedBlackTree<std::string, true, std::string>* gk::Node::properties() noexcept {
	if (nullptr == properties_) {
		properties_ = new gk::RedBlackTree<std::string, true, std::string>{};
	}
	return properties_;
}

const std::string& gk::Node::hash() noexcept {
	if (hash_.empty()) {
		hash_ = std::string{std::to_string(gk::NodeClassToInt(nodeClass_)) + type_ + std::to_string(id_)};
	}
	return hash_;
}

std::string gk::Node::toJSON() noexcept {
	return "";
}

void gk::Node::persist() noexcept {}

void gk::Node::unlink() noexcept {
	// delete the file
	uv_fs_t unlink_req;
	std::string dir (GK_FS_DB_DIR);
	uv_fs_unlink(uv_default_loop(), &unlink_req, (dir + "/" + hash() + ".gk").c_str(), NULL);
}

std::shared_ptr<gk::Coordinator> gk::Node::coordinator() noexcept {
	if (nullptr == coordinator_) {
		coordinator_ = std::make_shared<gk::Coordinator>();
	}
	return coordinator_;
}

GK_METHOD(gk::Node::AddGroup) {
	GK_SCOPE();
	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Group value.]");
	}
	v8::String::Utf8Value value(args[0]->ToString());
	auto node = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	std::string* v = new std::string{*value};
	auto result = node->groups()->insert(*v, v);
	if (result) {
		if (node->indexed()) {
			node->coordinator()->insertGroup(isolate, *v, node);
		}
		node->persist();
	}
	GK_RETURN(GK_BOOLEAN(result));
}

GK_METHOD(gk::Node::HasGroup) {
	GK_SCOPE();
	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Group value.]");
	}
	v8::String::Utf8Value value(args[0]->ToString());
	auto node = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_BOOLEAN(node->groups()->has(*value)));
}

GK_METHOD(gk::Node::RemoveGroup) {
	GK_SCOPE();
	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Group value.]");
	}
	v8::String::Utf8Value value(args[0]->ToString());
	auto node = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_BOOLEAN(node->groups()->remove(*value, [&](std::string* v) {
		if (node->indexed()) {
			node->coordinator()->removeGroup(*v, node->hash());
		}
		delete v;
		node->persist();
	})));
}

GK_METHOD(gk::Node::groupCount) {
	GK_SCOPE();
	auto node = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_INTEGER(node->groups()->count()));
}

GK_METHOD(gk::Node::NodeClassToString) {
	GK_SCOPE();
	auto node = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_STRING(gk::NodeClassToString(node->nodeClass())));
}

GK_INDEX_GETTER(gk::Node::IndexGetter) {
	GK_SCOPE();
	auto node = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	if (++index > node->groups()->count()) {
		GK_EXCEPTION("[GraphKit Error: Index out of range.]");
	}
	GK_RETURN(GK_STRING((*node->groups()->select(index)).c_str()));
};

GK_INDEX_SETTER(gk::Node::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be set.]");
}

GK_INDEX_DELETER(gk::Node::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be deleted.]");
}

GK_INDEX_ENUMERATOR(gk::Node::IndexEnumerator) {
	GK_SCOPE();
	auto node = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	auto count = node->groups()->count();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, count);
	for (auto i = count - 1; 0 <= i; --i) {
		array->Set(i, GK_INTEGER(i));
	}
	GK_RETURN(array);
}

GK_METHOD(gk::Node::propertyCount) {
	GK_SCOPE();
	auto node = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_INTEGER(node->properties()->count()));
}