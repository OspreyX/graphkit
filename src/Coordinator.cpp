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

#include <uv.h>
#include "Coordinator.h"
#include "json.h"
#include "symbols.h"
#include "Entity.h"
#include "Action.h"
#include "Bond.h"

bool gk::Coordinator::synched_ = false;
std::shared_ptr<gk::Coordinator::NodeGraph> gk::Coordinator::nodeGraph_;
std::shared_ptr<gk::Coordinator::GroupGraph> gk::Coordinator::groupGraph_;

gk::Coordinator::Coordinator() noexcept {}

gk::Coordinator::~Coordinator() {
	if (1 == nodeGraph_.use_count()) {
		// cleanup when the last instance
		nodeGraph_->clear([](Cluster* cluster) {
			cluster->cleanUp();
			cluster->Unref();
		});
		nodeGraph_.reset();

		// no longer synched
		synched_ = false;
	}

	if (1 == groupGraph_.use_count()) {
		// cleanup when the last instance
		groupGraph_->clear([](gk::Set* set) {
			set->cleanUp();
			set->Unref();
		});
		groupGraph_.reset();
	}
}

std::shared_ptr<gk::Coordinator::NodeGraph> gk::Coordinator::nodeGraph() noexcept {
	if (0 == nodeGraph_.use_count()) {
		nodeGraph_ = std::make_shared<NodeGraph>();
	}
	return nodeGraph_;
}

std::shared_ptr<gk::Coordinator::GroupGraph> gk::Coordinator::groupGraph() noexcept {
	if (0 == groupGraph_.use_count()) {
		groupGraph_ = std::make_shared<GroupGraph>();
	}
	return groupGraph_;
}

void gk::Coordinator::sync(v8::Isolate* isolate) noexcept {
	// should only sync once across instances
	if (!synched_) {
		synched_ = true;

		std::string dir (GK_FS_DB_DIR);
		uv_fs_t mkdir_req;
		uv_fs_mkdir(uv_default_loop(), &mkdir_req, ("./" + dir).c_str(), S_IRWXU, NULL);

		// scan through the data directory and insert the Nodes.
		uv_fs_t scandir_req;
		uv_fs_scandir(uv_default_loop(), &scandir_req, ("./" + dir).c_str(), O_CREAT | O_RDWR, NULL);
		uv_dirent_t dent;
		assert(scandir_req.fs_type == UV_FS_SCANDIR);
		assert(scandir_req.path);
		assert(memcmp(scandir_req.path, ("./" + dir + "\0").c_str(), 8) == 0);

		// create a buffer
		std::string dat = ".gk";
		while (UV_EOF != uv_fs_scandir_next(&scandir_req, &dent)) {
			assert(dent.type == UV_DIRENT_FILE || dent.type == UV_DIRENT_UNKNOWN);

			std::string dirname = "./" + dir + "/" + std::string(dent.name);
			if (dirname.compare(dirname.length() - 3, 3, dat) == 0) {
				// open the file
				uv_fs_t open_req;
				uv_fs_open(uv_default_loop(), &open_req, dirname.c_str(), O_RDONLY, 0, NULL);

				char buf[4096];
				uv_buf_t iov = uv_buf_init(buf, sizeof(buf));

				// read in file.
				uv_fs_t read_req;
				uv_fs_read(uv_default_loop(), &read_req, open_req.result, &iov, 1, -1, NULL);

				// cleanup file if empty
				if (0 == read_req.result) {
					uv_fs_t unlink_req;
					uv_fs_unlink(uv_default_loop(), &unlink_req, dirname.c_str(), NULL);
					uv_fs_req_cleanup(&unlink_req);
					continue;
				}

				auto json = nlohmann::json::parse(buf);
				auto nodeClass = gk::NodeClassFromInt(json["nodeClass"].get<short>());
				if  (nodeClass == gk::NodeClass::Entity) {
					// insert the nodes into the Graph.
					auto entity = gk::Entity::Instance(isolate, json["type"].get<std::string>().c_str());
					entity->id(json["id"].get<long long>());
					entity->indexed(true);
					insertNode(isolate, entity);

					// groups
					for (auto name : json["groups"]) {
						std::string *v = new std::string(name.get<std::string>());
						entity->groups()->insert(*v, v);
						insertGroup(isolate, *v, entity);
					}

					// properties
					for (auto property : json["properties"]) {
						std::string *v = new std::string(property[1].get<std::string>());
						entity->properties()->insert(property[0].get<std::string>(), v);
					}
				} else if (nodeClass == gk::NodeClass::Action) {
					// insert the nodes into the Graph.
					auto action = gk::Action<gk::Entity>::Instance(isolate, json["type"].get<std::string>().c_str());
					action->id(json["id"].get<long long>());
					action->indexed(true);
					insertNode(isolate, action);

					// groups
					for (auto name : json["groups"]) {
						std::string* v = new std::string(name.get<std::string>());
						action->groups()->insert(*v, v);
						insertGroup(isolate, *v, action);
					}

					// properties
					for (auto property : json["properties"]) {
						std::string* v = new std::string(property[1].get<std::string>());
						action->properties()->insert(property[0].get<std::string>(), v);
					}

					for (auto subject : json["subjects"]) {
						if (subject.is_object()) {
							auto cluster = nodeGraph()->findByKey(gk::NodeClassFromInt(subject["nodeClass"].get<short>()));
							if (cluster && 0 < cluster->count()) {
								auto index = cluster->findByKey(subject["type"].get<std::string>());
								if (index && 0 < index->count()) {
									auto node = index->findByKey(subject["id"].get<long long>());
									if (node) {
										action->addSubject(isolate, dynamic_cast<gk::Entity*>(node));
									}
								}
							}
						}
					}

					for (auto object : json["objects"]) {
						if (object.is_object()) {
							auto cluster = nodeGraph()->findByKey(gk::NodeClassFromInt(object["nodeClass"].get<short>()));
							if (cluster && 0 < cluster->count()) {
								auto index = cluster->findByKey(object["type"].get<std::string>());
								if (index && 0 < index->count()) {
									auto node = index->findByKey(object["id"].get<long long>());
									if (node) {
										action->addObject(isolate, dynamic_cast<gk::Entity*>(node));
									}
								}
							}
						}
					}
				} else if (nodeClass == gk::NodeClass::Bond) {
					// insert the nodes into the Graph.
					auto bond = gk::Bond<gk::Entity>::Instance(isolate, json["type"].get<std::string>().c_str());
					bond->id(json["id"].get<long long>());
					bond->indexed(true);
					insertNode(isolate, bond);

					// groups
					for (auto name : json["groups"]) {
						std::string *v = new std::string(name.get<std::string>());
						bond->groups()->insert(*v, v);
						insertGroup(isolate, *v, bond);
					}

					// properties
					for (auto property : json["properties"]) {
						std::string *v = new std::string(property[1].get<std::string>());
						bond->properties()->insert(property[0].get<std::string>(), v);
					}

					auto subject = json["subject"];
					if (subject.is_object()) {
						auto cluster = nodeGraph()->findByKey(gk::NodeClassFromInt(subject["nodeClass"].get<short>()));
						if (cluster && 0 < cluster->count()) {
							auto index = cluster->findByKey(subject["type"].get<std::string>());
							if (index && 0 < index->count()) {
								auto node = index->findByKey(subject["id"].get<long long>());
								if (node) {
									bond->subject(isolate, dynamic_cast<gk::Entity*>(node));
								}
							}
						}
					}

					auto object = json["object"];
					if (object.is_object()) {
						auto cluster = nodeGraph()->findByKey(gk::NodeClassFromInt(object["nodeClass"].get<short>()));
						if (cluster && 0 < cluster->count()) {
							auto index = cluster->findByKey(object["type"].get<std::string>());
							if (index && 0 < index->count()) {
								auto node = index->findByKey(object["id"].get<long long>());
								if (node) {
									bond->object(isolate, dynamic_cast<gk::Entity*>(node));
								}
							}
						}
					}
				}

				// close the directory
				uv_fs_t close_req;
				uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);

				// cleanup
				uv_fs_req_cleanup(&open_req);
				uv_fs_req_cleanup(&read_req);
				uv_fs_req_cleanup(&close_req);
			}
		}

		// cleanup
		uv_fs_req_cleanup(&scandir_req);
		uv_fs_req_cleanup(&mkdir_req);
	}
}

bool gk::Coordinator::insertNode(v8::Isolate* isolate, gk::Coordinator::Node* node) noexcept {
	auto cluster = nodeGraph()->findByKey(node->nodeClass());
	if (!cluster) {
		auto nodeClass = node->nodeClass();
		cluster = Cluster::Instance(isolate, nodeClass);
		if (!nodeGraph()->insert(nodeClass, cluster, [](Cluster* cluster) {
			cluster->Ref();
		})) {
			return false;
		}
	}
	return cluster->insert(isolate, node);
}

bool gk::Coordinator::removeNode(const ClusterKey& cKey, const IndexKey& iKey, const NodeKey& nKey) noexcept {
	auto cluster = nodeGraph()->findByKey(cKey);
	if (cluster) {
		auto index = cluster->findByKey(iKey);
		if (index) {
			return index->remove(nKey);
		}
	}
	return false;
}

bool gk::Coordinator::insertGroup(v8::Isolate* isolate, std::string& group, gk::Coordinator::Node* node) noexcept {
	auto set = groupGraph()->findByKey(group);
	if (!set) {
		set = Set::Instance(isolate);
		if (!groupGraph()->insert(group, set, [](Set* set) {
			set->Ref();
		})) {
			return false;
		}
	}
	return set->insert(node);
}

bool gk::Coordinator::removeGroup(const SetKey& sKey, const NodeHash& nHash) noexcept {
	auto set = groupGraph()->findByKey(sKey);
	if (set) {
		return set->remove(nHash);
	}
	return false;
}
