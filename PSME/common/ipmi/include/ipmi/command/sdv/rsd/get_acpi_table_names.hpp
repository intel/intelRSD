/*!
 * @brief GetAcpiTableNames command interface.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file get_acpi_table_names.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace sdv {
namespace rsd {

namespace request {

/*!
 * @brief Get ACPI Table Names request.
 */
class GetAcpiTableNames : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetAcpiTableNames();

    /*! Copy constructor. */
    GetAcpiTableNames(const GetAcpiTableNames&) = default;

    /*! Assignment operator */
    GetAcpiTableNames& operator=(const GetAcpiTableNames&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetAcpiTableNames();

    virtual const char* get_command_name() const override {
        return "GetAcpiTableNames";
    }

private:
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Get ACPI Table Names response.
 */
class GetAcpiTableNames : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    GetAcpiTableNames();

    /*! Copy constructor. */
    GetAcpiTableNames(const GetAcpiTableNames&) = default;

    /*! Assignment operator */
    GetAcpiTableNames& operator=(const GetAcpiTableNames&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetAcpiTableNames();

    /*!
    * @brief Gets data length
    * @return Data length
    */
    std::uint8_t get_data_length() const {
        return m_data_length;
    }

    /*!
     * @brief Gets ACPI table names
     * @return ACPI table names
     */
    const std::vector<std::string>& get_acpi_table_names() const {
        return m_acpi_table_names;
    }

    /*!
     * @brief Check if RackScale extension byte is present in the response.
     * @return True if extension byte is present, false otherwise.
     */
    bool is_rackscale_extension_byte_present() const {
        return m_is_rackscale_extension_byte_present;
    }

    virtual const char* get_command_name() const override {
        return "GetAcpiTableNames";
    }

private:

    static constexpr size_t RESPONSE_MINIMUM_SIZE = 4;
    static constexpr const uint8_t DATA_LENGTH_OFFSET = 2;
    static constexpr const uint8_t ACPI_TABLE_NAMES_OFFSET = 3;

    std::uint8_t m_data_length{};
    std::vector<std::string> m_acpi_table_names{};

    bool m_is_rackscale_extension_byte_present{false};

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}
}
