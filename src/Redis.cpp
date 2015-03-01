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
#include <cstring>
#include <stdarg.h>
#include "Redis.h"

gk::Redis::Redis(const std::string&& host, const int&& port) noexcept
	: host_{std::move(host)},
	  port_{std::move(port)},
	  context_(redisConnect(host_.c_str(), port_)),
	  reader_{redisReaderCreate()} {}

gk::Redis::~Redis() {
	redisFree(context_);
	redisReaderFree(reader_);
}

const std::string& gk::Redis::host() const noexcept {
	return host_;
}

int gk::Redis::port() const noexcept {
	return port_;
}

void gk::Redis::command(const char *format, ...) noexcept {
	va_list args;
	va_start(args, format);
	va_end(args);
	redisvAppendCommand(context_, format, args);
}

void gk::Redis::freeReply(redisReply* reply) noexcept {
	if (NULL != reply) {
		freeReplyObject(reply);
		reply = NULL;
	}
}

redisReply* gk::Redis::execute() const noexcept {
	redisReply* reply;
	redisGetReply(context_, &reply);
	return reply;
}