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

#include <algorithm>
#include "discovery/common.hpp"
#include "discovery/smbios/manager_and_chassis.hpp"
#include "agent-framework/module/compute_components.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace smbios::parser;

namespace agent {
namespace compute {
namespace discovery {
namespace smbios {

bool discover_manager(::smbios::parser::SmbiosParser::Ptr,
                      agent_framework::model::Manager&,
                      agent_framework::model::attribute::FruInfo&) {
    log_warning(GET_LOGGER("smbios-discovery"), "Manager information cannot be retrieved from SMBIOS.");
    return false;
}


bool discover_all_chassis(::smbios::parser::SmbiosParser::Ptr smbios,
                          Manager& manager,
                          std::string& out_chassis_uuid) {
    if (!smbios) {
        return false;
    }

    auto smbios_module = smbios->get_all<SMBIOS_MODULE_INFO_DATA>();
    if (smbios_module.size() > 1) {
        log_warning(GET_LOGGER("smbios-discovery"),
                    "Only one Module information record expected. Number of records: "
                    << smbios_module.size());
    }
    if (smbios_module.empty()) {
        log_warning(GET_LOGGER("smbios-discovery"), "There are no Modules in SMBIOS table.");
        return false;
    }
    const auto& module = smbios_module[0];

    auto smbios_chassis = smbios->get_all<SMBIOS_SYSTEM_ENCLOSURE_OR_CHASSIS>(&module.data.chassis_handle);
    if (smbios_chassis.empty()) {
        log_warning(GET_LOGGER("smbios-discovery"), "SMBIOS: Chassis (handle: 0x" << std::hex << module.data.chassis_handle
                                                 << ") not found in SMBIOS table" << smbios_module.size());
        return false;
    }
    const auto& chassis = smbios_chassis[0];

    const auto& manufacturer = chassis.get_string(chassis.data.manufacturer);
    const auto& model = module.get_string(module.data.product);
    const auto& sku = chassis.get_string(chassis.data.get_sku());
    const auto& serial_number = chassis.get_string(chassis.data.serial_number);
    const auto& part_number = chassis.get_string(chassis.data.version);
    const auto& asset_tag = chassis.get_string(chassis.data.asset_tag_number);

    attribute::FruInfo fruInfo;
    fruInfo.set_manufacturer(manufacturer);
    fruInfo.set_model_number(model);
    fruInfo.set_serial_number(serial_number);
    fruInfo.set_part_number(part_number);

    auto chassis_ref_vec = agent::compute::discovery::get_chassis_refs(manager.get_uuid());
    if (chassis_ref_vec.empty()) {
        log_error(GET_LOGGER("smbios-discovery"), "Manager does not have chassis!");
        return false;
    }
    for (auto& chassis_ref : chassis_ref_vec) {
        chassis_ref->set_fru_info(fruInfo);
        chassis_ref->set_sku(sku);
        chassis_ref->set_asset_tag(asset_tag);
    }

    out_chassis_uuid = chassis_ref_vec.front()->get_uuid();
    log_info(GET_LOGGER("smbios-discovery"), "Chassis Discovery was successful.");
    return true;
}


bool discover_manager_guid(::smbios::parser::SmbiosParser::Ptr, Manager&) {
    log_warning(GET_LOGGER("smbios-discovery"), "Manager GUID cannot be retrieved from SMBIOS.");
    return false;
}


bool discover_chassis_location(::smbios::parser::SmbiosParser::Ptr, Manager&) {
    log_warning(GET_LOGGER("smbios-discovery"), "Chassis location information cannot be retrieved from SMBIOS.");
    return false;
}

}
}
}
}

