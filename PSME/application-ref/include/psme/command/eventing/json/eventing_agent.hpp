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
 * @file command/eventing/json/eventing_agent.hpp
 *
 * @brief JSON command EventingAgent interface
 * */

#ifndef PSME_COMMAND_JSON_EVENTING_AGENT_HPP
#define PSME_COMMAND_JSON_EVENTING_AGENT_HPP

#include "psme/command/command_json.hpp"


namespace psme {
namespace command {
namespace eventing {
namespace json {

using psme::command::CommandJson;

/*! JSON eventing agent command class */
class EventingAgent : public CommandJson {
public:
    /*!
     * @brief Create JSON command
     * */
    EventingAgent();

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
} /* namespace eventing */
} /* namespace command */
} /* namespace psme */

#endif /* PSME_COMMAND_JSON_EVENTING_AGENT_HPP */
