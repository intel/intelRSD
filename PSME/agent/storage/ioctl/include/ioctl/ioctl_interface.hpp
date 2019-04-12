/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ioctl_interface.hpp
 */

#pragma once



#include "agent-framework/module/utils/optional_field.hpp"
#include <memory>

namespace agent {
namespace storage {
namespace ioctl {

class IoctlInterface {
public:
    using HdioData = std::uint16_t[];
    using HdioDataPtr = std::unique_ptr<HdioData>;

    IoctlInterface(const std::string& device_name) : m_device_name(device_name) {}

    /*!
     * @brief Reads ATA attributes from ioctl commands
     * @return Pointer to array with HDIO data
     */
    HdioDataPtr read_ata_attributes();

    /*!
     * @brief Unpack serial number from HDIO data
     * @param hdio_data Pointer to HDIO data
     * @return Unpacked serial number or null
     */
    OptionalField<std::string> get_serial_number(const HdioDataPtr& hdio_data);

    /*!
     * @brief Unpack model number from HDIO data
     * @param hdio_data Pointer to HDIO data
     * @return Unpacked model number or null
     */
    OptionalField<std::string> get_model_number(const HdioDataPtr& hdio_data);

    /*!
     * @brief Unpack manufacturer name from HDIO data
     * @param hdio_data Pointer to HDIO data
     * @return Unpacked manufacturer name or null
     */
    OptionalField<std::string> get_manufacturer(const HdioDataPtr& hdio_data);

    /*!
     * @brief Unpack RPM from HDIO data
     * @param hdio_data Pointer to HDIO data
     * @return Unpacked RPM value or null
     */
    OptionalField<std::uint32_t> get_rpm(const HdioDataPtr& hdio_data);

private:
    std::string m_device_name{};

    void perform_hdio_drive_cmd(HdioDataPtr& hdio_data);
    void perform_sg_io_cmd(HdioDataPtr& hdio_data);
    OptionalField<std::string> read_ata_string(std::size_t offset, std::size_t length, const HdioDataPtr& hdio_data);

};

}
}
}