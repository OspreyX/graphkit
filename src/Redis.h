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

#ifndef GRAPHKIT_SRC_REDIS_H
#define GRAPHKIT_SRC_REDIS_H

#include <string>
#include <hiredis/hiredis.h>

namespace gk {
	class Redis {
	public:
		Redis(const std::string&& host, const int&& port) noexcept;
		virtual ~Redis();
		Redis(const Redis&) = default;
		Redis& operator= (const Redis&) = default;
		Redis(Redis&&) = default;
		Redis& operator= (Redis&&) = default;

		const std::string& host() const noexcept;
		int port() const noexcept;

		void command(const char* format, ...) noexcept;
		redisReply* execute() const noexcept;
		void freeReply(redisReply* reply) noexcept;

	protected:
		const std::string host_;
		const int port_;
		redisContext* context_;
		redisReader* reader_;
	};
}

#endif