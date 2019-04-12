/*!
 * @brief Get OOB Target Boot Options command declaration.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
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
 * @file get_oob_target_boot_options.hpp
 */

#pragma once



#include "get_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI GetOOBTarget Boot Options.
 */
class GetOobTargetBootOptions : public GetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    GetOobTargetBootOptions(uint8_t index = 0);


    /*! Copy constructor */
    GetOobTargetBootOptions(const GetOobTargetBootOptions&) = default;


    /*! Assignment operator */
    GetOobTargetBootOptions& operator=(const GetOobTargetBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobTargetBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const override;

    virtual const char* get_command_name() const override {
        return "GetOobTargetBootOptions";
    }
};

}

namespace response {

/*!
 * @brief Response message for IPMI GetOOBTarget Boot option.
 */
class GetOobTargetBootOptions : public GetOobBootOptions {
public:

    enum class ChapType : uint8_t {
        None = 0,
        OneWayChap = 1,
        MutualChap = 2
    };


    /*!
     * @brief Default constructor.
     */
    GetOobTargetBootOptions();


    /*! Copy constructor */
    GetOobTargetBootOptions(const GetOobTargetBootOptions&) = default;


    /*! Assignment operator */
    GetOobTargetBootOptions& operator=(const GetOobTargetBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobTargetBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) override;


    uint16_t get_target_name_offset() const { return m_target_name_offset; }


    uint16_t get_target_name_length() const { return m_target_name_length; }


    bool get_dhcp_enabled() const { return m_dhcp_enabled; }


    const std::string& get_address() const { return m_address; }


    uint16_t get_port() const { return m_port; }


    uint64_t get_lun() const { return m_lun; }


    ChapType get_chap_type() const { return m_chap_type; }


    uint16_t get_chapname_offset() const { return m_chapname_offset; }


    uint16_t get_chapname_length() const { return m_chapname_length; }


    uint16_t get_chapsecret_offset() const { return m_chapsecret_offset; }


    uint16_t get_chapsecret_length() const { return m_chapsecret_length; }


    uint16_t get_mutual_chapname_offset() const { return m_mutual_chapname_offset; }


    uint16_t get_mutual_chapname_length() const { return m_mutual_chapname_length; }


    uint16_t get_mutual_chapsecret_offset() const { return m_mutual_chapsecret_offset; }


    uint16_t get_mutual_chapsecret_length() const { return m_mutual_chapsecret_length; }


    size_t v2_payload_size() override { return V2_RESPONSE_SIZE; }


    std::string what() override { return "Target"; }

    virtual const char* get_command_name() const override {
        return "GetOobTargetBootOptions";
    }

private:

    void set_target_name_offset(uint16_t target_name_offset) { m_target_name_offset = target_name_offset; }


    void set_target_name_length(uint16_t target_name_length) { m_target_name_length = target_name_length; }


    void set_dhcp_enabled(bool enabled) { m_dhcp_enabled = enabled; }


    void set_address(const std::string& address) { m_address = address; }


    void set_port(uint16_t port) { m_port = port; }


    void set_lun(uint64_t new_lun) { m_lun = new_lun; }


    void set_chap_type(ChapType chap_type) { m_chap_type = chap_type; }


    void set_chapname_offset(uint16_t chapname_offset) { m_chapname_offset = chapname_offset; }


    void set_chapname_length(uint16_t chapname_length) { m_chapname_length = chapname_length; }


    void set_chapsecret_offset(uint16_t chapsecret_offset) { m_chapsecret_offset = chapsecret_offset; }


    void set_chapsecret_length(uint16_t chapsecret_length) { m_chapsecret_length = chapsecret_length; }


    void
    set_mutual_chapname_offset(uint16_t mutual_chapname_offset) { m_mutual_chapname_offset = mutual_chapname_offset; }


    void
    set_mutual_chapname_length(uint16_t mutual_chapname_length) { m_mutual_chapname_length = mutual_chapname_length; }


    void set_mutual_chapsecret_offset(
        uint16_t mutual_chapsecret_offset) { m_mutual_chapsecret_offset = mutual_chapsecret_offset; }


    void set_mutual_chapsecret_length(
        uint16_t mutual_chapsecret_length) { m_mutual_chapsecret_length = mutual_chapsecret_length; }


    uint16_t m_target_name_offset{};
    uint16_t m_target_name_length{};
    bool m_dhcp_enabled{};
    std::string m_address{};
    uint16_t m_port{};
    uint64_t m_lun{};
    ChapType m_chap_type{};
    uint16_t m_chapname_offset{};
    uint16_t m_chapname_length{};
    uint16_t m_chapsecret_offset{};
    uint16_t m_chapsecret_length{};
    uint16_t m_mutual_chapname_offset{};
    uint16_t m_mutual_chapname_length{};
    uint16_t m_mutual_chapsecret_offset{};
    uint16_t m_mutual_chapsecret_length{};

    static constexpr size_t OFFSET_TARGET_NAME = OFFSET_INDEX + 1;
    static constexpr size_t OFFSET_TARGET_NAME_LEN = OFFSET_TARGET_NAME + 2;
    static constexpr size_t OFFSET_DHCP_ENABLED = OFFSET_TARGET_NAME_LEN + 2;
    static constexpr size_t OFFSET_IP_ADDRESS = OFFSET_DHCP_ENABLED + 1;
    static constexpr size_t OFFSET_PORT = OFFSET_IP_ADDRESS + IP_BYTES;
    static constexpr size_t OFFSET_LUN = OFFSET_PORT + 2;
    static constexpr size_t OFFSET_CHAP_TYPE = OFFSET_LUN + 8;
    static constexpr size_t OFFSET_CHAPNAME = OFFSET_CHAP_TYPE + 1;
    static constexpr size_t OFFSET_CHAPNAME_LEN = OFFSET_CHAPNAME + 2;
    static constexpr size_t OFFSET_CHAPSECRET = OFFSET_CHAPNAME_LEN + 2;
    static constexpr size_t OFFSET_CHAPSECRET_LEN = OFFSET_CHAPSECRET + 2;
    static constexpr size_t OFFSET_MUTUAL_CHAPNAME = OFFSET_CHAPSECRET_LEN + 2;
    static constexpr size_t OFFSET_MUTUAL_CHAPNAME_LEN = OFFSET_MUTUAL_CHAPNAME + 2;
    static constexpr size_t OFFSET_MUTUAL_CHAPSECRET = OFFSET_MUTUAL_CHAPNAME_LEN + 2;
    static constexpr size_t OFFSET_MUTUAL_CHAPSECRET_LEN = OFFSET_MUTUAL_CHAPSECRET + 2;
    static constexpr size_t V2_RESPONSE_SIZE = OFFSET_MUTUAL_CHAPSECRET_LEN + 2;
};

}

}
}
}
