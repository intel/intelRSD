/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#pragma once
#include "psme/rest/server/parameters.hpp"

#include <unordered_map>

namespace psme {
namespace rest {
namespace model {

class Mapper {
public:
    /*!
     * @brief Maps resource collection name to resource ID name
     *
     * @param component Resource collection name
     */
    static std::string get_param_from_component(const std::string& component);

    /*!
     * @brief Method converting link to component to Parameters object containing a map of
     * component ID name and component ID. Moreover checks whether the path matches the path template.
     *
     * @param[in] path Link to component.
     * @param[in] path_template Path will be checked with the path template.
     *
     * @return Parameters object.
     * @throw Throws NotFound exception if path does not match the template or resource name does not exist.
     * */
    static server::Parameters get_params(const std::string& path, const std::string& path_template);

private:
    using RequestParamsMap = std::unordered_map<std::string, std::string>;

    static RequestParamsMap m_params_map;
};

}
}
}

