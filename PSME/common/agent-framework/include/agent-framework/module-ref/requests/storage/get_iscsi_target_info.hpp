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
 * @file requests/storage/get_target_info.hpp
 * @brief Storage GetIscsiTargetInfo requests
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_STORAGE_GET_ISCSI_TARGET_INFO_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_STORAGE_GET_ISCSI_TARGET_INFO_HPP


#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! GetIscsiTargetInfo request */
class GetIscsiTargetInfo {
public:
    explicit GetIscsiTargetInfo(const std::string& target);

     static std::string get_command() {
        return "getiSCSITargetInfo";
    }

    /*!
     * @brief Get the uuid of the logical service to be deleted
     * @return Physical service uuid
     * */
    std::string get_uuid() const {
        return m_target;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create GetIscsiTargetInfo form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetIscsiTargetInfo
     */
    static GetIscsiTargetInfo from_json(const Json::Value& json);

private:
    std::string m_target{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODEL_REQUESTS_STORAGE_GET_ISCSI_TARGET_INFO_HPP */
