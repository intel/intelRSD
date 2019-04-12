/*!
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
 * @file nvme-target/base_nvme_target_handler.hpp
 */

#pragma once

#include <string>
#include <vector>
#include <tuple>

namespace nvme_target {

using Id = std::string;
using IdList = std::vector<std::string>;
// std::tuple<ip_address, rdma_port_num, ip_family, transport_type>
using PortParams = std::tuple<std::string, uint16_t, std::string, std::string>;

/*!
 * Base class for Nvme Target configuration handlers that wrap operations on the nvme target configuration
 */
class BaseNvmeTargetHandler {
public:

    virtual ~BaseNvmeTargetHandler();




    /* Host management commands */

    /*!
     * @brief Returns host list
     * @return List of all host ids
     */
    virtual IdList get_hosts() const = 0;

    /*!
     * @brief Adds a new host
     * @param hid Id of the new host
     */
    virtual void add_host(const Id& hid) const = 0;

    /*!
     * @brief Removes a host
     * @param hid Id of the host to be removed
     */
    virtual void remove_host(const Id& hid) const = 0;




    /* Subsystem management commands */

    /*!
     * @brief Returns subsystem list
     * @return List of all subsystem ids
     */
    virtual IdList get_subsystems() const = 0;

    /*!
     * @brief Adds a new subsystem
     * @param ssid Id of the new subsystem
     */
    virtual void add_subsystem(const Id& ssid) const = 0;

    /*!
     * @brief Removes a subsystem
     * @param ssid Id of the subsystem to be removed
     */
    virtual void remove_subsystem(const Id& ssid) const = 0;

    /*!
     * @brief Sets allow_all_hosts property of the subsystem
     * @param ssid Id of the subsystem
     * @param allow_all_hosts_flag New value of the allow_all_hosts property
     */
    virtual void set_subsystem_allow_all_hosts(const Id& ssid, bool allow_all_hosts_flag) const = 0;




    /* Subsystem host management commands */

    /*!
     * @brief Returns a list of subsystem allowed host
     * @param ssid Id of the parent subsystem
     * @return List of all subsystem allowed hosts
     */
    virtual IdList get_subsystem_hosts(const Id& ssid) const = 0;

    /*!
     * @brief Adds a new allowed host in the subsystem
     * @param ssid Id of the parent subsystem
     * @param hid Id of the host to be added
     */
    virtual void add_subsystem_host(const Id& ssid, const Id& hid) const = 0;

    /*!
     * @brief Removes an allowed host from the subsystem
     * @param ssid Id of the parent subsystem
     * @param hid Id of the host to be removed
     */
    virtual void remove_subsystem_host(const Id& ssid, const Id& hid) const = 0;




    /* Subsystem namespace management commands */

    /*!
     * @brief Returns a list of sybsystem namespaces
     * @param ssid Id of the parent subsystem
     * @return List of all namespaces in the subsystem
     */
    virtual IdList get_subsystem_namespaces(const Id& ssid) const = 0;

    /*!
     * @brief Adds a new subsystem namespace
     * @param ssid Id of the parent subsystem
     * @param nid Id of the new namespace
     */
    virtual void add_subsystem_namespace(const Id& ssid, const Id& nid) const = 0;

    /*!
     * @brief Removes a namespace from subsystem
     * @param ssid Id of the parent subsystem
     * @param nid Id of the new namespace to be removed
     */
    virtual void remove_subsystem_namespace(const Id& ssid, const Id& nid) const = 0;

    /*!
     * @brief Sets device path in the namespace
     * @param ssid Id of the parent subsystem
     * @param nid Id of the parent namespace
     * @param path New path to the namespace device
     */
    virtual void set_subsystem_namespace_device(const Id& ssid, const Id& nid, const std::string& path) const = 0;

    /*!
     * @brief Gets device path from the namespace
     * @param ssid Id of the parent subsystem
     * @param nid Id of the parent namespace
     * @return Device path stored in the namespace
     */
    virtual std::string get_subsystem_namespace_device(const Id& ssid, const Id& nid) const = 0;

    /*!
     * @brief Sets enable property in the namespace
     * @param ssid Id of the parent subsystem
     * @param nid Id of the parent namespace
     * @param enable_flag New value of the enable flag
     */
    virtual void set_subsystem_namespace_enable(const Id& ssid, const Id& nid, bool enable_flag) const = 0;




    /* Port management commands */

    /*!
     * @brief Returns a list of ports
     * @return List of all ports
     */
    virtual IdList get_ports() const = 0;

    /*!
     * @brief Adds a new port
     * @param pid Id of the port
     */
    virtual void add_port(const Id& pid) const = 0;

    /*!
     * @brief Removes a port
     * @param pid Id of the port
     */
    virtual void remove_port(const Id& pid) const = 0;

    /*!
     * @brief Sets port properties
     * @param pid Id of the port
     * @param params New port params values (ip and port number)
     */
    virtual void set_port_params(const Id& pid, const PortParams& params) const = 0;

    /*!
     * @brief Gets port properties
     * @param pid Id of the port
     * @return Port parameters values
     */
    virtual PortParams get_port_params(const Id& pid) const = 0;




    /* Port subsystem management commands */

    /*!
     * @brief Returns a list of port subsystems
     * @param pid Id of the parent port
     * @return List of all subsystems
     */
    virtual IdList get_port_subsystems(const Id& pid) const = 0;

    /*!
     * @brief Adds a new port subsystem
     * @param pid Id of the parent port
     * @param ssid Id of the new subsystem
     */
    virtual void add_port_subsystem(const Id& pid, const Id& ssid) const = 0;

    /*!
     * @brief Removes a subsystem from the port
     * @param pid Id of the parent port
     * @param ssid Id of the subsystem to be removed
     */
    virtual void remove_port_subsystem(const Id& pid, const Id& ssid) const = 0;




    /* Port referrals management commands */

    /*!
     * @brief Returns a list of port referrals
     * @param pid Id of the parent port
     * @return List of all port referrals
     */
    virtual IdList get_port_referrals(const Id& pid) const = 0;

    /*!
     * @brief Adds a new port referral
     * @param pid Id of the parent port
     * @param rid Id of the new referral
     */
    virtual void add_port_referral(const Id& pid, const Id& rid) const = 0;

    /*!
     * @brief Removes a referral from the port
     * @param pid Id of the parent port
     * @param rid Id of the referral to be removed
     */
    virtual void remove_port_referral(const Id& pid, const Id& rid) const = 0;

    /*!
     * @brief Sets port referral properties
     * @param pid Id of the parent port
     * @param rid Id of the referral
     * @param params New ip and port values
     */
    virtual void set_port_referral_params(const Id& pid, const Id& rid, const PortParams& params) const = 0;

    /*!
     * @brief Gets port referral properties (ip and port number)
     * @param pid Id of the parent port
     * @param rid Id of the referral
     * @return Ip and port of the port referral
     */
    virtual PortParams get_port_referral_params(const Id& pid, const Id& rid) const = 0;

    /*!
     * @brief Sets port referral enable property
     * @param pid Id of the parent port
     * @param rid Id of the referral
     * @param enable_flag New value of the enable flag
     */
    virtual void set_port_referral_enable(const Id& pid, const Id& rid, bool enable_flag) const = 0;

};

}
