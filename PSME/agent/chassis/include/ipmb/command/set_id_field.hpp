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
 * @file ipmb/command/set_id_field.hpp
 * @brief SetIdField IPMI command.
 * */

#ifndef AGENT_CHASSIS_IPMB_COMMAND_SET_ID_FIELD_HPP
#define AGENT_CHASSIS_IPMB_COMMAND_SET_ID_FIELD_HPP

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

/*! SetIDField command */
class SetIDField final : public Command {
public:
    /*! Default constructor */
    SetIDField(){}
    /*! Copy constructor */
    SetIDField(const SetIDField&) = default;
    /*! Assigment construcor */
    SetIDField& operator=(const SetIDField&) = default;

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
    static const constexpr uint8_t CMD_MIN_REQUEST_DATA_LENGTH = 4;
    static const constexpr uint8_t CMD_MAX_REQUEST_LENGTH = 7;
    static const constexpr uint8_t CMD_MAX_FIELD_SIZE = 4;
    static const constexpr uint8_t CMD_RESPONSE_DATA_LENGTH = 2;

    static const constexpr uint8_t CMD_TYPE_DEFAULT = 0x00;

    static const constexpr uint8_t OFFSET_CC = 0;
    static const constexpr uint8_t OFFSET_MAX_FIELD_SIZE = 1;

    void set_tray_ruid(const uint32_t ruid);
    void set_rack_puid(const uint32_t puid);
    uint32_t decode_id_field(uint8_t* data, uint8_t len);
    void populate(IpmiMessage& msg);

    class Request final : public Command::Request {
        uint8_t m_type{};
        uint8_t m_instance{};
        uint8_t m_field_size{};
        uint32_t m_id_field{};
    public:
        /*!
         * Set field type
         * @param type field type
         * */
        void set_type(const uint8_t type) {
            m_type = type;
        }

        /*!
         * Get field type
         * @return Returns field type
         * */
        uint8_t get_type() {
            return m_type;
        }

        /*!
         * Set field instance
         * @param instance Field instance
         * */
        void set_instance(const uint8_t instance) {
            m_instance = instance;
        }

        /*!
         * Get field type
         * @return Returns field type
         * */
        uint8_t get_instance() {
            return m_instance;
        }

        /*!
         * Set field size.
         * @param size field size
         * */
        void set_field_size(const uint8_t size) {
            m_field_size = size;
        }

        /*!
         * Get field size
         * @return Returns field size */
        uint8_t get_field_size() {
            return m_field_size;
        }

        /*!
         * Set ID field
         * @param id ID field
         * */
        void set_id_field(const uint32_t id) {
            m_id_field = id;
        }

        /*!
         * Get id field
         * @return Returns id field
         * */
        uint32_t get_id_field() {
            return m_id_field;
        }

    };

    class Response final : public Command::Response {
        uint8_t m_max_field_size{};
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
    };

    Request m_request{};
    Response m_response{};

};

}
}
}
}

#endif /* AGENT_CHASSIS_IPMB_COMMAND_SET_ID_FIELD_HPP */
