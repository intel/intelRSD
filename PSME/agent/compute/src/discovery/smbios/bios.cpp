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
 *
 * @file bios.cpp
 *
 * @brief SMBIOS-way of discovering BIOS properties
 * */

#include "agent-framework/module/compute_components.hpp"
#include "discovery/smbios/bios.hpp"

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace smbios::parser;

namespace agent {
namespace compute {
namespace discovery {
namespace smbios {

std::tuple<bool, enums::Health> discover_bios(SmbiosParser::Ptr smbios, System& system) {
    if (!smbios) {
        return std::make_tuple(false, enums::Health::Warning);
    }

    auto smbios_bios = smbios->get_all<SMBIOS_BIOS_INFO_DATA>();
    if (smbios_bios.size() > 1) {
        log_warning(GET_LOGGER("smbios-discovery"), "SMBIOS: Only one BIOS information record expected. Number of records: "
                                                 << smbios_bios.size());
    }

    if (!smbios_bios.empty()) {
        const auto bios = smbios_bios[0];
        const auto &bios_version = bios.get_string(bios.data.version);
        log_debug(GET_LOGGER("smbios-discovery"), "BIOS version: " << bios_version);
        system.set_bios_version(bios_version);

        log_info(GET_LOGGER("smbios-discovery"), "BIOS Discovery was successful.");
        return std::make_tuple(true, enums::Health::OK);
    }

    log_error(GET_LOGGER("smbios-discovery"), "Unable to discover BIOS information.");
    return std::make_tuple(false, enums::Health::Warning);
}

}
}
}
}
