/*!
 * @brief GetTpmConfiguration command interface.
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
 * @file get_tpm_configuration.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"



namespace ipmi {
namespace command {
namespace sdv {
namespace rsd {

namespace request {

/*!
 * @brief GetTpmConfiguration request.
 */
class GetTpmConfiguration : public Request {
public:
    GetTpmConfiguration();


    GetTpmConfiguration(const GetTpmConfiguration&) = default;


    GetTpmConfiguration(GetTpmConfiguration&&) = default;


    GetTpmConfiguration& operator=(const GetTpmConfiguration&) = default;


    GetTpmConfiguration& operator=(GetTpmConfiguration&&) = default;


    virtual ~GetTpmConfiguration();


    virtual const char* get_command_name() const override {
        return "GetTpmConfiguration";
    }


private:
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief GetTpmConfiguration response.
 */
class GetTpmConfiguration : public Response {
public:
    GetTpmConfiguration();


    GetTpmConfiguration(const GetTpmConfiguration&) = default;


    GetTpmConfiguration(GetTpmConfiguration&&) = default;


    GetTpmConfiguration& operator=(const GetTpmConfiguration&) = default;


    GetTpmConfiguration& operator=(GetTpmConfiguration&&) = default;


    virtual ~GetTpmConfiguration();


    virtual const char* get_command_name() const override {
        return "GetTpmConfiguration";
    }


    /*!
     * @brief Check if RackScale extension byte is present in the response.
     * @return True if extension byte is present, false otherwise.
     */
    bool is_rackscale_extension_byte_present() const {
        return m_is_rackscale_extension_byte_present;
    }


    /*!
     * @brief Get TPM command status.
     * @return TPM command status.
     */
    TpmCommandStatus get_tpm_command_status() const {
        return m_tpm_command_status;
    }


    /*!
     * @brief Get TPM version selection.
     * @return 0x00 if TPM is disabled or TPM version selection larger or equal than 0x01 otherwise.
     */
    std::uint8_t get_tpm_version_selection() const {
        return m_tpm_version_selection;
    }


    /*!
     * @brief Get clear TPM action mode.
     * @return Clear TPM action mode.
     */
    ClearTpmAction get_clear_tpm_action() const {
        return m_clear_tpm_action;
    }


private:
    virtual void unpack(const IpmiInterface::ByteBuffer& data) override;


    static constexpr size_t RESPONSE_SIZE = 5;
    static constexpr size_t TPM_COMMAND_STATUS_OFFSET = 2;
    static constexpr size_t TPM_VERSION_SELECTION_OFFSET = 3;
    static constexpr size_t CLEAR_TPM_ACTION_OFFSET = 4;

    bool m_is_rackscale_extension_byte_present{false};
    TpmCommandStatus m_tpm_command_status{TpmCommandStatus::COMMAND_INVALID};
    std::uint8_t m_tpm_version_selection{};
    ClearTpmAction m_clear_tpm_action{ClearTpmAction::PRESERVE_USER_TPM_OWNERSHIP};
};

}

}
}
}
}
