/*!
 * @brief GetCpuPeciPackageConfigData command interface.
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
 * @file get_cpu_peci_package_config_data.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <vector>



namespace ipmi {
namespace command {
namespace sdv {
namespace rsd {

namespace request {

/*!
 * @brief GetCpuPeciPackageConfigData request.
 */
class GetCpuPeciPackageConfigData : public Request {
public:
    GetCpuPeciPackageConfigData();


    GetCpuPeciPackageConfigData(const GetCpuPeciPackageConfigData&) = default;


    GetCpuPeciPackageConfigData(GetCpuPeciPackageConfigData&&) = default;


    GetCpuPeciPackageConfigData& operator=(const GetCpuPeciPackageConfigData&) = default;


    GetCpuPeciPackageConfigData& operator=(GetCpuPeciPackageConfigData&&) = default;


    virtual ~GetCpuPeciPackageConfigData();


    virtual const char* get_command_name() const override {
        return "GetCpuPeciPackageConfigData";
    }


    /*!
     * @brief Set node index.
     * @param[in] node_index Zero-based node index
     */
    void set_node_index(std::uint8_t node_index) {
        m_node_index = node_index;
    }


    /*!
     * @brief Set CPU index.
     * @param cpu_index Zero-based CPU index.
     */
    void set_cpu_index(std::uint8_t cpu_index) {
        m_cpu_index = cpu_index;
    }


    /*!
     * @brief Set PCS command index.
     * @param pcs_command_index PCS command index.
     */
    void set_pcs_command_index(std::uint8_t pcs_command_index) {
        m_pcs_command_index = pcs_command_index;
    }


    /*!
     * @brief Set PCS command parameter.
     * @param pcs_command_parameter PCS command parameter.
     */
    void set_pcs_command_parameter(const std::vector <std::uint8_t>& pcs_command_parameter);


private:
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;


    std::uint8_t m_node_index{};
    std::uint8_t m_cpu_index{};
    std::uint8_t m_pcs_command_index{};
    std::vector <std::uint8_t> m_pcs_command_parameter{0, 0, 0, 0};
};

}

namespace response {

/*!
 * @brief GetCpuPeciPackageConfigData response.
 */
class GetCpuPeciPackageConfigData : public Response {
public:
    GetCpuPeciPackageConfigData();


    GetCpuPeciPackageConfigData(const GetCpuPeciPackageConfigData&) = default;


    GetCpuPeciPackageConfigData(GetCpuPeciPackageConfigData&&) = default;


    GetCpuPeciPackageConfigData& operator=(const GetCpuPeciPackageConfigData&) = default;


    GetCpuPeciPackageConfigData& operator=(GetCpuPeciPackageConfigData&&) = default;


    virtual ~GetCpuPeciPackageConfigData();


    virtual const char* get_command_name() const override {
        return "GetCpuPeciPackageConfigData";
    }


    /*!
     * @brief Check if RackScale extension byte is present in the response.
     * @return True if extension byte is present, false otherwise.
     */
    bool is_rackscale_extension_byte_present() const {
        return m_is_rackscale_extension_byte_present;
    }


    /*!
     * @brief Get node index.
     * @return Zero-based node index.
     */
    std::uint8_t get_node_index() const {
        return m_node_index;
    }


    /*!
     * @brief Get CPU index.
     * @return Zero-based CPU index.
     */
    std::uint8_t get_cpu_index() const {
        return m_cpu_index;
    }


    /*!
     * @brief Get PCS PECI data.
     * @return PCS PECI data.
     */
    const std::vector <std::uint8_t>& get_pcs_peci_data() const {
        return m_pcs_peci_data;
    }


private:
    virtual void unpack(const IpmiInterface::ByteBuffer& data) override;


    static constexpr size_t RESPONSE_SIZE = 9;
    static constexpr size_t NODE_INDEX_OFFSET = 2;
    static constexpr size_t CPU_INDEX_OFFSET = 3;
    static constexpr size_t PSC_PECI_DATA_OFFSET = 5;

    std::uint8_t m_node_index{};
    std::uint8_t m_cpu_index{};
    std::vector <std::uint8_t> m_pcs_peci_data{};
    bool m_is_rackscale_extension_byte_present{false};
};

}

}
}
}
}
