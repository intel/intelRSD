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

#include "discovery/smbios/system.hpp"
#include "smbios/utilities/conversions.hpp"



using namespace agent_framework::model;
using namespace smbios::parser;

namespace agent {
namespace compute {
namespace discovery {
namespace smbios {

std::tuple<bool, enums::Health> discover_system(SmbiosParser::Ptr smbios, System& system) {
    if (!smbios) {
        return std::make_tuple(false, enums::Health::Warning);
    }

    // discover system GUID and SKU
    auto smbios_system = smbios->get_all<SMBIOS_SYSTEM_INFO_DATA>();
    if (smbios_system.size() > 1) {
        log_warning(GET_LOGGER("smbios-discovery"), "SMBIOS: Only one System information record expected. Number of records: "
                                                 << smbios_system.size());
    }

    auto smbios_module = smbios->get_all<SMBIOS_MODULE_INFO_DATA>();
    if (smbios_module.size() > 1) {
        log_warning(GET_LOGGER("smbios-discovery"),
                    "Only one Module information record expected. Number of records: "
                        << smbios_module.size());
    }

    if (smbios_system.empty()) {
        log_error(GET_LOGGER("smbios-discovery"), "SMBIOS: Unable to discover System information.");
        return std::make_tuple(false, enums::Health::Warning);
    }
    else if (smbios_module.empty()) {
        log_warning(GET_LOGGER("smbios-discovery"), "There are no Modules in SMBIOS table.");
        return std::make_tuple(false, enums::Health::Warning);
    }
    else {
        const auto& sys = smbios_system[0];
        const auto& system_guid = ::smbios::utilities::get_system_uuid(sys.data);
        const auto& sku_number = sys.get_string(sys.data.sku_number);
        const auto& manufacturer = sys.get_string(sys.data.manufacturer);
        const auto& model = sys.get_string(sys.data.product_name);
        const auto& version = sys.get_string(sys.data.version);

        const auto& module = smbios_module[0];
        const auto& serial_number = module.get_string(module.data.serial_number);

        attribute::FruInfo fru_info{};
        fru_info.set_manufacturer(manufacturer);
        fru_info.set_model_number(model);
        fru_info.set_serial_number(serial_number);
        fru_info.set_part_number(version);
        system.set_fru_info(fru_info);
        system.set_guid(system_guid);
        system.set_sku(sku_number);

        log_debug(GET_LOGGER("smbios-discovery"), "System UUID: " << system_guid);
        log_debug(GET_LOGGER("smbios-discovery"), "System SKU: " << sku_number);
        log_debug(GET_LOGGER("smbios-discovery"), "System Manufacturer: " + manufacturer);
        log_debug(GET_LOGGER("smbios-discovery"), "System Model: " + model);
        log_debug(GET_LOGGER("smbios-discovery"), "System Serial Number: " + serial_number);

        log_info(GET_LOGGER("smbios-discovery"), "System Discovery was successful.");
        return std::make_tuple(true, enums::Health::OK);
    }

}

}
}
}
}

