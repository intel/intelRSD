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
 * @file command/registration/json/register_agent.hpp
 *
 * @brief JSON command RegisterAgent interface
 * */

#ifndef PSME_COMMAND_JSON_HEART_BEAT_HPP
#define PSME_COMMAND_JSON_HEART_BEAT_HPP

#include "psme/command/command_json.hpp"

namespace psme {
namespace command {
namespace heart_beat {
namespace json {

using psme::command::CommandJson;

/*! JSON compute command class */
class HeartBeat : public CommandJson {
public:
    /*!
     * @brief Create JSON command
     * */
    HeartBeat();

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
} /* namespace compute */
} /* namespace reg */
} /* namespace psme */

#endif /* PSME_COMMAND_JSON_HEART_BEAT_HPP */
