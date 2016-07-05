/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#ifndef APP_REST_MAPPER_HPP
#define APP_REST_MAPPER_HPP

#include "psme/rest-ref/server/parameters.hpp"

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
    * @brief a method converting link to component to Parameters object containing
    * a map of component ID name and component ID
    *
    * @param path Link to component
    *
    * @return Parameters object
    */
    static server::Parameters get_params(const std::string& path);

private:
    using RequestParamsMap = std::unordered_map<std::string, std::string>;

    static RequestParamsMap m_params_map;
};

}
}
}

#endif
