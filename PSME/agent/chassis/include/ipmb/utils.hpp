/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file ipmb/utils.hpp
 * @brief IPMI IPMB Service utils.
 * */

#pragma once

#include <agent-framework/version.hpp>

#include <cstdint>
#include <string>
#include <mutex>

/*! Agent namespace */
namespace agent {
/*! Chassis namespace */
namespace chassis {
/*! IPMB namespace */
namespace ipmb {

using uint8_t = std::uint8_t;

/* MUX General */

/*! MUX MAX ACK LENGTH */
static constexpr const uint8_t MUX_MAX_ACK_LENGTH = 3;

/* I2C General */

/*! I2C Slave address */
static constexpr const uint8_t I2C_SLAVE_ADDRESS = 0x10;
/*! I2C Slave command */
static constexpr const uint8_t I2C_SLAVE_COMMAND = 0x00;
/*! I2C Command length */
static constexpr const uint8_t I2C_COMMAND_LENGTH = 0x01;

/* IPMI General */

/*! Device ID */
static constexpr const uint8_t IPMI_DEVICE_ID = 1;
/*! Device revision */
static constexpr const uint8_t IPMI_DEVICE_REV = 1;
/*! Device IPMI version */
static constexpr const uint8_t IPMI_DEVICE_IPMI_VER = 0x51;
/*! Chassis device */
static constexpr const uint8_t IPMI_CHASSIS_DEVICE = 0x80;
/*! Device major version */
uint8_t IPMI_DEVICE_REV_MAJOR();
/*! Device minor version */
uint8_t IPMI_DEVICE_REV_MINOR();
/*! Device firmware revision */
uint32_t IPMI_AUX_FW_REV();

/*! Maximum IPMI message lenght */
static constexpr const uint16_t IPMI_MAX_MSG_LENGTH = 272;
/*! IPMB frame header size */
static constexpr const uint8_t  IPMB_FRAME_HDR_LEN = 6;
/*! IPMB frame header size with data checksum */
static constexpr const uint8_t  IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN =
                                                        IPMB_FRAME_HDR_LEN + 1;
/*! IPMB checksum section length */
static constexpr const uint8_t  IPMB_CHCKSUM_LEN = 1;

/*! IPMB Destination Address offset */
static constexpr const uint8_t IPMB_OFFSET_DEST_ADDR = 0;
/*! IPMB Net Function/Destination LUN offset */
static constexpr const uint8_t IPMB_OFFSET_NETFN_DLUN = 1;
/*! IPMB Header checksum offset */
static constexpr const uint8_t IPMB_OFFSET_HDR_CHCKSUM = 2;
/*! IPMB Source Address offset */
static constexpr const uint8_t IPMB_OFFSET_SRC_ADDR = 3;
/*! IPMB Sequnce/Source LUN offset */
static constexpr const uint8_t IPMB_OFFSET_SEQ_SLUN = 4;
/*! IPMB Commnad offset */
static constexpr const uint8_t IPMB_OFFSET_CMD = 5;
/*! IPMI Completion Code offset */
static constexpr const uint8_t IPMB_OFFSET_DATA = 6;
/*! IPMI Data Checksum offset */
static constexpr const uint8_t IPMB_OFFSET_DATA_CHCKSUM = 7;
/*! Error frame data payload */
static constexpr const uint8_t IPMB_ERR_FRAM_PAYLOAD_SIZE = 4;

/* IPMI Message */
/*! IPMI message header size */
static constexpr const uint8_t IPMI_MSG_HEADER_SIZE = 2;
/*! Destination LUN mask */
static constexpr const uint8_t IPMI_MSG_DST_LUN_MASK = 0x03;
/*! Source LUN */
static constexpr const uint8_t IPMI_MSG_SRC_LUN_MASK = 0x03;
/*! Net function mask */
static constexpr const uint8_t IPMI_MSG_RSP_NET_FN_MASK = 0x04;
/*! Default LUN */
static constexpr const uint8_t IPMI_MSG_DEFAULT_LUN = 0;
/*! OEM request LUN */
static constexpr const uint8_t IPMI_MSG_OEM_REQ_LUN = 3;
/*! Get message sequnce number */
uint8_t IPMI_MSG_SEQ(uint8_t byte);
/*! Get message Net Function */
uint8_t IPMI_MSG_NETFN(uint8_t byte);
/*! Set request message NetFn/DLUN */
uint8_t IPMI_MSG_SET_REQ_NETFN_DLUN(const uint8_t netfn, const uint8_t dlun);
/*! Set response message NetFn/DLUN */
uint8_t IPMI_MSG_SET_RSP_NETFN_DLUN(const uint8_t netfn, const uint8_t dlun);
/*! Set message Sequnce/SLUN */
uint8_t IPMI_MSG_SET_SEQ_SLUN(const uint8_t seq, const uint8_t slun);
/*! IANA Intel */
static constexpr const uint32_t IPMI_IANA_INTEL = 0x157;
/*! IANA Byte 0 */
uint8_t IPMI_IANA_INTEL_0();
/*! IANA Byte 1 */
uint8_t IPMI_IANA_INTEL_1();
/*! IANA Byte 2 */
uint8_t IPMI_IANA_INTEL_2();

/* I2C */
/*! I2C slave read timeout */
static constexpr uint32_t I2C_SLAVE_READ_TIMEOUT_MSEC = 1000;
/*! I2C master read timeout */
static constexpr uint32_t I2C_MASTER_READ_TIMEOUT_MSEC = 2000;
/*! I2C master write timeout */
static constexpr uint32_t I2C_MASTER_WRITE_TIMEOUT_MSEC = 2000;
/*! I2C master write timeout */
static constexpr uint32_t I2C_MASTER_WRITE_ERR_RETRY_TIMEOUT_MSEC = 100;
/*! I2C writting mode */
static constexpr unsigned char I2C_MODE_WRITE = 1;
/*! I2C read mode */
static constexpr unsigned char I2C_MODE_READ = 0;
/*! I2C read retries */
static constexpr int I2C_WRITE_RETRIES = 5;

/*! MMP Address */
static constexpr const uint8_t I2C_MMP_ADDRESS = 0x24;

/*! GPIO read timeout */
static constexpr const uint8_t GPIO_READ_TIMEOUT_MS = 50;

/*! Get byte */
uint8_t GET_BYTE(uint32_t value, uint8_t byte);

/*!
 * @enum InstanceField
 * @brief Instance Field
 *
 * @var InstanceField RESERVED
 * Reserved Instance
 *
 * @var InstanceField RACKPUID
 * Rack PUID
 *
 * @var InstanceField RACKBPID
 * Rack BPID
 *
 * @var InstanceField TRAYRUID
 * Tray RUID
 *
 * @var InstanceField RACKID
 * Rack Id as string
 * */
enum class InstanceField: uint8_t {
    RESERVED = 0x00,
    RACKPUID = 0x01,
    RACKBPID = 0x02,
    TRAYRUID = 0x03,
    RACKID = 0x04
};

/*!
 * @enum CmdCode
 * @brief IPMI commands codes
 *
 * @var CmdCode Utils::GET_DEV_ID
 * Get Device ID
 *
 * @var CmdCode Utils::WARM_RESET
 * Warm Reset
 *
 * @var CmdCode Utils::SET_LAN_CONFIGURATION
 * Set Lan Configuration
 *
 * @var CmdCode Utils::GET_LAN_CONFIGURATION
 * Get Lan Configuration
 *
 * @var CmdCode Utils::GET_SLED_PRESENCE
 * Get SLED Presence
 *
 * @var CmdCode Utils::SET_ID_FIELD
 * Set ID Field
 *
 * @var CmdCode Utils::GET_ID_FIELD
 * Get ID Field
 *
 * @var CmdCode Utils::GET_AGGR_THERMAL_SENSOR
 * Get Aggregate Thermal Sensor
 *
 * @var CmdCode Utils::GET_AGGR_HEALTH_SENSOR
 * Get Aggregate Health Sensor
 *
 * @var CmdCode Utils::GET_TRAY_POWER
 * Get Tray Power
 *
 * @var CmdCode Utils::SET_AUTHORIZED_CERT
 * Set Authorized Cert
 *
 * @var CmdCode Utils::GET_AUTHORIZED_CERT
 * Get Authorized Cert
 *
 * @var CmdCode Utils::GET_AUTHORIZED_CERT_HASH
 * Get Authorized Cert Hash
 * */
enum class CmdCode {
    // NetFn == 0x06 (application)
    GET_DEV_ID               = 0x01,
    WARM_RESET               = 0x03,
    // NetFn == 0x0C (transport)
    SET_LAN_CONFIGURATION    = 0x01,
    GET_LAN_CONFIGURATION    = 0x02,
    // NetFn == 0x38 (intel)
    GET_SLED_PRESENCE        = 0x02,
    SET_ID_FIELD             = 0x06,
    GET_ID_FIELD             = 0x07,
    GET_AGGR_THERMAL_SENSOR  = 0xC1,
    GET_AGGR_HEALTH_SENSOR   = 0xC2,
    GET_TRAY_POWER         = 0xC3,
    SET_AUTHORIZED_CERT      = 0xC4,
    GET_AUTHORIZED_CERT      = 0xC5,
    GET_AUTHORIZED_CERT_HASH = 0xC6
};

/*!
 * @enum NetFn IPMI Net function
 * @brief Set of net functions used in IPMI communication.
 */
enum class NetFn: uint8_t {
    IPMI_NETFN_CHASSIS      = 0x00, //!< IPMI Chassis Net function
    IPMI_NETFN_BRIDGE       = 0x02, //!< IPMI Bridge Net function
    IPMI_NETFN_APP          = 0x06, //!< IPMI Application Net function
    IPMI_NETFN_TRANSPORT    = 0x0c, //!< IPMI Transport Net function
    IPMI_NETFN_OEM          = 0x30, //!< IPMI OEM Net function
    IPMI_NETFN_INTEL_REQ    = 0x38, //!< IPMI Intel Net function (request)
    IPMI_NETFN_INTEL_RSP    = 0x39  //!< IPMI Intel Net function (response)
};

/*!
 * @enum CompletionCode
 * @brief Set of IPMI completion codes
 * */
enum class CompletionCode: uint8_t {
    CC_OK                         = 0x00,
    CC_NODE_BUSY                  = 0xC0,
    CC_INVALID_CMD                = 0xC1,
    CC_INVALID_CMD_FOR_LUN        = 0xC2,
    CC_TIMEOUT                    = 0xC3,
    CC_OUT_OF_SPACE               = 0xC4,
    CC_INVALID_RESERVATION        = 0xC5,
    CC_REQ_DATA_TRUNCATED         = 0xC6,
    CC_REQ_DATA_LENGTH_INVALID    = 0xC7,
    CC_REQ_DATA_FIELD_LENGTH_EXC  = 0xC8,
    CC_PARAMETER_OUT_OF_RANGE     = 0xC9,
    CC_CANNOT_RETURN_REQ_DATA     = 0xCA,
    CC_REQ_DATA_NOT_PRESENT       = 0xCB,
    CC_INVALID_DATA_FIELD_IN_REQ  = 0xCC,
    CC_COMMAND_ILLEGAL_FOR_SENSOR = 0xCD,
    CC_CANNOT_PROVIDE_RESPONSE    = 0xCE,
    CC_CANNOT_EXEC_DUPLICATED_REQ = 0xCF,
    CC_NODE_NOT_PRESENT           = 0xD3,
    // Command specific CCs
    CC_PARAMETER_NOT_SUPPORTED    = 0x80,
    CC_PARAMETER_SET_IN_PROGRESS  = 0x81,
    CC_PARAMETER_READ_ONLY        = 0x82
};

/*! Utils */
namespace utils {

    /*!
     * @brief Print buffer
     * @param buffer pointer to buffer to be printed
     * @param len buffer length
     * @return Formatted string
     * */
    std::string print_frame(const uint8_t* buffer, const uint16_t len);

    /*!
     * @brief Calculate checksum
     * @param buffer pointer to data buffer
     * @param len data buffer length
     * @return Calculated checksum
     * */
    uint8_t ipmi_checksum(const uint8_t* buffer, uint16_t len);


}

}
}
}

