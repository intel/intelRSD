/*!
 * @section LICENSE
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
 *
 * @section Declaration of SubscriptionConfig class
 * @file subscription_config.hpp
 * */

#pragma once
#include "agent-framework/generic/singleton.hpp"
#include "psme/rest/eventing/model/subscription.hpp"
#include "psme/rest/eventing/manager/subscription_manager.hpp"
#include <mutex>
#include <map>

namespace psme {
namespace rest {
namespace eventing {
namespace config {

using namespace psme::rest::eventing;
using namespace psme::rest::eventing::manager;
using namespace psme::rest::eventing::model;
/*!
 * SubscriptionConfig implementation
 */
class SubscriptionConfig : public agent_framework::generic::Singleton<SubscriptionConfig> {
public:
    /*!
     * @brief Default subscription file path
     */
    static constexpr const char DEFAULT_SUBSCRIPTION_FILE_PATH[] = "/var/opt/psme/subscriptions";

    /*!
     * @brief Set subscription configuration file
     * @param config_file_path Subscription configuration file path
     */
    void set_config_file(const std::string& config_file_path);

    /*!
     * @brief Save subscriptions to file
     */
    void save();

    /*!
     * @brief Load subscriptions from file
     */
    void load();

    /*!
     * @brief Destructor
     */
    virtual ~SubscriptionConfig();
private:
    json::Value get_subscriptions_json(const SubscriptionMap& subscriptions) const;
    json::Value get_subscriptions_json(const std::string& json_content) const;
    std::mutex m_mutex{};
    std::string m_config_file_path{DEFAULT_SUBSCRIPTION_FILE_PATH};
};


}
}
}
}

