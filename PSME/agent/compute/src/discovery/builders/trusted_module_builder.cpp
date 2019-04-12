/*!
 * @brief Trusted module builder class implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file trusted_module_builder.cpp
 */

#include "discovery/builders/trusted_module_builder.hpp"

#include <algorithm>



using namespace agent::compute::discovery;
using namespace agent_framework::model;

namespace {

const std::string change_interface_type_to_enum(std::string str) {
    std::replace_if(str.begin(), str.end(), [](char ch) { return ch == '.'; }, '_'); // replacing dot
    std::remove_if(str.begin(), str.end(),
                   [](char ch) { return std::isspace<char>(ch, std::locale::classic()); }); // removing whitespaces
    std::string st = str.substr(0, str.size() - 1); // removing last element from string after using remove_if
    return st;
}

}


agent_framework::model::TrustedModule TrustedModuleBuilder::build_default(const std::string& parent_uuid) {
    return TrustedModule{parent_uuid};
}


void TrustedModuleBuilder::update_smbios_tpm_info(agent_framework::model::TrustedModule& trusted_module,
                                                  const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_TPM_INFO_DATA>& smbios_data) {
    enums::State tpm_state = smbios::parser::TPMStatus::ENABLED == smbios_data.data.tpm_status ? enums::State::Enabled
                                                                                               : enums::State::Disabled;
    trusted_module.set_status({tpm_state, {}});
    trusted_module.set_configuration_index(smbios_data.data.tpm_configuration_index);

    try {
        trusted_module.set_interface_type(
            enums::InterfaceType::from_string(
                change_interface_type_to_enum(smbios_data.get_string(smbios_data.data.tpm_version))));
    }
    catch (const agent_framework::exceptions::InvalidValue&) {
        trusted_module.set_interface_type(OptionalField<enums::InterfaceType>{});
    }
}
