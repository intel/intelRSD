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
 * @file nvme-target/sysfs_nvme_target_handler.hpp
 */

#pragma once

#include "base_nvme_target_handler.hpp"
#include "sysfs/abstract_sysfs_interface.hpp"

#include <memory>

namespace nvme_target {

/*!
 * Implementation of the BaseNvmeTargetHandler based on the sysfs lib
 */
class SysfsNvmeTargetHandler : public BaseNvmeTargetHandler {
public:

    /*!
     * @brief Builds a valid handler object
     * @param sysfs Sysfs interface to be used by the handler
     */
    SysfsNvmeTargetHandler(std::shared_ptr<sysfs::AbstractSysfsInterface> sysfs): m_sysfs(sysfs) {}

    virtual ~SysfsNvmeTargetHandler();

    /*! Implementation of the pure virtual methods */
    virtual IdList get_hosts() const override;
    virtual void add_host(const Id& hid) const override;
    virtual void remove_host(const Id& hid) const override;
    virtual IdList get_subsystems() const override;
    virtual void add_subsystem(const Id& ssid) const override;
    virtual void remove_subsystem(const Id& ssid) const override;
    virtual void set_subsystem_allow_all_hosts(const Id& ssid, bool allow_all_hosts_flag) const override;
    virtual IdList get_subsystem_hosts(const Id& ssid) const override;
    virtual void add_subsystem_host(const Id& ssid, const Id& hid) const override;
    virtual void remove_subsystem_host(const Id& ssid, const Id& hid) const override;
    virtual IdList get_subsystem_namespaces(const Id& ssid) const override;
    virtual void add_subsystem_namespace(const Id& ssid, const Id& nid) const override;
    virtual void remove_subsystem_namespace(const Id& ssid, const Id& nid) const override;
    virtual void set_subsystem_namespace_device(const Id& ssid, const Id& nid, const std::string& path) const override;
    virtual std::string get_subsystem_namespace_device(const Id& ssid, const Id& nid) const override;
    virtual void set_subsystem_namespace_enable(const Id& ssid, const Id& nid, bool enable_flag) const override;
    virtual IdList get_ports() const override;
    virtual void add_port(const Id& pid) const override;
    virtual void remove_port(const Id& pid) const override;
    virtual void set_port_params(const Id& pid, const PortParams&) const override;
    virtual PortParams get_port_params(const Id& pid) const override;
    virtual IdList get_port_subsystems(const Id& pid) const override;
    virtual void add_port_subsystem(const Id& pid, const Id& ssid) const override;
    virtual void remove_port_subsystem(const Id& pid, const Id& ssid) const override;
    virtual IdList get_port_referrals(const Id& pid) const override;
    virtual void add_port_referral(const Id& pid, const Id& rid) const override;
    virtual void remove_port_referral(const Id& pid, const Id& rid) const override;
    virtual void set_port_referral_params(const Id& pid, const Id& rid, const PortParams& params) const override;
    virtual PortParams get_port_referral_params(const Id& pid, const Id& rid) const override;
    virtual void set_port_referral_enable(const Id& pid, const Id& rid, bool enable_flag) const override;

private:

    std::shared_ptr<sysfs::AbstractSysfsInterface> m_sysfs{};

};

}
