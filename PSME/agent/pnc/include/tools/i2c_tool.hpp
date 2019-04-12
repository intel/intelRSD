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
 * @file i2c_tool.hpp
 *
 * @brief I2cTool class used for i2c operations - reading various oob fields.
 * */

#pragma once

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "seeprom.hpp"
#include "cable_id.hpp"
#include "fru_eeprom.hpp"
#include "nvme/vital_product_data.hpp"
#include "nvme/smart.hpp"
#include "nvme/fw_version.hpp"

namespace agent {
namespace pnc {
namespace tools {

/*! I2cTool class */
class I2cTool {
public:

    /*! Default constructor */
    I2cTool() {}

    /*! Enable copy */
    I2cTool(const I2cTool&){}
    I2cTool& operator=(const I2cTool&) {return *this;}

    /*! Default destructor */
    virtual ~I2cTool();

    /*!
     * @brief Reads seeprom of the fabric Switch
     * @param[out] seeprom Reference to the object to be filled with read data
     * @return True if successful
     * */
    virtual bool get_seeprom(Seeprom& seeprom) const;

    /*!
     * @brief Reads cable id on the provided port
     * @param[out] cable_id Reference to the object to be filled with read data
     * @param[in] port Upstream port used to read cable id
     * @return True if successful
     * */
    virtual bool get_cable_id(CableId& cable_id, const agent_framework::model::Port& port) const;

    /*!
     * @brief Reads vital product data on the provided port
     * @param[out] vpd Reference to the object to be filled with read data
     * @param[in] port Downstream port used to read vpd
     * @return True if successful
     * */
    virtual bool get_vpd(nvme::VitalProductData& vpd, const agent_framework::model::Port& port) const;

    /*!
     * @brief Reads smart data on the provided port
     * @param[out] smart Reference to the object to be filled with read data
     * @param[in] port Downstream port used to read smart
     * @return True if successful
     * */
    virtual bool get_smart(nvme::Smart& smart, const agent_framework::model::Port& port) const;

    /*!
     * @brief Reads firmware version data on the provided port
     * @param[out] fw Reference to the object to be filled with read data
     * @param[in] port Downstream port used to read fw_version
     * @return True if successful
     * */
    virtual bool get_firmware_version(nvme::FirmwareVersion& fw, const agent_framework::model::Port& port) const;

    /*!
     * @brief Reads FRU Eeprom data.
     * @param fru_eeprom Refernce to the object to be filled with read data
     * @return True if successful
     */
    bool get_fru_eeprom(FruEeprom& fru_eeprom) const;
};

using I2cToolPtr = std::shared_ptr<I2cTool>;

}
}
}
