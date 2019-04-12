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
 * @file ipmb/command/get_id_field.hpp
 * @brief GetIdField IPMI command.
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

/*! GetIdField command */
class GetIdField final : public Command {
public:
    /*! Default constructor */
    GetIdField(){}
    /*! Copy constructor */
    GetIdField(const GetIdField&) = default;
    /*! Assigment construcor */
    GetIdField& operator=(const GetIdField&) = default;

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
    static const constexpr uint8_t CMD_MAX_STRING_FIELD_SIZE = 128;
    static const constexpr uint8_t CMD_MAX_NUMERIC_FIELD_SIZE = sizeof(std::uint32_t);
    static const constexpr uint8_t CMD_TYPE_DEFAULT = 0x00;

    std::vector<uint8_t> get_tray_ruid() const;
    std::vector<uint8_t> get_rack_puid() const;
    std::vector<uint8_t> get_rack_location_id() const;
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
        uint8_t m_max_field_size{};
        std::vector<uint8_t> m_field{};
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
         * Get curr field size
         * @return Returns curr field size
         * */
        uint8_t get_curr_field_size() const {
            return uint8_t(m_field.size());
        }

        /*!
         * Set Field
         * @param field Field
         * */
        void set_field(std::vector<uint8_t>&& field) {
            m_field = std::move(field);
        }

        /*!
         * Get ID Filed
         * @returns ID Field
         * */
        const std::vector<uint8_t>& get_field() const {
            return m_field;
        }
    };

    Response m_response{};
    Request m_request{};

};

}
}
}
}

