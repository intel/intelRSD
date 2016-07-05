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
 *
 *
 * @file request/compute/get_dimm_info.hpp
 * @brief Compute GetDimmInfo request
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_DIMM_INFO_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_DIMM_INFO_HPP

#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! GetDimmInfo request */
class GetDimmInfo {
public:
    explicit GetDimmInfo(const std::string& dimm);

    static std::string get_command() {
        return "getDimmInfo";
    }

    /*!
     * @brief Get dimm from request
     * @return uuid string
     * */
    const std::string& get_uuid() const {
        return m_dimm;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create GetDimmInfo form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetDimmInfo
     */
    static GetDimmInfo from_json(const Json::Value& json);

private:
    std::string m_dimm{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_DIMM_INFO_HPP */
