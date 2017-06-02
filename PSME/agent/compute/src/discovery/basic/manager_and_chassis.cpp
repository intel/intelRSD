/*!
 * @copyright
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
 * */

#include "discovery/common.hpp"
#include "discovery/basic/manager_and_chassis.hpp"

#include "ipmi/management_controller.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/command/generic/get_device_guid.hpp"
#include "ipmi/command/sdv/get_slot_id.hpp"
#include "ipmi/command/sdv/enums.hpp"

#include "agent-framework/module/compute_components.hpp"



using namespace ipmi::command;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::generic;
using namespace agent::compute::discovery;
using namespace agent::compute::discovery::basic;

using COMPLETION_CODE = ipmi::Response::COMPLETION_CODE;
using ChassisRef = ObjReference<agent_framework::model::Chassis, std::recursive_mutex>;

namespace {

std::tuple<enums::Health, std::uint8_t> get_slot_id(::ipmi::ManagementController& mc, std::string uuid) {

    sdv::request::GetSlotId req{};
    sdv::response::GetSlotId res{};

    req.set_network_function(sdv::NetFn::RACKSCALE);

    try {
        mc.send(req, res);
    }
    catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("basic-discovery"), "GetSlotId error:"
                                       << " error: "
                                       << e.what());
        return std::make_tuple(enums::Health::Critical, 0);
    }

    const auto cc = res.get_completion_code();

    if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
        log_bad_completion_code(uuid, cc, "GetSlotId");
        return std::make_tuple(enums::Health::Critical, 0);
    }

    return std::make_tuple(enums::Health::OK, res.get_slot_id());
}


void set_fru_info_for_each_chassis(std::vector<ChassisRef>& vec, const attribute::FruInfo& fru) {
    for (auto& chassis_ref : vec) {
        // Copy of fru info on purpose.
        chassis_ref->set_fru_info(fru);
    }
}

}

namespace agent {
namespace compute {
namespace discovery {
namespace basic {

std::tuple<bool, enums::Health> discover_manager(ipmi::ManagementController& mc,
                                                 Manager& manager,
                                                 attribute::FruInfo& fru_info) {

    log_debug(GET_LOGGER("basic-discovery"), "Sending GetDeviceId...");

    generic::request::GetDeviceId request{};
    generic::response::GetDeviceId response{};

    try {
        mc.send(request, response);
    }
    catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("basic-discovery"), "GetDeviceId error: " << e.what());
        return std::make_tuple(false, enums::Health::Critical);
    }

    auto cc = response.get_completion_code();
    if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
        log_bad_completion_code(manager.get_uuid(), cc, "GetDeviceId");
        return std::make_tuple(false, enums::Health::Critical);
    }

    manager.set_firmware_version(response.get_firmware_version());

    fru_info.set_manufacturer(response.get_manufacturer_name());
    fru_info.set_model_number(response.get_product_name());

    log_debug(GET_LOGGER("basic-discovery"), "Product name: " << response.get_product_name());
    log_debug(GET_LOGGER("basic-discovery"), "Manufacturer name: " << response.get_manufacturer_name());
    log_debug(GET_LOGGER("basic-discovery"), "Firmware version: " << response.get_firmware_version());

    log_info(GET_LOGGER("basic-discovery"), "Manager Discovery was successful.");
    return std::make_tuple(true, enums::Health::OK);
}


std::tuple<bool, enums::Health> discover_all_chassis(ipmi::ManagementController&,
                                                     Manager& manager,
                                                     const attribute::FruInfo& fru_info,
                                                     std::string& out_chassis_uuid) {
    enums::Health health = enums::Health::OK;

    auto chassis_ref_vec = ::get_chassis_refs(manager.get_uuid());
    if (chassis_ref_vec.empty()) {
        log_error(GET_LOGGER("basic-discovery"), "Manager does not have chassis!");
        return std::make_tuple(false, health);
    }
    set_fru_info_for_each_chassis(chassis_ref_vec, fru_info);

    out_chassis_uuid = chassis_ref_vec.front()->get_uuid();
    log_info(GET_LOGGER("basic-discovery"), "Chassis Discovery was successful.");
    return std::make_tuple(true, health);
}


std::tuple<bool, enums::Health> discover_manager_guid(ipmi::ManagementController& mc, Manager& manager) {
    generic::request::GetDeviceGuid request{};
    generic::response::GetDeviceGuid response{};

    try {
        mc.send(request, response);
    }
    catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("basic-discovery"), "Cannot send command GetDeviceGuid: "
                                       << e.what()
                                       << " for Manager: "
                                       << manager.get_uuid());
        return std::make_tuple(false, enums::Health::Warning);
    }

    const auto cc = response.get_completion_code();
    if (COMPLETION_CODE::COMPLETION_CODE_NORMAL != cc) {
        log_bad_completion_code(manager.get_uuid(), cc, "GetDeviceGuid");
        return std::make_tuple(false, enums::Health::Warning);
    }

    manager.set_guid(response.get_guid());

    log_debug(GET_LOGGER("basic-discovery"), "GetDeviceGuid successful. Manager GUID: " << manager.get_guid());
    log_info(GET_LOGGER("basic-discovery"), "Manager GUID Discovery was successful.");
    return std::make_tuple(true, enums::Health::OK);
}


std::tuple<bool, enums::Health> discover_chassis_location(ipmi::ManagementController& mc, Manager& manager) {
    enums::Health health = enums::Health::OK;

    auto chassis_ref_vec = get_chassis_refs(manager.get_uuid());
    if (chassis_ref_vec.empty()) {
        log_error(GET_LOGGER("basic-discovery"), "Manager does not have chassis!");
        return std::make_tuple(false, health);
    }

    std::uint8_t slot_id{};
    std::tie(health, slot_id) = ::get_slot_id(mc, manager.get_uuid());

    if (health == enums::Health::OK) {
        for (auto& chassis_ref : chassis_ref_vec) {
            log_debug(GET_LOGGER("basic-discovery"), "Set Slot ID: "
                                           << std::to_string(unsigned(slot_id))
                                           << " for Chassis: "
                                           << chassis_ref->get_uuid());
            chassis_ref->set_location_offset(slot_id);
        }

        log_info(GET_LOGGER("basic-discovery"), "Chassis location Discovery was successful.");
        return std::make_tuple(true, health);
    }
    return std::make_tuple(false, health);
}

}
}
}
}


