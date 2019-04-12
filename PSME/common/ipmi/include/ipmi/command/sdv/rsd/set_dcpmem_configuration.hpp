/*!
 * @brief SetDcpmemConfiguration command interface.
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
 * @file set_dcpmem_configuration.hpp
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
 * @brief Set Intel Optane DC Persistent Memory Configuration request.
 */
class SetDcpmemConfiguration : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    SetDcpmemConfiguration();


    /*! Copy constructor. */
    SetDcpmemConfiguration(const SetDcpmemConfiguration&) = default;


    /*! Assignment operator */
    SetDcpmemConfiguration& operator=(const SetDcpmemConfiguration&) = default;


    /*!
     * @brief Default destructor.
     */
    ~SetDcpmemConfiguration();


    /*!
     * @brief Set command status.
     * @param Command status.
     */
    void set_command_status(DcpmemConfigurationCommandStatus command_status) {
        m_command_status = command_status;
    }


    /*!
     * @brief Set erase DCPMEM action.
     * @param Erase DCPMEM action.
     */
    void set_erase_dcpmem_action(EraseDcpmemAction erase_dcpmem_action) {
        m_erase_dcpmem_action = erase_dcpmem_action;
    }


    virtual const char* get_command_name() const override {
        return "SetDcpmemConfiguration";
    }


private:
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;


    DcpmemConfigurationCommandStatus m_command_status{};
    EraseDcpmemAction m_erase_dcpmem_action{};
};

}

namespace response {
/*!
 * @brief Set Intel Optane DC Persistent Memory Configuration request.
 */
class SetDcpmemConfiguration : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    SetDcpmemConfiguration();


    /*! Copy constructor. */
    SetDcpmemConfiguration(const SetDcpmemConfiguration&) = default;


    /*! Assignment operator */
    SetDcpmemConfiguration& operator=(const SetDcpmemConfiguration&) = default;


    /*!
     * @brief Default destructor.
     */
    ~SetDcpmemConfiguration();


    /*!
     * @brief Gets command status
     * @return Command status
     */
    DcpmemConfigurationCommandStatus get_command_status() const {
        return m_command_status;
    }


    /*!
     * @brief Gets erase DCPMEM action
     * @return Erase DCPMEM action
     */
    EraseDcpmemAction get_erase_dcpmem_action() const {
        return m_erase_dcpmem_action;
    }


    /*!
     * @brief Check if RackScale extension byte is present in the response.
     * @return True if extension byte is present, false otherwise.
     */
    bool is_rackscale_extension_byte_present() const {
        return m_is_rackscale_extension_byte_present;
    }


    virtual const char* get_command_name() const override {
        return "SetDcpmemConfiguration";
    }


private:

    static constexpr size_t RESPONSE_SIZE = 4;
    static constexpr const uint8_t COMMAND_STATUS_OFFSET = 2;
    static constexpr const uint8_t ERASE_DCPMEM_ACTION_OFFSET = 3;

    DcpmemConfigurationCommandStatus m_command_status{};
    EraseDcpmemAction m_erase_dcpmem_action{};

    bool m_is_rackscale_extension_byte_present{false};


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}
}
