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
 * @file command/network/json/set_component_attributes.hpp
 * @brief Declaration of JSON SetComonentsAttributes command interface.
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_NETWORK_JSON_SET_COMPONENT_ATTRIBUTES_HPP
#define AGENT_FRAMEWORK_COMMAND_NETWORK_JSON_SET_COMPONENT_ATTRIBUTES_HPP

#include "agent-framework/command/command_json.hpp"
#include "agent-framework/command/network/set_component_attributes.hpp"

namespace agent_framework {
namespace command{
namespace network {
namespace json {

using agent_framework::command::CommandJson;

/*! @brief JSON SetComonentsAttributes command class */
class SetComponentAttributes : public CommandJson {
public:
    /*! @brief Agent tag string */
    static constexpr const char* AGENT = network::SetComponentAttributes::AGENT;
    /*! @brief Command name tag string */
    static constexpr const char* TAG = network::SetComponentAttributes::TAG;

    /*! @brief Command Constructor */
    SetComponentAttributes();

    /*!
     * @brief JSON RPC method
     *
     * @param[in]   params  JSON RPC request parameters
     * @param[out]  result  JSON RPC response results
     * */
    void method(const Json::Value& params, Json::Value& result) final override;

    /*!
     * @brief JSON RPC notification
     *
     * @param[in] params JSON RPC request parameters
     * */
    void notification(const Json::Value& params) final override;
};

}
}
}
}

#endif /*! AGENT_FRAMEWORK_COMMAND_NETWORK_JSON_SET_COMPONENT_ATTRIBUTES_HPP */
