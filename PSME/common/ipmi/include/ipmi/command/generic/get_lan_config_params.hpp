/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 *
 * @file get_lan_config_params.hpp
 *
 * @brief IPMI Get LAN configuration parameters command.
 * */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "enums.hpp"



namespace ipmi {
namespace command {
namespace generic {

/*
 * @brief Indicates IP Address Source
 */
enum LanConfigIpAddressSource : std::uint8_t {
    UNSPECIFIED,
    STATIC,
    DHCP,
    BIOS,
    OTHER
};

namespace request {

/*!
 * @brief Base class for IPMI Get LAN configuration parameters command request.
 */
class GetLanConfigParams : public Request {
public:
    /*!
     * @brief Creates GetLanConfigParams request.
     * @param parameter_selector Parameter selector
     */
    GetLanConfigParams(std::uint8_t parameter_selector);


    /*
     * @brief Channel selector.
     * @param channel Channel number.
     */
    void set_channel(std::uint8_t channel);


    /*
     * @brief Specifies if response should return parameter revision only (without data).
     */
    void select_revision_only();


private:
    static constexpr const std::uint8_t DEFAULT_LAN_CHANNEL = 1;
    static constexpr const std::uint8_t GET_REVISION_ONLY_MASK = 0x80;
    static constexpr const std::uint8_t CHANNEL_MASK = 0xf;

    std::uint8_t m_channel_get_revision{DEFAULT_LAN_CHANNEL};
    std::uint8_t m_parameter_selector{0};
    std::uint8_t m_set_selector{0};
    std::uint8_t m_block_selector{0};


    void pack(IpmiInterface::ByteBuffer& data) const override;
};

/*
 * @brief Get LAN IP Address command request.
 */
class GetLanIpAddress : public GetLanConfigParams {
public:
    /*!
     * @brief Default constructor.
     */
    GetLanIpAddress();


    GetLanIpAddress(const GetLanIpAddress&) = default;


    GetLanIpAddress(GetLanIpAddress&&) = default;


    virtual ~GetLanIpAddress();


    const char* get_command_name() const override {
        return "GetLanIpAddress";
    }


private:
    static constexpr const int PARAM_INDEX = 3;
};

/*
 * @brief Get LAN IP Address Source command request.
 */
class GetLanIpAddressSource : public GetLanConfigParams {
public:
    /*!
     * @brief Default constructor.
     */
    GetLanIpAddressSource();


    GetLanIpAddressSource(const GetLanIpAddressSource&) = default;


    GetLanIpAddressSource(GetLanIpAddressSource&&) = default;


    virtual ~GetLanIpAddressSource();


    const char* get_command_name() const override {
        return "GetLanIpAddressSource";
    }


private:
    static constexpr const int PARAM_INDEX = 4;
};

/*
 * @brief Get LAN MAC Address command request.
 */
class GetLanMacAddress : public GetLanConfigParams {
public:

    /*!
     * @brief Default constructor.
     */
    GetLanMacAddress();


    GetLanMacAddress(const GetLanMacAddress&) = default;


    GetLanMacAddress(GetLanMacAddress&&) = default;


    virtual ~GetLanMacAddress();


    const char* get_command_name() const override {
        return "GetLanMacAddress";
    }


private:
    static constexpr const int PARAM_INDEX = 5;
};

/*
 * @brief Get LAN net mask command request.
 */
class GetLanNetMask : public GetLanConfigParams {
public:

    /*!
     * @brief Default constructor.
     */
    GetLanNetMask();


    GetLanNetMask(const GetLanNetMask&) = default;


    GetLanNetMask(GetLanNetMask&&) = default;


    virtual ~GetLanNetMask();


    const char* get_command_name() const override {
        return "GetLanNetMask";
    }


private:
    static constexpr const int PARAM_INDEX = 6;
};

/*
 * @brief Get LAN gateway command request.
 */
class GetLanGateway : public GetLanConfigParams {
public:

    /*!
     * @brief Default constructor.
     */
    GetLanGateway();


    GetLanGateway(const GetLanGateway&) = default;


    GetLanGateway(GetLanGateway&&) = default;


    virtual ~GetLanGateway();


    const char* get_command_name() const override {
        return "GetLanGateway";
    }


private:
    static constexpr const int PARAM_INDEX = 12;
};

}

namespace response {

/*!
 * @brief Base class for IPMI Get LAN configuration parameters command response.
 *
 * */
class GetLanConfigParams : public Response {
public:
    /*!
     * @brief Default constructor.
     * @param response_size Expected response size.
     */
    GetLanConfigParams(const std::size_t response_size);


    GetLanConfigParams(const GetLanConfigParams&) = default;


    GetLanConfigParams(GetLanConfigParams&&) = default;


    virtual ~GetLanConfigParams();


protected:
    static constexpr const std::uint8_t DATA_OFFSET = 2;
    IpmiInterface::ByteBuffer response{};

private:
    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

/*
 * @brief Get LAN IP Address command response.
 */
class GetLanIpAddress : public GetLanConfigParams {
public:
    /*!
     * @brief Default constructor.
     */
    GetLanIpAddress();


    GetLanIpAddress(const GetLanIpAddress&) = default;


    GetLanIpAddress(GetLanIpAddress&&) = default;


    ~GetLanIpAddress();


    const char* get_command_name() const override {
        return "GetLanIpAddress";
    }


    /*!
     * @brief Getter for string representation of IPv4 address.
     * @return IPv4 address as string.
     */
    std::string get_ip_address() const;


private:
    static constexpr const std::size_t RESP_SIZE{6};
};

/*
 * @brief Get LAN IP Address Source command response.
 */
class GetLanIpAddressSource : public GetLanConfigParams {
public:
    /*!
     * @brief Default constructor.
     */
    GetLanIpAddressSource();


    GetLanIpAddressSource(const GetLanIpAddressSource&) = default;


    GetLanIpAddressSource(GetLanIpAddressSource&&) = default;


    virtual ~GetLanIpAddressSource();


    const char* get_command_name() const override {
        return "GetLanIpAddressSource";
    }


    /*!
     * @brief IP address source getter.
     * @return IP address source.
     */
    LanConfigIpAddressSource get_ip_address_source() const;


private:
    static constexpr const std::size_t RESP_SIZE{3};
    static constexpr const std::uint8_t ADDRESS_SOURCE_MASK = 0xf;
};

/*
 * @brief Get LAN MAC Address command response.
 */
class GetLanMacAddress : public GetLanConfigParams {
public:
    /*!
     * @brief Default constructor.
     */
    GetLanMacAddress();


    GetLanMacAddress(const GetLanMacAddress&) = default;


    GetLanMacAddress(GetLanMacAddress&&) = default;


    virtual ~GetLanMacAddress();


    const char* get_command_name() const override {
        return "GetLanMacAddress";
    }


    /*!
     * @brief Getter for string representation of MAC address.
     *
     * example: "50:ab:3a:8e:0c:01"
     *
     * @return MAC address as string.
     */
    std::string get_mac_address() const;


private:
    static constexpr const std::size_t RESP_SIZE{8};
};

/*
 * @brief Get LAN IP net mask command response.
 */
class GetLanNetmask : public GetLanConfigParams {
public:
    /*!
     * @brief Default constructor.
     */
    GetLanNetmask();


    GetLanNetmask(const GetLanNetmask&) = default;


    GetLanNetmask(GetLanNetmask&&) = default;


    virtual ~GetLanNetmask();


    const char* get_command_name() const override {
        return "GetLanNetMask";
    }


    /*!
     * @brief Getter for string representation of netmask.
     *
     * example: "255.255.0.0"
     *
     * @return IPv4 address as string.
     */
    std::string get_netmask() const;


private:
    static constexpr const std::size_t RESP_SIZE{6};
};

/*
 * @brief Get LAN gateway command response.
 */
class GetLanGateway : public GetLanConfigParams {
public:
    /*!
     * @brief Default constructor.
     */
    GetLanGateway();


    GetLanGateway(const GetLanGateway&) = default;


    GetLanGateway(GetLanGateway&&) = default;


    virtual ~GetLanGateway();


    const char* get_command_name() const override {
        return "GetLanGateway";
    }


    /*!
     * @brief Getter for string representation of the gateway.
     *
     * example: "255.255.0.0"
     *
     * @return IPv4 address as string.
     */
    std::string get_gateway() const;


private:
    static constexpr const std::size_t RESP_SIZE{6};
};

using GetLanNetMask = GetLanNetmask;

}

}
}
}
