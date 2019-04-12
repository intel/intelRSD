/*!
 * @brief GetIdField IPMI command request and response.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ipmi/command/sdv/rmm/get_id_field.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * GetIdField command request
 */
class GetIdField : public ipmi::Request {
public:
    /*!
     * @brief Constructors
     */
    GetIdField();


    GetIdField(const GetIdField&) = default;


    GetIdField(GetIdField&&) = default;


    /*!
     * @brief Destructor
     */
    virtual ~GetIdField();


    const char* get_command_name() const override {
        return "GetIdField";
    }


    /*!
     * @brief Set id field type that is to be set
     * @param id_field_type Id field type
     */
    void set_id_field_type(IdFieldType id_field_type) {
        m_id_field_type = id_field_type;
    }


private:
    void pack(IpmiInterface::ByteBuffer& data) const override;


    IdFieldType m_id_field_type{IdFieldType::RESERVED};
};

}

namespace response {

/*!
 * GetIdField command response
 */
class GetIdField : public ipmi::Response {
public:
    /*!
     * @brief Constructor
     */
    GetIdField();
    GetIdField(const GetIdField&) = default;
    GetIdField(GetIdField&&) = default;


    /*!
     * @brief Destructor
     */
    virtual ~GetIdField();


    const char* get_command_name() const override {
        return "GetIdField";
    }


    /*!
     * @brief Get maximum data size that can be sent
     * @return Maximum data size that can be sent
     */
    std::uint8_t get_maximum_data_size() const {
        return m_maximum_data_size;
    }


    /*!
     * @brief Get received data size
     * @return Received data size
     */
    std::uint8_t get_received_data_size() const {
        return m_received_data_size;
    }


    /*!
     * @brief Get received data
     * @return Received data
     */
    const std::vector <std::uint8_t>& get_received_data() const {
        return m_received_data;
    }


private:
    static constexpr std::size_t MINIMUM_RESPONSE_SIZE = 2;
    static constexpr size_t MAXIMUM_DATA_SIZE_OFFSET = 1;
    static constexpr size_t RECEIVED_DATA_SIZE_OFFSET = 2;
    static constexpr size_t RECEIVED_DATA_OFFSET = 3;


    void unpack(const IpmiInterface::ByteBuffer& byte_buffer) override;


    std::uint8_t m_maximum_data_size{};
    std::uint8_t m_received_data_size{};
    std::vector <std::uint8_t> m_received_data{};
};

}

}
}
}
