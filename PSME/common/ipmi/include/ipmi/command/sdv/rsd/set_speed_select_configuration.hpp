/*!
 * @brief SetSpeedSelectConfiguration command interface.
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
 * @file set_speed_select_configuration.hpp
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
 * @brief Set Speed Select Configuration request.
 */
class SetSpeedSelectConfiguration : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    SetSpeedSelectConfiguration();

    /*! Copy constructor. */
    SetSpeedSelectConfiguration(const SetSpeedSelectConfiguration&) = default;

    /*! Assignment operator */
    SetSpeedSelectConfiguration& operator=(const SetSpeedSelectConfiguration&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~SetSpeedSelectConfiguration();

    virtual const char* get_command_name() const override {
        return "SetSpeedSelectConfiguration";
    }

    /*!
     * @brief Set 0 based node index.
     * @param node index.
     */
    void set_node_index(std::uint8_t node_index) {
        m_node_index = node_index;
    }

    /*!
     * @brief Set 0 based CPU index.
     * @param node index.
     */
    void set_cpu_index(std::uint8_t cpu_index) {
        m_cpu_index = cpu_index;
    }

    /*!
     * @brief Set new configuration index.
     * @param node index.
     */
    void set_configuration_index(std::uint8_t configuration_index) {
        m_configuration_index = configuration_index;
    }

private:
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;

    std::uint8_t m_node_index{};
    std::uint8_t m_cpu_index{};
    std::uint8_t m_configuration_index{};
};

}

namespace response {

/*!
 * @brief Set Speed Select Configuration response.
 */
class SetSpeedSelectConfiguration : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    SetSpeedSelectConfiguration();

    /*! Copy constructor. */
    SetSpeedSelectConfiguration(const SetSpeedSelectConfiguration&) = default;

    /*! Assignment operator */
    SetSpeedSelectConfiguration& operator=(const SetSpeedSelectConfiguration&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~SetSpeedSelectConfiguration();

    /*!
    * @brief Gets 0 based Node index.
    * @return Node index.
    */
    std::uint8_t get_node_index() const {
        return m_node_index;
    }

    /*!
    * @brief Gets 0 based CPU index.
    * @return CPU index.
    */
    std::uint8_t get_cpu_index() const {
        return m_cpu_index;
    }

    /*!
    * @brief Gets Active or Requested Configuration Index.
    * @return Configuration Index.
    */
    std::uint8_t get_configuration_index() const {
        return m_configuration_index;
    }

    /*!
     * @brief Check if RackScale extension byte is present in the response.
     * @return True if extension byte is present, false otherwise.
     */
    bool is_rackscale_extension_byte_present() const {
        return m_is_rackscale_extension_byte_present;
    }

    virtual const char* get_command_name() const override {
        return "SetSpeedSelectConfiguration";
    }

private:

    static constexpr const uint8_t NODE_INDEX_OFFSET = 2;
    static constexpr const uint8_t CPU_INDEX_OFFSET = 3;
    static constexpr const uint8_t CONFIGURATION_INDEX_OFFSET = 4;
    static constexpr size_t RESPONSE_SIZE = 5;

    std::uint8_t m_node_index{};
    std::uint8_t m_cpu_index{};
    std::uint8_t m_configuration_index{};

    bool m_is_rackscale_extension_byte_present{false};

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}
}
