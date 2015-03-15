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

#include <iostream>
#include <uv.h>
#include "Coordinator.h"
#include "json.h"
#include "symbols.h"
#include "Entity.h"
#include "Action.h"
#include "Bond.h"

// file system request handle
static uv_fs_event_t* fs_event_req;

/**
* fs_change_callback
* Callback when the file system changes.
* @param		uv_fs_event_t* handle
* @param		const char* filename
* @param		int events
* @param		int status
*/
static void fs_change_callback(uv_fs_event_t* handle, const char* filename, int events, int status) {
	if (events & UV_RENAME || events & UV_CHANGE) {
		GK_ISOLATE();

		auto coordinator = (gk::Coordinator *) handle->data;

		std::string dir (GK_FS_DB_DIR);
		std::string dat = ".gk";
		std::string dirname = "./" + dir + "/" + std::string(filename);
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
			} else {
				auto json = nlohmann::json::parse(buf);
				auto nodeClass = gk::NodeClassFromInt(json["nodeClass"].get<short>());
				if (nodeClass == gk::NodeClass::Entity) {
					// insert the nodes into the Graph.
					auto e = gk::Entity::Instance(isolate, json["type"].get<std::string>().c_str());
					e->id(json["id"].get<long long>());
					e->indexed(true);
					coordinator->insert(isolate, e);

					// groups
					for (auto name : json["groups"]) {
						std::string *v = new std::string(name.get<std::string>());
						e->groups()->insert(*v, v);
					}

					// properties
					for (auto property : json["properties"]) {
						std::string *v = new std::string(property[1].get<std::string>());
						e->properties()->insert(property[0].get<std::string>(), v);
					}
				} else if (nodeClass == gk::NodeClass::Action) {
					// insert the nodes into the Graph.
					auto a = gk::Action < gk::Entity > ::Instance(isolate, json["type"].get<std::string>().c_str());
					a->id(json["id"].get<long long>());
					a->indexed(true);
					coordinator->insert(isolate, a);

					// groups
					for (auto name : json["groups"]) {
						std::string *v = new std::string(name.get<std::string>());
						a->groups()->insert(*v, v);
					}

					// properties
					for (auto property : json["properties"]) {
						std::string *v = new std::string(property[1].get<std::string>());
						a->properties()->insert(property[0].get<std::string>(), v);
					}

					for (auto subject : json["subjects"]) {
						if (subject.is_object()) {
							auto c = coordinator->nodeGraph()->findByKey(gk::NodeClassFromInt(subject["nodeClass"].get<short>()));
							if (c && 0 < c->count()) {
								auto i = c->findByKey(subject["type"].get<std::string>());
								if (i && 0 < i->count()) {
									auto n = i->findByKey(subject["id"].get<long long>());
									if (n) {
										a->addSubject(isolate, dynamic_cast<gk::Entity *>(n));
									}
								}
							}
						}
					}

					for (auto object : json["objects"]) {
						if (object.is_object()) {
							auto c = coordinator->nodeGraph()->findByKey(gk::NodeClassFromInt(object["nodeClass"].get<short>()));
							if (c && 0 < c->count()) {
								auto i = c->findByKey(object["type"].get<std::string>());
								if (i && 0 < i->count()) {
									auto n = i->findByKey(object["id"].get<long long>());
									if (n) {
										a->addObject(isolate, dynamic_cast<gk::Entity *>(n));
									}
								}
							}
						}
					}
				} else if (nodeClass == gk::NodeClass::Bond) {
					// insert the nodes into the Graph.
					auto b = gk::Bond < gk::Entity > ::Instance(isolate, json["type"].get<std::string>().c_str());
					b->id(json["id"].get<long long>());
					b->indexed(true);
					coordinator->insert(isolate, b);

					// groups
					for (auto name : json["groups"]) {
						std::string *v = new std::string(name.get<std::string>());
						b->groups()->insert(*v, v);
					}

					// properties
					for (auto property : json["properties"]) {
						std::string *v = new std::string(property[1].get<std::string>());
						b->properties()->insert(property[0].get<std::string>(), v);
					}

					auto subject = json["subject"];
					if (subject.is_object()) {
						auto c = coordinator->nodeGraph()->findByKey(gk::NodeClassFromInt(subject["nodeClass"].get<short>()));
						if (c && 0 < c->count()) {
							auto i = c->findByKey(subject["type"].get<std::string>());
							if (i && 0 < i->count()) {
								auto n = i->findByKey(subject["id"].get<long long>());
								if (n) {
									b->subject(isolate, dynamic_cast<gk::Entity *>(n));
								}
							}
						}
					}

					auto object = json["object"];
					if (object.is_object()) {
						auto c = coordinator->nodeGraph()->findByKey(gk::NodeClassFromInt(object["nodeClass"].get<short>()));
						if (c && 0 < c->count()) {
							auto i = c->findByKey(object["type"].get<std::string>());
							if (i && 0 < i->count()) {
								auto n = i->findByKey(object["id"].get<long long>());
								if (n) {
									b->object(isolate, dynamic_cast<gk::Entity *>(n));
								}
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

			std::cout << "Count " << coordinator->nodeGraph()->findByKey(gk::NodeClass::Entity)->findByKey("User")->count() << std::endl;
		}
	}
}

bool gk::Coordinator::synched_ = false;
std::shared_ptr<gk::Coordinator::Tree> gk::Coordinator::nodeGraph_;

gk::Coordinator::Coordinator() noexcept {}

gk::Coordinator::~Coordinator() {
	if (1 == nodeGraph_.use_count()) {

		// cleanup when the last instance
		nodeGraph_->clear([](gk::Coordinator::Cluster *cluster) {
			cluster->cleanUp();
			cluster->Unref();
		});
		nodeGraph_.reset();

		// no longer synched
		synched_ = false;

		// cleanup for the request
		if (fs_event_req) {
			free(fs_event_req);
		}
	}
}

std::shared_ptr<gk::Coordinator::Tree> gk::Coordinator::nodeGraph() noexcept {
	if (0 == nodeGraph_.use_count()) {
		nodeGraph_ = std::make_shared<Tree>();
	}
	return nodeGraph_;
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
					auto e = gk::Entity::Instance(isolate, json["type"].get<std::string>().c_str());
					e->id(json["id"].get<long long>());
					e->indexed(true);
					insert(isolate, e);

					// groups
					for (auto name : json["groups"]) {
						std::string *v = new std::string(name.get<std::string>());
						e->groups()->insert(*v, v);
					}

					// properties
					for (auto property : json["properties"]) {
						std::string *v = new std::string(property[1].get<std::string>());
						e->properties()->insert(property[0].get<std::string>(), v);
					}
				} else if (nodeClass == gk::NodeClass::Action) {
					// insert the nodes into the Graph.
					auto a = gk::Action<gk::Entity>::Instance(isolate, json["type"].get<std::string>().c_str());
					a->id(json["id"].get<long long>());
					a->indexed(true);
					insert(isolate, a);

					// groups
					for (auto name : json["groups"]) {
						std::string* v = new std::string(name.get<std::string>());
						a->groups()->insert(*v, v);
					}

					// properties
					for (auto property : json["properties"]) {
						std::string* v = new std::string(property[1].get<std::string>());
						a->properties()->insert(property[0].get<std::string>(), v);
					}

					for (auto subject : json["subjects"]) {
						if (subject.is_object()) {
							auto c = nodeGraph()->findByKey(gk::NodeClassFromInt(subject["nodeClass"].get<short>()));
							if (c && 0 < c->count()) {
								auto i = c->findByKey(subject["type"].get<std::string>());
								if (i && 0 < i->count()) {
									auto n = i->findByKey(subject["id"].get<long long>());
									if (n) {
										a->addSubject(isolate, dynamic_cast<gk::Entity*>(n));
									}
								}
							}
						}
					}

					for (auto object : json["objects"]) {
						if (object.is_object()) {
							auto c = nodeGraph()->findByKey(gk::NodeClassFromInt(object["nodeClass"].get<short>()));
							if (c && 0 < c->count()) {
								auto i = c->findByKey(object["type"].get<std::string>());
								if (i && 0 < i->count()) {
									auto n = i->findByKey(object["id"].get<long long>());
									if (n) {
										a->addObject(isolate, dynamic_cast<gk::Entity*>(n));
									}
								}
							}
						}
					}
				} else if (nodeClass == gk::NodeClass::Bond) {
					// insert the nodes into the Graph.
					auto b = gk::Bond<gk::Entity>::Instance(isolate, json["type"].get<std::string>().c_str());
					b->id(json["id"].get<long long>());
					b->indexed(true);
					insert(isolate, b);

					// groups
					for (auto name : json["groups"]) {
						std::string *v = new std::string(name.get<std::string>());
						b->groups()->insert(*v, v);
					}

					// properties
					for (auto property : json["properties"]) {
						std::string *v = new std::string(property[1].get<std::string>());
						b->properties()->insert(property[0].get<std::string>(), v);
					}

					auto subject = json["subject"];
					if (subject.is_object()) {
						auto c = nodeGraph()->findByKey(gk::NodeClassFromInt(subject["nodeClass"].get<short>()));
						if (c && 0 < c->count()) {
							auto i = c->findByKey(subject["type"].get<std::string>());
							if (i && 0 < i->count()) {
								auto n = i->findByKey(subject["id"].get<long long>());
								if (n) {
									b->subject(isolate, dynamic_cast<gk::Entity*>(n));
								}
							}
						}
					}

					auto object = json["object"];
					if (object.is_object()) {
						auto c = nodeGraph()->findByKey(gk::NodeClassFromInt(object["nodeClass"].get<short>()));
						if (c && 0 < c->count()) {
							auto i = c->findByKey(object["type"].get<std::string>());
							if (i && 0 < i->count()) {
								auto n = i->findByKey(object["id"].get<long long>());
								if (n) {
									b->object(isolate, dynamic_cast<gk::Entity*>(n));
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

		// setup a watcher on the directory
		fs_event_req = (uv_fs_event_t*)malloc(sizeof(uv_fs_event_t));
		fs_event_req->data = (void*)this;
		uv_fs_event_init(uv_default_loop(), fs_event_req);
		// The recursive flag watches subdirectories too.
		uv_fs_event_start(fs_event_req, fs_change_callback, GK_FS_DB_DIR, UV_FS_EVENT_RECURSIVE);
	}
}

bool gk::Coordinator::insert(v8::Isolate* isolate, gk::Coordinator::Node* node) noexcept {
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

bool gk::Coordinator::remove(const ClusterKey& cKey, const IndexKey& iKey, const NodeKey& nKey) noexcept {
	auto cluster = nodeGraph()->findByKey(cKey);
	if (cluster) {
		auto index = cluster->findByKey(iKey);
		if (index) {
			return index->remove(nKey);
		}
	}
	return false;
}
