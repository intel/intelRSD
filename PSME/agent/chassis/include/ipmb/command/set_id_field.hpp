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
 * @file ipmb/command/set_id_field.hpp
 * @brief SetIdField IPMI command.
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

/*! SetIdField command */
class SetIdField final : public Command {
public:
    /*! Default constructor */
    SetIdField(){}
    /*! Copy constructor */
    SetIdField(const SetIdField&) = default;
    /*! Assignment constructor */
    SetIdField& operator=(const SetIdField&) = default;

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
    static const constexpr uint8_t CMD_MAX_STRING_FIELD_SIZE = 128;
    static const constexpr uint8_t CMD_MAX_NUMERIC_FIELD_SIZE = sizeof(std::uint32_t);
    static const constexpr uint8_t CMD_TYPE_DEFAULT = 0x00;

    void set_tray_ruid(const uint32_t ruid);
    void set_rack_puid(const uint32_t puid);
    void set_location_id(const std::string& location_id);
    uint32_t decode_id_field(const uint8_t* data, uint8_t len);
    void populate(IpmiMessage& msg);

    class Request final : public Command::Request {
        uint8_t m_type{};
        uint8_t m_instance{};
        uint8_t m_field_size{};
        std::vector<uint8_t> m_field{};
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
         * @param field ID field
         * */
        void set_field(std::vector<uint8_t>&& field) {
            m_field = std::move(field);
        }

        /*!
         * Get id field
         * @return Returns id field
         * */
        const std::vector<uint8_t>& get_field() const {
            return m_field;
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

