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
 * @file drive_builder.hpp
 *
 * @brief DriveBuilder interface
 * */

#pragma once

#include "discovery/builders/abstract_builder.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "sysfs/sysfs_dto.hpp"
#include "nvme/nvme.hpp"
#include "nvme/smart.hpp"
#include "nvme/vital_product_data.hpp"
#include "nvme/fw_version.hpp"
#include "tools/toolset.hpp"
#include "fru_eeprom/fru_eeprom_parser.hpp"

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder used to create drive objects
 */
class DriveBuilder : public AbstractBuilder<agent_framework::model::Drive> {

public:

    using ReturnType = DriveBuilder&;

    DriveBuilder() = default;
    virtual ~DriveBuilder() = default;

    /*!
     * @brief Updates sysfs data of the drive (capacity)
     * @param[in] sysfs_drive Drive data read from sysfs
     * @return Pointer to the builder
     * */
    virtual ReturnType update_sysfs_drive_data(const sysfs::SysfsDrive& sysfs_drive);

    /*!
     * @brief Updates sysfs data of the device (link speed/width)
     * @param[in] sysfs_device Drive's device in the sysfs
     * @return Pointer to the builder
     * */
    virtual ReturnType update_sysfs_device_data(const sysfs::SysfsDevice& sysfs_device);

    /*!
     * @brief Updates location of the drive
     * @param[in] port Physical port of the drive
     * @return Pointer to the builder
     * */
    virtual ReturnType update_location(const agent_framework::model::Port& port);

    /*!
     * @brief Updates vpd of the drive
     * @param[in] vpd Read vpd data structure
     * @return Pointer to the builder
     * */
    virtual ReturnType update_vpd(const nvme::VitalProductData& vpd);

    /*!
     * @brief Updates vpd of the drive
     * @param[in] parser Fru Ipmi format parser used as data source
     * @return Pointer to the builder
     * */
    virtual ReturnType update_vpd(const fru_eeprom::parser::FruEepromParser& parser);

    /*!
     * @brief Updates SMART data of the drive
     * @param[in] t Toolset used to extract data
     * @param[in] smart Read smart data structure
     * @return Pointer to the builder
     * */
    virtual ReturnType update_smart(const tools::Toolset& t, const nvme::Smart& smart);

    /*!
     * @brief Updates FirmwareVersion data of the drive
     * @param[in] fw_ver Read FirmwareVersion data structure
     * @return Pointer to the builder
     * */
    virtual ReturnType update_firmware_version(const nvme::FirmwareVersion& fw_ver);

    /*!
     * @brief Updates drive to be in critical discovery state
     * @return Pointer to the builder
     * */
    virtual ReturnType update_critical_state();

    /*!
     * @brief Adds link to a dsp port
     * @return dsp_port_uuid Dsp port uuid
     * */
    virtual ReturnType add_dsp_port_uuid(const std::string& dsp_port_uuid);

    /*!
     * @brief Updates processor FRU info
     * @param fru_info FRU info
     * @return Pointer to the builder
     */
    ReturnType update_fru_info(const agent_framework::model::attribute::FruInfo& fru_info);

protected:
    /*!
     * @brief Performs construction of the default object
     * */
    virtual void build_default() override;
};

}
}
}
}
