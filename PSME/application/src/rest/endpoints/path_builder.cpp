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

#include "psme/rest/endpoints/path_builder.hpp"
#include "psme/rest/server/request.hpp"

using namespace psme::rest::endpoint;
using namespace psme::rest::constants;

namespace {
    std::string trim_trailing_slash(std::string path) {
        auto len = path.size();
        if (len > 1
            && PathParam::PATH_SEP == path[len - 1]
            && PathParam::PATH_SEP != path[len - 2]) {
            return path.substr(0, len - 1);
        }
        return path;
    }
}

PathBuilder::PathBuilder(const psme::rest::server::Request& request) {
    m_str << trim_trailing_slash(request.get_url());
}

PathBuilder& PathBuilder::append_variable(const std::string& name) {
    m_str << PathParam::PATH_SEP << PathParam::VARIABLE_BEGIN
          << name << PathParam::VARIABLE_END;
    return *this;
}

PathBuilder& PathBuilder::append_regex(const std::string& name, const std::string& regex) {
    m_str << PathParam::PATH_SEP << PathParam::VARIABLE_BEGIN
          << name
          << PathParam::REGEX_MARK
          << regex << PathParam::VARIABLE_END;
    return *this;
}

PathBuilder& PathBuilder::append_jsonptr(const std::string& jsonptr) {
    m_str << Common::HASH << jsonptr;
    return *this;
}


std::string PathBuilder::build() {
    return m_str.str();
}
