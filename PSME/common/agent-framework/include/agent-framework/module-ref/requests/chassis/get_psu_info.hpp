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
 * @file command/chassis/get_psu_info.hpp
 * @brief Generic chassis GetPsuInfo command
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_GET_PSU_INFO_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_GET_PSU_INFO_HPP


#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! GetPsuInfo request */
class GetPsuInfo {
public:
    explicit GetPsuInfo(const std::string& psu);

    static std::string get_command() {
        return "getPsuInfo";
    }

    /*!
     * @brief Get PSU UUID from request
     * @return uuid string
     * */
    const std::string& get_psu() const {
        return m_psu;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create GetPsuInfo form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetPsuInfo
     */
    static GetPsuInfo from_json(const Json::Value& json);

private:
    std::string m_psu{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODEL_REQUESTS_GET_PSU_INFO_HPP */
