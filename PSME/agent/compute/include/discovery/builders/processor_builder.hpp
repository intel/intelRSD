/*!
 * @brief Processor builder class interface.
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
 * @file processor_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/processor.hpp"
#include "smbios/smbios_parser.hpp"

namespace agent {
namespace compute {
namespace discovery {

class ProcessorBuilder {
public:
    /*!
     * @brief Build default processor object.
     * @param parent_uuid Parent UUID.
     * @return Default build processor object.
     */
    static agent_framework::model::Processor build_default(const std::string& parent_uuid);


    /*!
     * @brief Update processor object with SMBIOS processor data.
     * @param processor Processor object to be filled with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_processor_data(agent_framework::model::Processor& processor,
                                             const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_PROCESSOR_INFO_DATA>& smbios_data);


    /*!
     * @brief Update processor object with SMBIOS Extended CPU ID data.
     * @param processor Processor object to be filled with discovered data.
     * @param cpu_ids_v1 SMBIOS CPU ID subtype 1 data.
     * @param cpu_ids_v2 SMBIOS CPU ID subtype 2 data.
     */
    static void update_smbios_cpu_id_data(agent_framework::model::Processor& processor,
                                          const std::vector<smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_CPUID_DATA>>& cpu_ids_v1,
                                          const std::vector<smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_CPUID_DATA_V2>>& cpu_ids_v2);

    /*!
     * @brief Update processor object with SMBIOS FPGA data.
     * @param processor Processor object to be filled with discovered data.
     * @param smbios_data SMBIOS data.
     */
    static void update_smbios_fpga_data(agent_framework::model::Processor& processor,
                                        const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_FPGA_DATA>& smbios_data);

    /*!
    * @brief Update processor object with SMBIOS FPGA data.
    * @param processor Processor object to be filled with discovered data.
    * @param smbios_data SMBIOS data.
    */
    static void update_smbios_fpga_data(agent_framework::model::Processor& processor,
                                            const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_FPGA_DATA_OEM>& smbios_data);




};

}
}
}
