/*!
 * @brief Declaration of ComponentNotification class
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Files}
 * @file component_notification.hpp
 */

#include "agent-framework/eventing/event_data.hpp"
#include <vector>



namespace agent_framework {
namespace eventing {


class ComponentNotification {
public:

    virtual ~ComponentNotification();

    explicit ComponentNotification() = default;

    ComponentNotification(const ComponentNotification&) = default;

    ComponentNotification(ComponentNotification&&) = default;

    /*!
     * @brief Get GAMI ID the agent identifier
     * @return GAMI ID the agent identifier
     * */
    const std::string& get_gami_id() const {
        return m_gami_id;
    }


    /*!
     * @brief Set GAMI ID the agent identifier
     * @param[in] gami_id the GAMI ID the agent identifier
     * */
    void set_gami_id(const std::string& gami_id) {
        m_gami_id = gami_id;
    }

    /*!
     * @brief Get notification elements included in this notification
     * @return notification elements included in this notification
     * */
    const EventDataVec& get_notifications() const {
        return m_notifications;
    }


    /*!
     * @brief Set notification elements included in this notification
     * @param[in] notifications the notification elements included in this notification
     * */
    void set_notifications(const EventDataVec& notifications) {
        m_notifications = notifications;
    }


private:
    std::string m_gami_id{};
    EventDataVec m_notifications{};
};

}
}
