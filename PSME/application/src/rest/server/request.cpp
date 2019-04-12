/*
 * Copyright (C) 2014 MediaSift Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */

#include "psme/rest/server/request.hpp"
#include "psme/rest/server/methods.hpp"
#include <algorithm>

using namespace psme::rest::server;

void Request::set_method(const Method method) {
	m_method = method;
}

void Request::set_destination(const std::string& destination) {
	m_destination = destination;
}

void Request::set_HTTP_version(const std::string& HTTP_version) {
	m_HTTP_version = HTTP_version;
}

void Request::set_source(const std::string& source) {
	m_source = source;
}

void Request::set_header(const std::string& header, const std::string& value) {
	// All headers set to lower case.
	std::string mut_header;
	mut_header.resize(header.size());

	std::transform(header.begin(), header.end(), mut_header.begin(), ::tolower);

	m_headers[mut_header] = value;
}

void Request::set_body(const std::string& body) {
	m_body = body;
}

void Request::append_body(const std::string& body) {
    m_body.append(body);
}

void Request::set_secure(bool is_request_secure) {
    m_is_secure = is_request_secure;
}

Method Request::get_method() const {
	return m_method;
}

const std::string& Request::get_url() const {
	return m_destination;
}

const std::string& Request::get_HTTP_version() const {
	return m_HTTP_version;
}

const std::string& Request::get_source() const {
	return m_source;
}

std::string Request::get_header(const std::string& header) const {
	// All headers set to lower case.
	std::string mut_header;
	mut_header.resize(header.size());

	std::transform(header.begin(), header.end(), mut_header.begin(), ::tolower);

	HeaderList::const_iterator it = m_headers.find(mut_header);
	if (it != m_headers.end()) {
		return it->second;
	}
	return {};
}

const std::string& Request::get_body() const {
	return m_body;
}

bool Request::is_secure() const {
    return m_is_secure;
}
