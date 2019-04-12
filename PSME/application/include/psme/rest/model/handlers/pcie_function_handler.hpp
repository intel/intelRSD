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
 * @file pcie_function_handler.hpp
 *
 * Specialization of GenericHandler for PcieFunctions
 * */

#pragma once



#include "agent-framework/module/requests/common/get_pcie_function_info.hpp"
#include "agent-framework/module/common_components.hpp"



namespace psme {
namespace rest {
namespace model {
namespace handler {

using PcieFunctionHandlerBase = GenericHandler <
agent_framework::model::requests::GetPcieFunctionInfo,
agent_framework::model::PcieFunction,
IdPolicy<agent_framework::model::enums::Component::PcieFunction, NumberingZone::PARENT_SPACE>>;

/*!
 * @brief PcieFunctionHandler template specialization.
 *
 * This is necessary because we have to handle PcieFunctions assigned to PcieZones in
 * the way that is inconsistent with most other components.
 * */
class PcieFunctionHandler : public PcieFunctionHandlerBase {
public:
    PcieFunctionHandler() : PcieFunctionHandlerBase() {}


    virtual ~PcieFunctionHandler();


protected:
    using PncComponents = agent_framework::module::PncComponents;

    /*!
     * @brief collection might be strong or weak
     *
     * @param[in] parent_component Component of parent node
     */
    bool is_strong_collection(const Component parent_component) {
        return (Component::PcieDevice == parent_component);
    }


    /*!
     * @brief Specialization of fetch_siblings()
     *
     * An PcieFunction can be either a subcomponent of a PcieDevice or a Drive.
     * This code handles both cases.
     *
     * @param[in] ctx - keeps data that is required during processing and
     * needs to be passed down to sub-handlers
     * @param[in] parent_uuid - uuid of parent whose subcomponents we want to
     * retrieve
     * @param[in] collection_name name of the collection
     */
    void fetch_siblings(Context& ctx, const std::string& parent_uuid,
                        const std::string& collection_name) override {
        if (is_strong_collection(ctx.get_parent_component())) {
            return PcieFunctionHandlerBase::fetch_siblings(ctx, parent_uuid,
                                                           collection_name);
        }
        else if (Component::Drive == ctx.get_parent_component()) {
            fetch_parent_children(ctx, parent_uuid, collection_name,
                                  agent_framework::module::PncComponents::get_instance()->get_drive_function_manager());
        }
        else {
            log_debug("rest", ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Found a PcieZone under an unexpected parent " << parent_uuid);
        }
    }


    /*!
     * @brief  Specialization of remove_agent_data()
     *
     * This override is necessary to properly clean the Drive <-> PcieFunction
     * bindings for all the PcieFunctions.
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param[in] gami_id uuid of the agent whose data is to be removed.
     * */
    void remove_agent_data(Context& ctx, const std::string& gami_id) override {
        PncComponents::get_instance()->
            get_drive_function_manager().clean_resources_for_agent(gami_id);
        PcieFunctionHandlerBase::remove_agent_data(ctx, gami_id);
    }


    /*!
     * @brief  Specialization of do_remove() from GenericManager.
     *
     * This override is necessary for clearing the Drive <-> PcieFunction bindings.
     *
     * @param[in] ctx keeps data that is required during processing and needs to be passed down to sub-handlers
     * @param[in] uuid uuid of the function to be removed.
     * */
    void do_remove(Context& ctx, const std::string& uuid) override {
        // function is the child in Drive <-> PcieFunction relation
        PncComponents::get_instance()->
            get_drive_function_manager().remove_child(uuid);
        PcieFunctionHandlerBase::do_remove(ctx, uuid);
    }


    bool do_accept_recursively(ResourceVisitor& visitor,
                               const std::string& parent_uuid,
                               const Component parent_component) override {
        if (Component::PcieDevice == parent_component) {
            return PcieFunctionHandlerBase::do_accept_recursively(visitor, parent_uuid, parent_component);
        }
        else {
            auto& manager = PncComponents::get_instance()->get_drive_function_manager();
            auto children = manager.get_children(parent_uuid);
            for (const std::string& child_uuid : children) {
                if (!do_accept(visitor, child_uuid)) {
                    return false; // break
                }
            }
        }
        return true;
    }
};


PcieFunctionHandler::~PcieFunctionHandler() {}

}
}
}
}

