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
 *
 *
 * @file ssdp_service_config.hpp
 *
 * @brief Declaration of SsdpServiceConfig.
 * */

#pragma once

#include <chrono>
#include <string>
#include <unordered_set>

namespace ssdp {

/*!
 * SsdpService configuration object.
 *
 * Contains configuration settings for:
 * - UUID of service being announced
 * - disable/enable of ssdp:alive/ssdp:byebye messages
 * - announcement interval of ssdp packets
 * - TTL value for ssdp datagrams
 * - List of network interfaces on which ssdp packets are accepted
 **/
class SsdpServiceConfig final {
public:
    /*!
     * Enables/Disables SSDP service.
     */
    void enable_ssdp_service() {
        m_ssdp_service_enabled = true;
    }

    /*!
     * Disables SSDP service.
     */
    void disable_ssdp_service() {
        m_ssdp_service_enabled = false;
    }

    /*!
     * Returns state of SSDP service.
     * @return true if SSDP service is enabled, false otherwise.
     */
    bool is_ssdp_service_enabled() const {
        return m_ssdp_service_enabled;
    }

    /*!
     * Service uuid setter
     * @param service_uuid UUID of announced service
     */
    void set_service_uuid(const std::string& service_uuid) {
        m_service_uuid = service_uuid;
    }

    /*!
     * Service uuid getter.
     * @return UUID of announced service
     */
    const std::string& get_service_uuid() const {
        return m_service_uuid;
    }

    /*!
     * Service URN setter.
     * @param service_urn URN of announced service
     */
    void set_service_urn(const std::string& service_urn) {
        m_service_urn = service_urn;
    }

    /*!
     * Service URN getter
     * Defaults value "urn:dmtf-org:service:redfish-rest:1"
     * @return URN of announced service
     */
    const std::string& get_service_urn() const {
        return m_service_urn;
    }

    /*!
     * Service URL setter
     * Defaults value "urn:dmtf-org:service:redfish-rest:1"
     * @param service_url URL of announced service
     */
    void set_service_url(const std::string& service_url) {
        m_service_url = service_url;
    }

    /*!
     * Service URL getter
     * @return URL of announced service
     */
    const std::string& get_service_url() const {
        return m_service_url;
    }

    /*!
     * SSDP datagram TTL value setter
     * @param ttl TTL value of datagram
     */
    void set_socket_ttl(unsigned char ttl) {
        m_ttl = ttl;
    }

    /*!
     * SSDP datagram TTL value getter
     * @return TTL value of datagram
     */
    unsigned char get_socket_ttl() const {
        return m_ttl;
    }

    /*!
     * Returns state of sending SSDP announcement messages.
     * @return true if announcement messages are turned on, false otherwise.
     */
    bool is_announcement_enabled() const {
        return m_announce_interval > std::chrono::seconds(0);
    }

    /*!
     * SSDP service announcement interval setter.
     * Default value = 15min.
     * @param interval service announcement interval.
     */
    void set_announce_interval(const std::chrono::seconds& interval) {
        m_announce_interval = interval;
    }

    /*!
     * SSDP service announcement interval getter.
     * Default value = 15min.
     * @return SSDP service announcement interval
     */
    const std::chrono::seconds& get_announce_interval() const {
        return m_announce_interval;
    }

    /*!
     * Adds network interface name to a collection of interfaces on which
     * ssdp packets are accepted.
     * @param nic_name network interface name
     */
    void add_nic_name(const std::string& nic_name) {
        m_nic_names.emplace(nic_name);
    }

    /*!
     * Network interface names collection getter.
     * @return Collection of network interface names
     * on which ssdp packets are accepted.
     */
    const std::unordered_set<std::string>& get_nic_names() const {
        return m_nic_names;
    }

private:
    bool m_ssdp_service_enabled{false};
    std::string m_service_uuid{};
    std::string m_service_urn{"urn:dmtf-org:service:redfish-rest:1"};
    std::string m_service_url{};
    unsigned char m_ttl{2};
    std::chrono::seconds m_announce_interval{0};
    std::unordered_set<std::string> m_nic_names{};
};

}
