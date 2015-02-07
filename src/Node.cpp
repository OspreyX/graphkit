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
*/

#include "Node.h"
#include <cstring>
#include <cassert>

gk::Node::Node(const gk::NodeClass& nodeClass, const std::string& type) noexcept
	: gk::ObjectWrapPolicy{},
	  nodeClass_{nodeClass},
	  type_{std::move(type)},
	  id_{},
	  indexed_{false},
	  groups_{nullptr},
	  properties_{nullptr},
	  graph_{nullptr},
	  hash_{} {}

gk::Node::~Node() {
	if (nullptr != groups_) {
		groups_->clear([&](std::string* v) {
			delete v;
		});
		delete groups_;
	}
	if (nullptr != properties_) {
		properties_->clear([&](std::string* v) {
			delete v;
		});
		delete properties_;
	}
	if (nullptr != graph_) {
		graph_->Unref();
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

void gk::Node::id(long long& id) noexcept {
	id_ = id;
}

void gk::Node::id(long long&& id) noexcept {
	id_ = id;
}

bool gk::Node::indexed() const noexcept {
	return indexed_;
}

void gk::Node::graph(v8::Isolate* isolate, gk::Graph<gk::Cluster<gk::Index<gk::Node>>>* graph) noexcept {
	assert(graph);
	assert(nullptr == graph_);
	graph_ = graph;
	graph_->Ref();

	// add the node to the graph groups
//	for (auto i = groups()->size() - 1; 0 <= i; --i) {
//		graph_->index(isolate, GK_SYMBOL_GROUP, this);
//	}
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
		properties_->insert(std::string{GK_SYMBOL_OPERATION_NODE_CLASS}, new std::string{GK_SYMBOL_OPERATION_NODE_CLASS});
		properties_->insert(std::string{GK_SYMBOL_OPERATION_ID}, new std::string{GK_SYMBOL_OPERATION_ID});
		properties_->insert(std::string{GK_SYMBOL_OPERATION_TYPE}, new std::string{GK_SYMBOL_OPERATION_TYPE});
		properties_->insert(std::string{GK_SYMBOL_OPERATION_INDEXED}, new std::string{GK_SYMBOL_OPERATION_INDEXED});
	}
	return properties_;
}

gk::Graph<gk::Cluster<gk::Index<gk::Node>>>* gk::Node::graph() const noexcept {
	return graph_;
}

const std::string& gk::Node::hash() noexcept {
	if (0 < id_ && hash_.empty()) {
		hash_ = std::string{std::string(gk::NodeClassToString(nodeClass_)) + ":" + type_ + ":" + std::to_string(id_)};
	}
	return hash_;
}

GK_METHOD(gk::Node::AddGroup) {
	GK_SCOPE();
	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Group value.]");
	}
	v8::String::Utf8Value value(args[0]->ToString());
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	std::string* v = new std::string{*value};
	GK_RETURN(GK_BOOLEAN(n->groups()->insert(*v, v)));
}

GK_METHOD(gk::Node::HasGroup) {
	GK_SCOPE();
	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Group value.]");
	}
	v8::String::Utf8Value value(args[0]->ToString());
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_BOOLEAN(n->groups()->has(*value)));
}

GK_METHOD(gk::Node::RemoveGroup) {
	GK_SCOPE();
	if (!args[0]->IsString()) {
		GK_EXCEPTION("[GraphKit Error: Please specify a correct Group value.]");
	}
	v8::String::Utf8Value value(args[0]->ToString());
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_BOOLEAN(n->groups()->remove(*value, [&](std::string* v) {
		delete v;
	})));
}

GK_METHOD(gk::Node::groupSize) {
	GK_SCOPE();
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_INTEGER(n->groups()->size()));
}

GK_METHOD(gk::Node::NodeClassToString) {
	GK_SCOPE();
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_STRING(gk::NodeClassToString(n->nodeClass())));
}

GK_INDEX_GETTER(gk::Node::IndexGetter) {
	GK_SCOPE();
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	if (++index > n->groups()->size()) {
		GK_EXCEPTION("[GraphKit Error: Index out of range.]");
	}
	GK_RETURN(GK_STRING((*n->groups()->select(index)).c_str()));
};

GK_INDEX_SETTER(gk::Node::IndexSetter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be set.]");
}

GK_INDEX_DELETER(gk::Node::IndexDeleter) {
	GK_SCOPE();
	GK_EXCEPTION("[GraphKit Error: Index values may not be deleted.]");
}

GK_PROPERTY_GETTER(gk::Node::PropertyGetter) {
	GK_SCOPE();
	v8::String::Utf8Value p(property);
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS)) {
		GK_RETURN(GK_INTEGER(gk::NodeClassToInt(n->nodeClass())));
	}
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_TYPE)) {
		GK_RETURN(GK_STRING(n->type().c_str()));
	}
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_ID)) {
		GK_RETURN(GK_INTEGER(n->id()));
	}
	if (0 == strcmp(*p, GK_SYMBOL_OPERATION_INDEXED)) {
		GK_RETURN(GK_BOOLEAN(n->indexed()));
	}
	if (0 != strcmp(*p, GK_SYMBOL_OPERATION_ADD_GROUP) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_HAS_GROUP) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_REMOVE_GROUP) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_GROUP_SIZE) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_PROPERTY_SIZE) &&
		0 != strcmp(*p, GK_SYMBOL_OPERATION_NODE_CLASS_TO_STRING)) {
		auto v = n->properties()->findByKey(*p);
		if (v) {
			if (0 == v->compare("true")) {
				GK_RETURN(GK_BOOLEAN(true));
			}
			if (0 == v->compare("false")) {
				GK_RETURN(GK_BOOLEAN(false));
			}
			GK_RETURN(GK_STRING((*v).c_str()));
		}
		GK_RETURN(GK_UNDEFINED());
	}
}

GK_PROPERTY_SETTER(gk::Node::PropertySetter) {
	GK_SCOPE();
	v8::String::Utf8Value p(property);
	v8::String::Utf8Value v(value);
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_BOOLEAN(n->properties()->insert(std::string{*p}, new std::string{*v})));
}

GK_PROPERTY_DELETER(gk::Node::PropertyDeleter) {
	GK_SCOPE();
	v8::String::Utf8Value prop(property);
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_BOOLEAN(n->properties()->remove(*prop, [&](std::string* v) {
		delete v;
	})));
}

GK_PROPERTY_ENUMERATOR(gk::Node::PropertyEnumerator) {
	GK_SCOPE();
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	auto ps = n->properties()->size();
	auto gs = n->groups()->size();
	v8::Handle<v8::Array> array = v8::Array::New(isolate, ps + gs);
	for (auto i = ps - 1; 0 <= i; --i) {
		auto node = n->properties()->node(i + 1);
		array->Set(i, GK_STRING(node->key().c_str()));
	}
	for (auto i = gs - 1; 0 <= i; --i) {
		array->Set(ps++, GK_INTEGER(i));
	}
	GK_RETURN(array);
}

GK_METHOD(gk::Node::propertySize) {
	GK_SCOPE();
	auto n = node::ObjectWrap::Unwrap<gk::Node>(args.Holder());
	GK_RETURN(GK_INTEGER(n->properties()->size()));
}