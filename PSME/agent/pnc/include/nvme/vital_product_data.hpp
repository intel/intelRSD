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
 * @file vital_product_data.hpp
 * @brief NVMe drive Vital Product Data structure
 * */

#pragma once

#include "i2c/i2c_access_interface.hpp"
#include "gas/utils.hpp"
#include "agent-framework/module/enum/chassis.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! NVMe namespace */
namespace nvme {

/*! NVMe drive Vital Product Data structure */
class VitalProductData {
    agent_framework::model::enums::PlatformType m_platform{agent_framework::model::enums::PlatformType::EDK};

#pragma pack(push, 1)
    struct pcie_capabilities_t {
        uint8_t max_link_speed;
        uint8_t max_link_width;
    };
#pragma pack(pop)
    void select_channel(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port,
        gas::PCA9548TwiExpanderChannel channel);
    void read_vpd(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port);
    void log_vpd(gas::PM85X6TwiPort port, gas::PCA9548TwiExpanderChannel channel);
public:

    /*! Size of the VPD if stored in SFF format */
    static constexpr uint8_t NVME_VPD_SFF_FORMAT_SIZE_BYTES = 77;
    /*! Size of the VPD if stored in FRU IPMI format */
    static constexpr uint8_t NVME_VPD_FRU_IPMI_FORMAT_SIZE_BYTES = 192;

    /*! Size of fields struct (in bytes): larger of the above */
    /* TODO reading too many bytes may be an issue on EDK platform - this is to be checked */
    static constexpr uint8_t NVME_VPD_STRUCTURE_SIZE_BYTES = NVME_VPD_FRU_IPMI_FORMAT_SIZE_BYTES;

    /*! Size of the class_code field of the output (in bytes) */
    static constexpr uint8_t NVME_CLASS_CODE_SIZE_BYTES = 3;
    /*! Size of the serial_number field of the output (in bytes) */
    static constexpr uint8_t NVME_SERIAL_NUMBER_SIZE_BYTES = 20;
    /*! Size of the model_number field of the output (in bytes) */
    static constexpr uint8_t NVME_MODEL_NUMBER_SIZE_BYTES = 40;

    /*! Default constructor */
    VitalProductData(agent_framework::model::enums::PlatformType platform): m_platform(platform) {}

    /*! Copy constructor */
    VitalProductData(const VitalProductData&) = default;

    /*! Assignment operator */
    VitalProductData& operator=(const VitalProductData&) = default;

#pragma pack(push, 1)
    /*! Vital Product Data */
    union {
        struct {
            uint8_t class_code[NVME_CLASS_CODE_SIZE_BYTES];
            uint16_t vendor_id;
            uint8_t serial_number[NVME_SERIAL_NUMBER_SIZE_BYTES];
            uint8_t model_number[NVME_MODEL_NUMBER_SIZE_BYTES];
            pcie_capabilities_t pcie_port0;
            pcie_capabilities_t pcie_port1;
            uint8_t init_power_reqs;
            uint16_t reserved0;
            uint8_t max_power_reqs;
            uint16_t reserved1;
            uint16_t caps_list_ptr;
        } fields{};

        uint8_t raw_data[NVME_VPD_STRUCTURE_SIZE_BYTES];
    };
#pragma pack(pop)

    /*!
     * @brief Reads Vital Product Data of drive connected to given port and channel.
     *
     * @param i2c Interface used to read vpd
     * @param port TWI Port number
     * @param channel TWI Expander channel number
     * */
    void read(const i2c::I2cAccessInterfacePtr i2c, gas::PM85X6TwiPort port,
        gas::PCA9548TwiExpanderChannel channel);

    virtual ~VitalProductData();
};

}
}
}
