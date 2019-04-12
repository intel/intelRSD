/*!
 * @brief Trusted module builder class interface.
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
 * @file trusted_module_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/trusted_module.hpp"
#include "smbios/smbios_parser.hpp"



namespace agent {
namespace compute {
namespace discovery {

class TrustedModuleBuilder {
public:
    /*!
     * @brief Build default trusted module object.
     * @param parent_uuid Parent UUID.
     * @return Built trusted module object.
     */
    static agent_framework::model::TrustedModule build_default(const std::string& parent_uuid);


    /*!
     * @brief Update trusted module object with SMBIOS TPM data.
     * @param trusted_module Trusted module object to be updated with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_tpm_info(agent_framework::model::TrustedModule& trusted_module,
                                       const smbios::parser::SmbiosParser::StructEnhanced <smbios::parser::SMBIOS_TPM_INFO_DATA>& smbios_data);
};

}
}
}
