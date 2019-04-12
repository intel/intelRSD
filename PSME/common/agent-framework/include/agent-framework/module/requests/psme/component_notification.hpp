/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file command/psme/component_notification.hpp
 * @brief Generic PSME ComponentNotification command
 * */

#pragma once

#include "agent-framework/module/constants/psme.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"

namespace agent_framework {
namespace model {
namespace requests {

/*! ComponentNotification request */
class ComponentNotification {
public:

    explicit ComponentNotification();

    /*!
     * @brief Get command name
     * @return Command name
     * */
    static std::string get_command() {
        return literals::Command::COMPONENT_NOTIFICATION;
    }


    /*!
    * @brief Gets notifications (vector of EventData)
    * @return Notifications
    */
    const attribute::EventData::Vector& get_notifications() const {
        return m_notifications;
    }


    /*!
     * @brief Set notifications
     * @@param notifications (vector of EventData) to send in this request
     */
    void set_notifications(const attribute::EventData::Vector& notifications) {
        m_notifications = notifications;
    }


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
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const;


    /*!
     * @brief create ComponentNotification form Json
     * @param[in] json the input argument
     * @return new ComponentNotification
     */
    static ComponentNotification from_json(const json::Json& json);


    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::ComponentNotification::GAMI_ID, jsonrpc::JSON_STRING,
            literals::ComponentNotification::NOTIFICATIONS, VALID_ARRAY_OF(VALID_ATTRIBUTE(NotificationSchema)),
            nullptr
        };
        return procedure;
    }

private:
    class NotificationSchema {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static const jsonrpc::ProcedureValidator procedure{
                jsonrpc::PARAMS_BY_NAME,
                literals::ComponentNotification::COMPONENT, VALID_UUID,
                literals::ComponentNotification::NOTIFICATION, VALID_ENUM(agent_framework::model::enums::Notification),
                literals::ComponentNotification::TYPE, VALID_ENUM(agent_framework::model::enums::Component),
                literals::ComponentNotification::PARENT, jsonrpc::JSON_STRING,
                nullptr
            };
            return procedure;
        }
    };

    std::string m_gami_id{};
    attribute::EventData::Vector m_notifications{};
};

}
}
}
