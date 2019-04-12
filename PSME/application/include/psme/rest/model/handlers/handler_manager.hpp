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
 * @file handler_manager.hpp
 * @brief HandlerManager model interface
 * */
#pragma once

#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/model/attributes/model_attributes.hpp"

#include <vector>
#include <unordered_map>

namespace agent_framework {
namespace model {
namespace enums {
    class Component;
    class CollectionName;
}

namespace attribute {
    class Collection;
}
}
}

namespace psme {
namespace rest {
namespace model {
namespace handler {

struct HandlerInterface;
class HandlerManager : public agent_framework::generic::Singleton<HandlerManager> {
public:
    /*!
     * @brief Default constructor for HandlerManager class. It initializes
     * the component and collection handlers.
     */
    HandlerManager();

    /*!
     * @brief Default destructor for HandlerManager class.
     */
    virtual ~HandlerManager();

    /*!
     * @brief Get a pointer to the handler that handles given component type.
     * If the handler cannot be found, throw an exception.
     *
     * @param[in] component Component type enum
     *
     * @return The handler for the given component type
     */
    HandlerInterface* get_handler(agent_framework::model::enums::Component component);

    /*!
     * @brief Get a pointer to the handler that handles given collection type.
     * If the handler cannot be found, throw an exception.
     *
     * @param[in] collection_type Collection type enum
     *
     * @return The handler for the given component type
     */
    HandlerInterface* get_handler(agent_framework::model::enums::CollectionType collection_type);

    /*!
     * @brief Get a vector of pairs <collection name, pointers to handlers>.
     * The handlers handle component types listed in the collection array.
     * If some (perhaps all) of the handlers couldn't be found, just log error
     * and return the vector anyway (do not throw).
     *
     * @param[in] collections An array of collections
     *
     * @return A vector of pairs of collection name and handlers handling them
     */
    std::vector<std::pair<std::string, HandlerInterface*>> get_handlers(
        agent_framework::model::attribute::Array
        <agent_framework::model::attribute::Collection> collections);

    /*!
     * @brief Get a vector of pointers to resource handlers
     *
     * @return std::vector of pointers to resource handlers
     * */
    std::vector<HandlerInterface*> get_handlers() const;

    /*!
     * @brief Send a request to resource handler to remove agent resources
     *
     * @param[in] in_gami_id Agent GAMI ID
     * */
    void remove_agent_data(const std::string& in_gami_id);

protected:
    // these maps' keys are enums, but upon calling find() or adding an element
    // those enums are being converted to std::uint32_t
    // map for enums::Component::Component_enum
    const std::unordered_map
        <std::uint32_t,
        std::shared_ptr<HandlerInterface>>
        m_component_handlers {};

    // map for enums::CollectionType::CollectionType_enum
    const std::unordered_map
        <std::uint32_t,
        std::shared_ptr<HandlerInterface>>
        m_collection_handlers {};
};


}
}
}
}

