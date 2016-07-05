/*!
 * @section LICENSE
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
 * @section Declaration of SubscriptionManager class
 * @file subscription_manager.hpp
 * */

#ifndef PSME_REST_EVENTING_SUBSCRIPTION_MANAGER
#define PSME_REST_EVENTING_SUBSCRIPTION_MANAGER

#include "agent-framework/generic/singleton.hpp"
#include "psme/rest-ref/eventing/model/subscription.hpp"
#include <mutex>
#include <map>

namespace psme {
namespace rest {
namespace eventing {
namespace manager{

using namespace psme::rest::eventing;
using namespace psme::rest::eventing::model;

using SubscriptionMap = std::map<std::string, Subscription>;
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
    void add(Subscription subscription);

    /*!
     * @brief Get subscription by subscription id
     *
     * @param subscription_id Subscription id
     * @return Subscription
     */
    Subscription get(uint64_t subscription_id);

    /*!
     * @brief Returns subscription by subscription name
     *
     * @param subscription_name Subscription name
     * @return Subscription
     */
    Subscription get(const std::string& subscription_name);

    /*!
     * @brief Get all subscriptions
     *
     * @return Subscription collection
     */
    SubscriptionMap get();

    /*!
     * @brief Removes subscription by subscribtion id
     *
     * @param subscription_id Subscription id
     */
    void del(uint64_t subscription_id);

    /*!
     * @brief Removes subscription by subscribtion name
     *
     * @param subscription_name Subscription name
     */
    void del(const std::string& subscription_name);

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
    /*!
     * @brief Notifies subscribers according to event collection
     * @param event Event collection
     */
    void do_notify(const Event& event);

    Subscription get_by_name(const std::string& subscription_name);
    void del_by_name(const std::string& subscription_name);
    std::mutex m_mutex{};
    SubscriptionMap m_subscriptions{};
    static uint64_t id;
};


}
}
}
}

#endif //PSME_REST_EVENTING_SUBSCRIPTION_MANAGER
