/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file ipmb/command/get_id_field.hpp
 * @brief GetIdField IPMI command.
 * */

#ifndef AGENT_CHASSIS_IPMB_COMMAND_GET_ID_FIELD_HPP
#define AGENT_CHASSIS_IPMB_COMMAND_GET_ID_FIELD_HPP

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

/*! GetIDField command */
class GetIDField final : public Command {
public:
    /*! Default constructor */
    GetIDField(){}
    /*! Copy constructor */
    GetIDField(const GetIDField&) = default;
    /*! Assigment construcor */
    GetIDField& operator=(const GetIDField&) = default;

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

private:
    static const constexpr uint8_t CMD_REQUEST_DATA_LENGTH = 1;
    static const constexpr uint8_t CMD_MAX_FIELD_SIZE = 4;
    static const constexpr uint8_t CMD_CURR_FIELD_SIZE = 4;
    static const constexpr uint8_t CMD_RESPONSE_DATA_LENGTH = 3;

    static const constexpr uint8_t CMD_TYPE_DEFAULT = 0x00;

    static const constexpr uint8_t OFFSET_CC = 0;
    static const constexpr uint8_t OFFSET_MAX_FIELD_SIZE = 1;
    static const constexpr uint8_t OFFSET_CURR_FIELD_SIZE = 2;
    static const constexpr uint8_t OFFSET_ID_FIELD_0 = 3;
    static const constexpr uint8_t OFFSET_ID_FIELD_1 = 4;
    static const constexpr uint8_t OFFSET_ID_FIELD_2 = 5;
    static const constexpr uint8_t OFFSET_ID_FIELD_3 = 6;

    uint32_t get_tray_ruid();
    uint32_t get_rack_puid();
    void populate(IpmiMessage& msg);

    class Request final : public Command::Request {
        uint8_t m_instance;
    public:
        /*!
         * Set field instance
         * @param instance Field instance
         * */
        void set_instance(const uint8_t instance) {
            m_instance = instance;
        }

        /*!
         * Get filed instance
         * @return field instance
         * */
        uint8_t get_instance() const {
            return m_instance;
        }
    };

    class Response final : public Command::Response {
        uint8_t m_max_field_size{4};
        uint8_t m_curr_field_size{4};
        uint32_t m_id_field{};
    public:
        /*!
         * Fill data buffer
         * @param msg IPMI Message
         * */
        void add_data(IpmiMessage& msg);

        /*!
         * Set max field size.
         * @param size Max field size
         * */
        void set_max_field_size(const uint8_t size) {
            m_max_field_size = size;
        }

        /*!
         * Get max field size
         * @return Returns max field size */
        uint8_t get_max_field_size() {
            return m_max_field_size;
        }

        /*!
         * Set curr field size.
         * @param size Curr field size
         * */
        void set_curr_field_size(const uint8_t size) {
            m_curr_field_size = size;
        }

        /*!
         * Get curr field size
         * @return Returns curr field size
         * */
        uint8_t get_curr_field_size() {
            return m_curr_field_size;
        }

        /*!
         * Set ID Field
         * @param id Id Field
         * */
        void set_id_field(const uint32_t id) {
            m_id_field = id;
        }

        /*!
         * Get ID Filed
         * @returns ID Field
         * */
        uint32_t get_id_field() const {
            return m_id_field;
        }
    };

    Response m_response{};
    Request m_request{};

};

}
}
}
}

#endif /* AGENT_CHASSIS_IPMB_COMMAND_GET_ID_FIELD_HPP */
