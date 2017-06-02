/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file network_interface_info.hpp
 *
 * @brief Network interface information reader
 * */

#pragma once
#include <string>

namespace psme {
namespace rest {
namespace utils {

/*!
 * @brief Util class for reading network interface information
 */
class NetworkInterfaceInfo {
public:
    /*!
     * @brief NetworkInterfaceAddress class keeps IP and Netmask Address
     * */
    class NetworkInterfaceAddress {
    public:
        /*!
         * @brief Sets Netmask
         *
         * @param netmask Netmask
         */
        void set_netmask(const std::string& netmask) {
            m_netmask = netmask;
        }

        /*!
         * @brief Gets Netmask
         *
         * @return netmask
         */
        const std::string& get_netmask() const {
            return m_netmask;
        }

        /*!
         * @brief Sets IP Address
         *
         * @param ip_address IP Address
         */
        void set_ip_address(const std::string& ip_address) {
            m_ip_address = ip_address;
        }

        /*!
         * @brief Gets IP Address
         *
         * @return IP Address
         */
        const std::string& get_ip_address() const {
            return m_ip_address;
        }

        /*!
         * @brief Sets MAC Address
         *
         * @param mac_address MAC Address
         */
        void set_mac_address(const std::string& mac_address) {
            m_mac_address = mac_address;
        }

        /*!
         * @brief Gets MAC Address
         *
         * @return MAC Address
         */
        const std::string& get_mac_address() const {
            return m_mac_address;
        }
    private:
        std::string m_ip_address{};
        std::string m_netmask{};
        std::string m_mac_address{};
    };

    /*!
     * @brief Default Constructor
     */
    NetworkInterfaceInfo() = delete;

    /*!
     * @brief Constructor
     * @param interface_name Name of the network interface
     */
    NetworkInterfaceInfo(const std::string& interface_name) :
        m_interface_name{interface_name} {
    }

    /*!
     * @brief Copy Constructor
     */
    NetworkInterfaceInfo(NetworkInterfaceInfo&) = delete;

    /*!
     * @brief Move Constructor
     */
    NetworkInterfaceInfo(NetworkInterfaceInfo&&) = delete;

    /*!
     * @brief Assignment operator
     */
    NetworkInterfaceInfo& operator=(const NetworkInterfaceInfo &) = delete;

    /*!
     * @brief Gets Network Interface Address i.e. IP Address, Netmask, MAC
     * @return Network Interface Address
     */
    NetworkInterfaceAddress get_interface_address();

private:
    std::string m_interface_name{};
    int m_fd{};
    void m_socket_open();
    const std::string m_get_mac_address() const;
    const std::string m_get_ip_address() const;
    const std::string m_get_netmask() const;
    void m_socket_close();
};

}
}
}
