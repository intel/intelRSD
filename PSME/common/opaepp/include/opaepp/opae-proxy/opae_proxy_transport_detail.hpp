/*!
 * @brief Details of specific transport.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 *
 * @file transport_detail.hpp
 */

#pragma once
namespace opaepp {

class TransportDetails {
public:
    /**
     * @brief               OPAE-Proxy transport details
     * @param protocol_name Protocol name
     * @param ipv4          Target host IP address
     * @param port          Target host port
     */
    TransportDetails(std::string protocol_name, std::string ipv4, const uint16_t port) :
        m_protocol_name{std::move(protocol_name)},
        m_ipv4{std::move(ipv4)},
        m_port{port} {}


    /**
     * @brief Get protocol name
     * @return Protocol name
     */
    std::string get_protocol_name() const {
        return m_protocol_name;
    }


    /**
     * @brief Get target host IPv4 address
     * @return Target host IPv4 address
     */
    std::string get_ipv4_address() const {
        return m_ipv4;
    }


    /**
     * @brief Get target host port
     * @return Get target host port
     */
    uint16_t get_port() const {
        return m_port;
    }


private:
    std::string m_protocol_name{};
    std::string m_ipv4{};
    uint16_t m_port{};
};

}