/*!
 * @brief Intel Optane DC Persistent Memory firmware interface accessor
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file dcpmem_data_accessor.hpp
 */

#pragma once

#include "dcpmem_commands.hpp"
#include "ipmi/ipmi_controller.hpp"

namespace ipmi {
namespace sdv {

/*!
 * Class responsible for providing access to data from DCPMEM module via FW Interface
 */
class DcpmemDataAccessor final {
public:
    /*!
     * @brief Constructor
     * @param ipmi_controller IpmiController
     * @param bridge BridgeInfo to NodeManager
     */
    explicit DcpmemDataAccessor(IpmiController& ipmi_controller, const ipmi::BridgeInfo& bridge)
        : m_ipmi(ipmi_controller), m_bridge{bridge} {}

    /*!
     * @brief Executes relevant DCPMEM command and returns response buffer
     * @param dimm DIMM for which data is extracted
     * @param command DCPMEM FW Command to execute
     */
    IpmiInterface::ByteBuffer get_dcpmem_data(ipmi::command::sdv::DcpmemDimmLocation dimm,
                                              ipmi::command::sdv::DcpmemCommand command) const;

private:
    void trigger_access(std::uint8_t cpu_number, std::uint8_t smbus_id, std::uint8_t dimm_index,
                         std::uint8_t opcode, std::uint8_t subopcode, std::uint8_t read_offset) const;

    IpmiInterface::ByteBuffer get_readings() const;

    IpmiController& m_ipmi;
    const ipmi::BridgeInfo& m_bridge;
};

}
}