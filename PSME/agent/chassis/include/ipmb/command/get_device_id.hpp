/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file ipmb/command/get_device_id.hpp
 * @brief GetDeviceID IPMI command.
 * */

#pragma once
#include <ipmb/command/command.hpp>

#include <vector>

/*! Agent namesapce */
namespace agent {
/*! Chassis namesapce */
namespace chassis {
/*! IPMB namesapce */
namespace ipmb {
/*! Command namesapce */
namespace command {

/*! GetDeviceID command */
class GetDeviceID final : public Command {
public:
    /*! Default constructor */
    GetDeviceID(){}
    /*! Copy constructor */
    GetDeviceID(const GetDeviceID&) = default;
    /*! Assigment construcor */
    GetDeviceID& operator=(const GetDeviceID&) = default;

    /*!
     * Unpacks IPMI Message
     * @param msg IPMI Message
     * */
    void unpack(IpmiMessage& msg);

    /*!
     * Packs IPMI Message
     * @param msg IPMI Message
     * */
    void pack(IpmiMessage& msg);

    ~GetDeviceID();

private:
    static const constexpr uint8_t CMD_RESPONSE_DATA_LENGHT = 16;

    static const constexpr uint8_t OFFSET_CC = 0;
    static const constexpr uint8_t OFFSET_DEV_ID = 1;
    static const constexpr uint8_t OFFSET_DEV_REV = 2;
    static const constexpr uint8_t OFFSET_REV_MAJOR = 3;
    static const constexpr uint8_t OFFSET_REV_MINOR = 4;
    static const constexpr uint8_t OFFSET_IPMI_VER = 5;
    static const constexpr uint8_t OFFSET_DEV_SUPP = 6;
    static const constexpr uint8_t OFFSET_MAN_ID_0 = 7;
    static const constexpr uint8_t OFFSET_MAN_ID_1 = 8;
    static const constexpr uint8_t OFFSET_MAN_ID_2 = 9;

    class Response final : public Command::Response {
        uint8_t m_dev_id{};
        uint8_t m_dev_rev{};
        uint8_t m_rev_major{};
        uint8_t m_rev_minor{};
        uint8_t m_ipmi_ver{};
        uint8_t m_dev_supp{};
        std::vector<uint8_t> m_man_id {std::vector<uint8_t>(3,0)};
        std::vector<uint8_t> m_prod_id {std::vector<uint8_t>(2,0)};
        std::vector<uint8_t> m_aux_fw_rev {std::vector<uint8_t>(4,0)};
    public:
        /*!
         * Fill data buffer
         * @param msg IPMI Message
         * */
        void add_data(IpmiMessage& msg);

        /*!
         * Set Device ID
         * @param dev_id Device ID
         * */
        void set_dev_id(const uint8_t dev_id) {
            m_dev_id = dev_id;
        }

        /*!
         * Set Device Revision
         * @param dev_rev Device Revision
         * */
        void set_dev_rev(const uint8_t dev_rev) {
            m_dev_rev = dev_rev;
        }

        /*!
         * Set Firmware Revison Major
         * @param rev_major Revision Major
         * */
        void set_rev_major(const uint8_t rev_major) {
            m_rev_major = rev_major;
        }

        /*!
         * Set Revison Minor
         * @param rev_minor Revision Minor
         * */
        void set_rev_minor(const uint8_t rev_minor) {
            m_rev_minor = rev_minor;
        }

        /*!
         * Set IPMI Version
         * @param ipmi_ver IPMI Version
         * */
        void set_ipmi_ver(const uint8_t ipmi_ver) {
            m_ipmi_ver = ipmi_ver;
        }

        /*!
         * Set Device Support
         * @param dev_supp Device Support
         * */
        void set_dev_supp(const uint8_t dev_supp) {
            m_dev_supp = dev_supp;
        }

        /*!
         * Set Manufacturer ID
         * @param man_id Manufacturer ID
         * */
        void set_man_id(const std::vector<uint8_t>& man_id) {
            m_man_id = man_id;
        }

        /*!
         * Get Manufacturer ID
         * @return Manufacturer ID
         * */
        const std::vector<uint8_t>& get_man_id() const {
            return m_man_id;
        }

        /*!
         * Set Product ID
         * @param prod_id Product ID
         * */
        void set_prod_id(const std::vector<uint8_t>& prod_id) {
            m_prod_id = prod_id;
        }

        /*!
         * Get Product ID
         * @return Product ID
         * */
        const std::vector<uint8_t>& get_prod_id() const {
            return m_prod_id;
        }

        /*!
         * Set Auxiliary Firmware Revision
         * @param aux_fw_rev Auxiliary Firmware Revision
         * */
        void set_aux_fw_rev(const std::vector<uint8_t>& aux_fw_rev) {
            m_aux_fw_rev = aux_fw_rev;
        }

        /*!
         * Get Auxiliary Firmware Revision
         * @return Auxiliary Firmware Revision
         * */
        const std::vector<uint8_t>& get_aux_fw_rev() const {
            return m_aux_fw_rev;
        }
    };

    Response m_response{};

};

}
}
}
}

