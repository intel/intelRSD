/*!
 * @brief SetAcpiTableNames command interface.
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
 * @file set_acpi_table_names.hpp
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
 * @brief Set ACPI Table Names request.
 */
class SetAcpiTableNames : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    SetAcpiTableNames();

    /*! Copy constructor. */
    SetAcpiTableNames(const SetAcpiTableNames&) = default;

    /*! Assignment operator */
    SetAcpiTableNames& operator=(const SetAcpiTableNames&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~SetAcpiTableNames();

    virtual const char* get_command_name() const override {
        return "SetAcpiTableNames";
    }

    /*!
     * @brief Set reserved byte.
     * @param reserved byte.
     */
    void set_reserved(std::uint8_t reserved) {
        m_reserved = reserved;
    }

    /*!
     * @brief Set data length byte.
     * @param data length byte.
     */
    void set_data_length(std::uint8_t data_length) {
        m_data_length = data_length;
    }

    /*!
     * @brief Set APCI table names.
     * @param APCI table names.
     */
    void set_acpi_table_names(std::vector<std::string> acpi_table_names) {
        for (const auto& name : acpi_table_names) {
            if (name.size() != 4) {
                 throw std::logic_error("Incorrect table name size.");
            }
        }
        m_acpi_table_names = acpi_table_names;
    }

private:
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;

    std::uint8_t m_data_length{};
    std::uint8_t m_reserved{};
    std::vector<std::string> m_acpi_table_names{};
};

}

namespace response {

/*!
 * @brief Set ACPI Table Names response.
 */
class SetAcpiTableNames : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    SetAcpiTableNames();

    /*! Copy constructor. */
    SetAcpiTableNames(const SetAcpiTableNames&) = default;

    /*! Assignment operator */
    SetAcpiTableNames& operator=(const SetAcpiTableNames&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~SetAcpiTableNames();

    /*!
    * @brief Gets data length
    * @return Data length
    */
    const std::uint8_t& get_data_length() const {
        return m_data_length;
    }

    /*!
     * @brief Gets ACPI table names
     * @return ACPI table names
     */
    std::vector<std::string> get_acpi_table_names() const {
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
        return "SetAcpiTableNames";
    }


private:

    static constexpr size_t RESPONSE_MINIMUM_SIZE = 7;
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

