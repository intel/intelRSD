/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 *
 * @file discovery_utils.hpp
 *
 * @brief Utility and tool functions used in discovery implementation
 * */

#pragma once

#include "agent-framework/module/enum/pnc.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/model/attributes/status.hpp"
#include "gas/utils.hpp"
#include "nvme/smart.hpp"
#include "fru_eeprom/utils.hpp"

#include <string>
#include <iomanip>
#include <sstream>

namespace agent {
namespace pnc {
namespace discovery {
namespace utils {

/*! Unbind Port delay for discovery */
static constexpr uint8_t UNBIND_BIND_PORT_DISCOVERY_DELAY_MSEC = 100;

/*!
 * @brief Function used to convert PartitionState enum into valid Health enum
 * @param[in] ps Partition state to be converted
 * @return Health
 * */
agent_framework::model::enums::Health partition_state_to_status_health(
    const agent::pnc::gas::PartitionState& ps);

/*!
 * @brief Function used to convert PartitionStatus enum into valid State enum
 * @param[in] ps Partition status to be converted
 * @return State
 * */
agent_framework::model::enums::State partition_status_to_status_state(
    const agent::pnc::gas::PartitionStatus& ps);

/*!
 * @brief Function used to convert State Operation bits into valid Status
 * @param[in] so State Operation bits to be converted
 * @return Status
 * */
agent_framework::model::attribute::Status state_operation_result_to_status(uint32_t so);

/*!
 * @brief Function used to decode hexadecimal class code into its enum representation
 * @param[in] dc Numeric code of the device class
 * @return Pcie Function Device Class enum
 * */
agent_framework::model::enums::PcieFunctionDeviceClass to_device_class_enum(uint8_t dc);

/*!
 * @brief Function used to strip spaces from both ends of the string,
 * eg.: "   abc   def   " will be converted to "abc  def".
 * @param[in] str String to be stripped
 * @return Stripped string
 * */
std::string strip_spaces(const std::string& str);

/*!
 * @brief Function that converts raw byte table into strings. It converts all null characters
 * into spaces and strips spaces from both sides of the string. It requires providing
 * valid data buffer start and its size. It works also for buffers not ended with null
 * character.
 * @param[in] data Pointer to the buffer start
 * @param[in] size Size of the buffer
 * @return Converted string
 * */
std::string get_null_terminated_string(const std::uint8_t* data, const std::uint32_t size);

/*!
 * @brief Function is smilar to get_null_terminated_string(), but returns optional string.
 * @param[in] data Pointer to the buffer start
 * @param[in] size Size of the buffer
 * @return Converted optional string
 * */
OptionalField<std::string> get_null_terminated_optional_string(const std::uint8_t* data, const std::uint32_t size);

/*!
 * Transforms FRU Chassis type to Model Chassis type.
 * @param fru_type FRU Chassis type
 * @return Model Chassis type
 */
agent_framework::model::enums::ChassisType fru_chassis_type_to_model_chassis_type(
            fru_eeprom::parser::SystemEnclosureOrChassisType fru_type);

/*!
 * @brief update endpoint-zone binding from db during port discovery
 * @param[in] zones_uuids Uuids of the zones
 * @param[in] endpoint_uuid Uuid of the discovered endpoint
 */
void update_endpoint_zone_binding_from_db(const std::vector<Uuid>& zones_uuids,
                                          const Uuid& endpoint_uuid);

}
}
}
}
