/*!
 * @copyright
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
 * @file zone_handler.hpp
 *
 * Specialization of GenericHandler for Zones
 * */

#pragma once
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/requests/common/get_zone_info.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {

using ZoneHandlerBase = GenericHandler<
    agent_framework::model::requests::GetZoneInfo,
    agent_framework::model::Zone,
    IdPolicy<agent_framework::model::enums::Component::Zone, NumberingZone::PARENT_SPACE>>;

/*!
 * @brief ZoneHandler template specialization
 *
 * Zones have collections of Endpoints.
 * Many-to-many-managers are used to store the logical connections between resources.
 * Above is the reason why we need separate implementation.
 */
class ZoneHandler : public ZoneHandlerBase {
public:
    ZoneHandler() : ZoneHandlerBase() {}
    virtual ~ZoneHandler();
protected:

    /*!
     * @brief Overrides remove_agent_data() from GenericManager
     *
     * This override is necessary to properly clean the contents of
     * ZoneEndpointManager
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param[in] gami_id uuid of the agent whose data is to be removed.
     * */
    void remove_agent_data(Context& ctx, const std::string& gami_id) override {
        ZoneHandlerBase::remove_agent_data(ctx, gami_id);

        agent_framework::module::CommonComponents::get_instance()->
            get_zone_endpoint_manager().clean_resources_for_agent(gami_id);
    }

    /*!
     * @brief removes a component and all its descendants from the model
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param uuid component's uuid
     * */
    void do_remove(Context& ctx, const std::string &uuid) override {
        agent_framework::module::CommonComponents::get_instance()->
            get_zone_endpoint_manager().remove_parent(uuid);

        ZoneHandlerBase::do_remove(ctx, uuid);
    }

    /*!
     * @brief updates a single component
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent uuid of parent component
     * @param[in] uuid uuid of updated component
     *
     * @return component's REST id
     */
     std::uint64_t update(Context& ctx, const std::string& parent, const std::string& uuid) override {
         // The only collection under Zone is a 'weak' list of Endpoints
         // therefore we may use recursive do_load to update this list.
         // This solution will not work for all cases as it will reload
         // the whole subtree.
         return do_load(ctx, parent, uuid, true);
     }
};

ZoneHandler::~ZoneHandler() {}

}
}
}
}
