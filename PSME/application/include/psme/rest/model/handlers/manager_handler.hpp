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
 * */

#pragma once
#include "agent-framework/module/model/manager.hpp"
#include "agent-framework/module/requests/common/get_manager_info.hpp"
#include "agent-framework/module/requests/common/get_managers_collection.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {

using ManagerHandlerBase = GenericHandler<
        agent_framework::model::requests::GetManagerInfo,
        agent_framework::model::Manager,
        IdPolicy<agent_framework::model::enums::Component::Manager, NumberingZone::SHARED>>;

/*! @brief ManagerHandler template specialization */
class ManagerHandler : public ManagerHandlerBase {
public:
    ManagerHandler() : ManagerHandlerBase() {}
    virtual ~ManagerHandler();
protected:
    template<typename T>
    using Array = agent_framework::model::attribute::Array<T>;
    using Managers = Array<agent_framework::model::attribute::ManagerEntry>;
    using SubcomponentEntry = agent_framework::model::attribute::SubcomponentEntry;

    Array<SubcomponentEntry> fetch_sibling_uuid_list(Context& ctx,
            const std::string& parent_uuid, const std::string& collection_name) override {

        try {
            if (Component::None ==  ctx.get_parent_component()) {

                log_debug("rest", ctx.indent
                    << "[" << static_cast<char>(ctx.mode) << "] "
                    <<  "Fetching list of all managers from agent "
                    << ctx.agent->get_gami_id());

                agent_framework::model::requests::GetManagersCollection collection{};
                auto manager_entries = ctx.agent->execute<Managers>(collection);

                // Convert array of ManagerEntry to Array of SubcomponentEntry
                Array<SubcomponentEntry> response;
                for (const auto& entry : manager_entries) {
                    response.add_entry(entry.get_manager());
                }
                return response;
            }
            else {
                return ManagerHandlerBase::fetch_sibling_uuid_list(ctx, parent_uuid, collection_name);
            }
        }
        catch (const json_rpc::JsonRpcException& e) {
            log_error("rest", ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Agent exception while fetching list of all components of "
                   "type [" << component_s() << "] for parent "
                << parent_uuid <<e.what());
            return Array<SubcomponentEntry>();
        }
    }
};

ManagerHandler::~ManagerHandler() {}

}
}
}
}

