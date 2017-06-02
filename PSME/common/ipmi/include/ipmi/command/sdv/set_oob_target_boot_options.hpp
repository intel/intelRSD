/*!
 * @brief Set OOB Target Boot Options command declaration.
 *
 * @copyright Copyright (c) 2016-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Files}
 * @file set_oob_target_boot_options.hpp
 */

#pragma once



#include "ipmi/command/sdv/set_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI SetOOBTarget Boot Options.
 */
class SetOobTargetBootOptions : public SetOobBootOptions {
public:

    enum class ChapType : uint8_t {
        None = 0,
        OneWayChap = 1,
        MutualChap = 2
    };


    /*!
     * @brief Default constructor.
     */
    SetOobTargetBootOptions();


    /*! Copy constructor */
    SetOobTargetBootOptions(const SetOobTargetBootOptions&) = default;


    /*! Assginment operator */
    SetOobTargetBootOptions& operator=(const SetOobTargetBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobTargetBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const;


    void set_dhcp_enabled(bool enabled) { m_dhcp_enabled = enabled; }


    void set_address(const std::string& address) { m_ip_address = address; }


    void set_name_offset(uint16_t offset) { m_name_offset = offset; }


    void set_name_length(uint16_t length) { m_name_length = length; }


    void set_port(uint16_t port) { m_port = port; }


    void set_lun(uint64_t lun) { m_lun = lun; }


    void set_chap_type(ChapType type) { m_chap_type = type; }


    void set_chap_name_offset(uint16_t offset) { m_chap_name_offset = offset; }


    void set_chap_name_length(uint16_t length) { m_chap_name_length = length; }


    void set_chap_secret_offset(uint16_t offset) { m_chap_secret_offset = offset; }


    void set_chap_secret_length(uint16_t length) { m_chap_secret_length = length; }


    void set_mutual_chap_name_offset(uint16_t offset) { m_mutual_chap_name_offset = offset; }


    void set_mutual_chap_name_length(uint16_t length) { m_mutual_chap_name_length = length; }


    void set_mutual_chap_secret_offset(uint16_t offset) { m_mutual_chap_secret_offset = offset; }


    void set_mutual_chap_secret_length(uint16_t length) { m_mutual_chap_secret_length = length; }


private:

    static constexpr std::uint8_t TARGET_PARAMETER_INDEX = 0x00;

    uint16_t m_name_offset{0};
    uint16_t m_name_length{0};
    bool m_dhcp_enabled{false};
    std::string m_ip_address{NULL_ADDRESS};
    uint16_t m_port{0};
    uint64_t m_lun{0};
    ChapType m_chap_type{ChapType::None};
    uint16_t m_chap_name_offset{0};
    uint16_t m_chap_name_length{0};
    uint16_t m_chap_secret_offset{0};
    uint16_t m_chap_secret_length{0};
    uint16_t m_mutual_chap_name_offset{0};
    uint16_t m_mutual_chap_name_length{0};
    uint16_t m_mutual_chap_secret_offset{0};
    uint16_t m_mutual_chap_secret_length{0};
};

}

namespace response {

/*!
 * @brief Response message for IPMI SetOOBTarget Boot option.
 */
class SetOobTargetBootOptions : public SetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobTargetBootOptions();


    /*! Copy constructor */
    SetOobTargetBootOptions(const SetOobTargetBootOptions&) = default;


    /*! Assginment operator */
    SetOobTargetBootOptions& operator=(const SetOobTargetBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobTargetBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data);


private:
    static constexpr size_t RESPONSE_SIZE = 1;
};

}

}
}
}
