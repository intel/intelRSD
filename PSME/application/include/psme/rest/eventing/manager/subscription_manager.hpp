/*!
 * @section LICENSE
 * Copyright (c) 2015-2019 Intel Corporation
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
#include "database/database.hpp"
#include <mutex>
#include <map>

namespace psme {
namespace rest {
namespace eventing {
namespace manager {

using namespace psme::rest::eventing;
using namespace psme::rest::eventing::model;


/*!
 * SubscriptionManager implementation
 */
class SubscriptionManager : public agent_framework::generic::Singleton<SubscriptionManager> {
public:
    /*!
     * @brief Callback prototype for for_each()
     */
    using SubscriptionCallback = std::function<void(const Subscription&)>;

    /*!
     * @brief Default constructor
     *
     * Subscription manager keeps tracking subscriptions: in start all subscriptions
     * are read from the DB. Each add/del causes DB modifications as well.
     */
    SubscriptionManager();

    /*!
     * @brief Destructor
     */
    virtual ~SubscriptionManager();

    /*!
     * @brief Visit all subscriptions kept by the manager
     * @param handle Callback to be called on each subscription
     * @warning Not allowed to use any SubscriptionManager methods.
     */
    void for_each(const SubscriptionCallback& handle) const;

    /*!
     * @brief Get subscription by subscription id
     *
     * @param subscription_id Subscription id
     * @return Subscription
     */
    Subscription get(uint64_t subscription_id);

    /*!
     * @brief Add subscription
     *
     * @param subscription Subscription
     */
    uint64_t add(Subscription subscription);

    /*!
     * @brief Removes subscription by subscription id
     *
     * @param subscription_id Subscription id
     */
    void del(uint64_t subscription_id);

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
     * @brief Do clean up of the Subscription Manager.
     *
     * @warning This function is required to be called before application exit,
     * as Database is a static vector and causes core dumps
     */
    void clean_up();

private:
    using SubscriptionMap = std::map<std::uint64_t, Subscription>;
    SubscriptionMap m_subscriptions{};

    mutable std::mutex m_mutex{};

    /*! @brief Last assigned ID */
    std::uint64_t m_id{};
    database::Database::SPtr m_db{};
};


}
}
}
}

#endif //PSME_REST_EVENTING_SUBSCRIPTION_MANAGER
