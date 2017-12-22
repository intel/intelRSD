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
 * @section Declaration of SubscriptionManager class
 * @file subscription_manager.hpp
 * */

#ifndef PSME_REST_EVENTING_SUBSCRIPTION_MANAGER
#define PSME_REST_EVENTING_SUBSCRIPTION_MANAGER

#include "agent-framework/generic/singleton.hpp"
#include "psme/rest/eventing/model/subscription.hpp"
#include <mutex>
#include <map>

namespace psme {
namespace rest {
namespace eventing {
namespace manager{

using namespace psme::rest::eventing;
using namespace psme::rest::eventing::model;

using SubscriptionMap = std::map<std::uint64_t, Subscription>;
/*!
 * SubscriptionManager implementation
 */
class SubscriptionManager : public agent_framework::generic::Singleton<SubscriptionManager> {
public:
    /*!
     * @brief Add subscription
     *
     * @param subscription Subscription
     */
    uint64_t add(Subscription subscription);

    /*!
    * @brief Set subscriptions
    *
    * @param subscriptions SubscriptionMap
    */
    void set_subscriptions(const SubscriptionMap& subscriptions);

    /*!
     * @brief Get subscription by subscription id
     *
     * @param subscription_id Subscription id
     * @return Subscription
     */
    Subscription get(uint64_t subscription_id);

    /*!
     * @brief Get all subscriptions
     *
     * @return Subscription collection
     */
    SubscriptionMap get();

    /*!
     * @brief Removes subscription by subscription id
     *
     * @param subscription_id Subscription id
     */
    void del(uint64_t subscription_id);

    /*!
     * @brief Get number of subscriptions
     *
     * @return Number of subscriptions
     */
    uint32_t size();

    /*!
     * @brief Notifies subscribers according to event
     *
     * @param event Event
     */
    void notify(const Event& event);

    /*!
     * @brief Notifies subscribers according to event collection
     * @param events Event collection
     */
    void notify(const EventVec& events);

    /*!
     * @brief Destructor
     */
    virtual ~SubscriptionManager();

private:

    SubscriptionMap m_subscriptions{};
    std::mutex m_mutex{};

    static std::uint64_t id;
};


}
}
}
}

#endif //PSME_REST_EVENTING_SUBSCRIPTION_MANAGER
