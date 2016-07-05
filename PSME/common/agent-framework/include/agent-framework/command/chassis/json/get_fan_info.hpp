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
 * @file command/chassis/json/get_fan_info.hpp
 *
 * @brief JSON command get component info interface
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_CHASSIS_JSON_GET_FAN_INFO_HPP
#define AGENT_FRAMEWORK_COMMAND_CHASSIS_JSON_GET_FAN_INFO_HPP

#include "agent-framework/command/command_json.hpp"
#include "agent-framework/command/chassis/get_fan_info.hpp"

namespace agent_framework {
namespace command {
namespace chassis {
namespace json {

using namespace agent_framework::command;
using agent_framework::command::CommandJson;

/*! JSON chassis command class */
class GetFanInfo : public CommandJson {
public:
    /*! Agent name tag */
    static constexpr const char* AGENT = chassis::GetFanInfo::AGENT;

    /*! Command name tag */
    static constexpr const char* TAG = chassis::GetFanInfo::TAG;

    /*!
     * @brief Create JSON command
     * */
    GetFanInfo();

    /*!
     * @brief JSON RPC method
     *
     * @param[in] params JSON RPC params request
     * @param[out] result JSON RPC result response
     * */
    void method(const Json::Value& params, Json::Value& result) final override;

    /*!
     * @brief JSON RPC notification
     *
     * @param[in] params JSON RPC params request
     * */
    void notification(const Json::Value& params) final override;
};

} /* namespace json */
} /* namespace chassis */
} /* namespace command */
} /* namespace agent_framework */

#endif	/* AGENT_FRAMEWORK_COMMAND_CHASSIS_JSON_GET_FAN_INFOHPP */
