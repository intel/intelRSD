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
 * @file nvme-target/sysfs_nvme_target_handler.cpp
 */

#include "nvme-target/sysfs_nvme_target_handler.hpp"

#include <cctype>
#include <algorithm>

using namespace nvme_target;
using namespace sysfs;

namespace {
    const Path BASE_PATH = "/sys/kernel/config/nvmet/";
    const std::string DIR_HOSTS = "hosts";
    const std::string DIR_SUBSYSTEMS = "subsystems";
    const std::string DIR_PORTS = "ports";
    const std::string DIR_ALLOWED_HOSTS = "allowed_hosts";
    const std::string DIR_NAMESPACES = "namespaces";
    const std::string DIR_REFERRALS = "referrals";
    const std::string FILE_ALLOW_ALL = "attr_allow_any_host";
    const std::string FILE_DEV_PATH = "device_path";
    const std::string FILE_ENABLE = "enable";
    const std::string FILE_FAMILY = "addr_adrfam";
    const std::string FILE_IP = "addr_traddr";
    const std::string FILE_TYPE = "addr_trtype";
    const std::string FILE_PORT = "addr_trsvcid";
    const std::string VALUE_DISABLED = "0";
    const std::string VALUE_ENABLED = "1";

    Id left_trim(Id s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    Id right_trim(Id s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    Id trim(Id s) {
        return left_trim(right_trim(s));
    }
}

SysfsNvmeTargetHandler::~SysfsNvmeTargetHandler() {}

IdList SysfsNvmeTargetHandler::get_hosts() const {
    IdList ret{};
    for (const auto& path : m_sysfs->get_dir(BASE_PATH / DIR_HOSTS).dirs) {
        ret.emplace_back(path.basename());
    }
    return ret;
}

void SysfsNvmeTargetHandler::add_host(const Id& hid) const {
    m_sysfs->add_dir(BASE_PATH / DIR_HOSTS, hid);
}

void SysfsNvmeTargetHandler::remove_host(const Id& hid) const {
    m_sysfs->del_dir(BASE_PATH / DIR_HOSTS / hid);
}

IdList SysfsNvmeTargetHandler::get_subsystems() const {
    IdList ret{};
    for (const auto& path : m_sysfs->get_dir(BASE_PATH / DIR_SUBSYSTEMS).dirs) {
        ret.emplace_back(path.basename());
    }
    return ret;
}

void SysfsNvmeTargetHandler::add_subsystem(const Id& ssid) const {
    m_sysfs->add_dir(BASE_PATH / DIR_SUBSYSTEMS, ssid);
}

void SysfsNvmeTargetHandler::remove_subsystem(const Id& ssid) const {
    m_sysfs->del_dir(BASE_PATH / DIR_SUBSYSTEMS / ssid);
}

void SysfsNvmeTargetHandler::set_subsystem_allow_all_hosts(const Id& ssid, bool allow_all_hosts_flag) const {
    std::string value = (allow_all_hosts_flag ? VALUE_ENABLED : VALUE_DISABLED);
    m_sysfs->set_file(BASE_PATH / DIR_SUBSYSTEMS / ssid / FILE_ALLOW_ALL, value);
}

IdList SysfsNvmeTargetHandler::get_subsystem_hosts(const Id& ssid) const {
    IdList ret{};
    for (const auto& link_path : m_sysfs->get_dir(BASE_PATH / DIR_SUBSYSTEMS / ssid / DIR_ALLOWED_HOSTS).links) {
        ret.emplace_back(m_sysfs->get_absolute_path(link_path).basename());
    }
    return ret;
}

void SysfsNvmeTargetHandler::add_subsystem_host(const Id& ssid, const Id& hid) const {
    m_sysfs->add_link(BASE_PATH / DIR_SUBSYSTEMS / ssid / DIR_ALLOWED_HOSTS, hid, BASE_PATH / DIR_HOSTS / hid);
}

void SysfsNvmeTargetHandler::remove_subsystem_host(const Id& ssid, const Id& hid) const {
    m_sysfs->del_link(BASE_PATH / DIR_SUBSYSTEMS / ssid / DIR_ALLOWED_HOSTS / hid);
}

IdList SysfsNvmeTargetHandler::get_subsystem_namespaces(const Id& ssid) const {
    IdList ret{};
    for (const auto& path : m_sysfs->get_dir(BASE_PATH / DIR_SUBSYSTEMS / ssid / DIR_NAMESPACES).dirs) {
        ret.emplace_back(path.basename());
    }
    return ret;
}

void SysfsNvmeTargetHandler::add_subsystem_namespace(const Id& ssid, const Id& nid) const {
    m_sysfs->add_dir(BASE_PATH / DIR_SUBSYSTEMS / ssid / DIR_NAMESPACES, nid);
}

void SysfsNvmeTargetHandler::remove_subsystem_namespace(const Id& ssid, const Id& nid) const {
    m_sysfs->del_dir(BASE_PATH / DIR_SUBSYSTEMS / ssid / DIR_NAMESPACES / nid);
}

void SysfsNvmeTargetHandler::set_subsystem_namespace_device(const Id& ssid, const Id& nid, const std::string& path) const {
    m_sysfs->set_file(BASE_PATH / DIR_SUBSYSTEMS / ssid / DIR_NAMESPACES / nid / FILE_DEV_PATH, path);
}

std::string SysfsNvmeTargetHandler::get_subsystem_namespace_device(const Id& ssid, const Id& nid) const {
    return trim(m_sysfs->get_file(BASE_PATH / DIR_SUBSYSTEMS / ssid / DIR_NAMESPACES / nid / FILE_DEV_PATH).value);
}

void SysfsNvmeTargetHandler::set_subsystem_namespace_enable(const Id& ssid, const Id& nid, bool enable_flag) const {
    std::string value = (enable_flag ? VALUE_ENABLED : VALUE_DISABLED);
    m_sysfs->set_file(BASE_PATH / DIR_SUBSYSTEMS / ssid / DIR_NAMESPACES / nid / FILE_ENABLE, value);
}

IdList SysfsNvmeTargetHandler::get_ports() const {
    IdList ret{};
    for (const auto& path : m_sysfs->get_dir(BASE_PATH / DIR_PORTS).dirs) {
        ret.emplace_back(path.basename());
    }
    return ret;
}

void SysfsNvmeTargetHandler::add_port(const Id& pid) const {
    m_sysfs->add_dir(BASE_PATH / DIR_PORTS, pid);
}

void SysfsNvmeTargetHandler::remove_port(const Id& pid) const {
    m_sysfs->del_dir(BASE_PATH / DIR_PORTS / pid);
}

void SysfsNvmeTargetHandler::set_port_params(const Id& pid, const PortParams& params) const {
    m_sysfs->set_file(BASE_PATH / DIR_PORTS / pid / FILE_IP, std::get<0>(params));
    m_sysfs->set_file(BASE_PATH / DIR_PORTS / pid / FILE_PORT, std::to_string(uint32_t(std::get<1>(params))));
    m_sysfs->set_file(BASE_PATH / DIR_PORTS / pid / FILE_FAMILY, std::get<2>(params));
    m_sysfs->set_file(BASE_PATH / DIR_PORTS / pid / FILE_TYPE, std::get<3>(params));
}

PortParams SysfsNvmeTargetHandler::get_port_params(const Id& pid) const {
    auto ip = trim(m_sysfs->get_file(BASE_PATH / DIR_PORTS / pid / FILE_IP).value);
    auto port = trim(m_sysfs->get_file(BASE_PATH / DIR_PORTS / pid / FILE_PORT).value);
    auto ip_family = trim(m_sysfs->get_file(BASE_PATH / DIR_PORTS / pid / FILE_FAMILY).value);
    auto transport = trim(m_sysfs->get_file(BASE_PATH / DIR_PORTS / pid / FILE_TYPE).value);
    return std::make_tuple(ip, uint16_t(std::stoi(port, 0)), ip_family, transport);
}

IdList SysfsNvmeTargetHandler::get_port_subsystems(const Id& pid) const {
    IdList ret{};
    for (const auto& link_path : m_sysfs->get_dir(BASE_PATH / DIR_PORTS / pid / DIR_SUBSYSTEMS).links) {
        ret.emplace_back(m_sysfs->get_absolute_path(link_path).basename());
    }
    return ret;
}

void SysfsNvmeTargetHandler::add_port_subsystem(const Id& pid, const Id& ssid) const {
    m_sysfs->add_link(BASE_PATH / DIR_PORTS / pid / DIR_SUBSYSTEMS, ssid, BASE_PATH / DIR_SUBSYSTEMS / ssid);
}

void SysfsNvmeTargetHandler::remove_port_subsystem(const Id& pid, const Id& ssid) const {
    m_sysfs->del_link(BASE_PATH / DIR_PORTS / pid / DIR_SUBSYSTEMS / ssid);
}

IdList SysfsNvmeTargetHandler::get_port_referrals(const Id& pid) const {
    IdList ret{};
    for (const auto& path : m_sysfs->get_dir(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS).dirs) {
        ret.emplace_back(path.basename());
    }
    return ret;
}

void SysfsNvmeTargetHandler::add_port_referral(const Id& pid, const Id& rid) const {
    m_sysfs->add_dir(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS, rid);
}

void SysfsNvmeTargetHandler::remove_port_referral(const Id& pid, const Id& rid) const {
    m_sysfs->del_dir(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS / rid);
}

void SysfsNvmeTargetHandler::set_port_referral_params(const Id& pid, const Id& rid, const PortParams& params) const {
    m_sysfs->set_file(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS / rid / FILE_IP, std::get<0>(params));
    m_sysfs->set_file(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS / rid / FILE_PORT, std::to_string(uint32_t(std::get<1>(params))));
    m_sysfs->set_file(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS / rid / FILE_FAMILY, std::get<2>(params));
    m_sysfs->set_file(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS / rid / FILE_TYPE, std::get<3>(params));
}

PortParams SysfsNvmeTargetHandler::get_port_referral_params(const Id& pid, const Id& rid) const {
    auto ip = trim(m_sysfs->get_file(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS / rid / FILE_IP).value);
    auto port = trim(m_sysfs->get_file(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS / rid / FILE_PORT).value);
    auto ip_family = trim(m_sysfs->get_file(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS / rid / FILE_FAMILY).value);
    auto transport = trim(m_sysfs->get_file(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS / rid / FILE_TYPE).value);
    return std::make_tuple(ip, uint16_t(std::stoi(port, 0)), ip_family, transport);
}

void SysfsNvmeTargetHandler::set_port_referral_enable(const Id& pid, const Id& rid, bool enable_flag) const {
    std::string value = (enable_flag ? VALUE_ENABLED : VALUE_DISABLED);
    m_sysfs->set_file(BASE_PATH / DIR_PORTS / pid / DIR_REFERRALS / rid / FILE_ENABLE, value);
}
