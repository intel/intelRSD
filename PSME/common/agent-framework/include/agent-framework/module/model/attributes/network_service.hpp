 /*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file network_service.hpp
 *
 *
 * @brief network_service
 * */

#pragma once
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

class NetworkService {
public:

    explicit NetworkService();
    NetworkService(const enums::NetworkServiceName& name,
                   const std::uint32_t port,
                   bool enabled):
                   m_name{name}, m_port{port}, m_enabled{enabled}
                   { }

    ~NetworkService();

    /*! Enable copy */
    NetworkService(const NetworkService&) = default;
    NetworkService& operator=(const NetworkService&) = default;
    NetworkService(NetworkService&&) = default;
    NetworkService& operator=(NetworkService&&) = default;

    /*!
     * @brief Get name
     * @return name
     * */
    const enums::NetworkServiceName get_name() const {
        return m_name;
    }

    /*!
     * @brief Set name
     * @param[in] name Network service name
     * */
    void set_name(const enums::NetworkServiceName name) {
        m_name = name;
    }

    /*!
     * @brief Get port
     * @return port
     * */
    std::uint32_t get_port() const {
        return m_port;
    }

    /*!
     * @brief Set port
     * @param port Network service port
     * */
    void set_port(const std::uint32_t port) {
        m_port = port;
    }

    /*!
     * @brief Is enabled
     * @return true if network service is enabled, false otherwise
     * */
    bool get_enabled() const {
        return m_enabled;
    }

    /*!
     * @brief Set enabled
     * @param enabled Network service enabled
     * */
    void set_enabled(const bool enabled) {
        m_enabled = enabled;
    }

    /*!
	 * @brief construct an object of class NetworkService from JSON
	 *
	 * @param json the json::Json deserialized to object
	 *
	 * @return the newly constructed NetworkService object
	 */
	static NetworkService from_json(const json::Json& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;

private:
    enums::NetworkServiceName m_name{enums::NetworkServiceName::HTTP};
    std::uint32_t m_port{};
    bool m_enabled{};
};

}
}
}

