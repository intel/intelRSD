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
 * @file iscsi_data.hpp
 * @brief Iscsi data interface
 * */

#pragma once

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! Iscsi data configutaion */
class IscsiData {
public:
    /*! Default constructor */
    explicit IscsiData();

    /*! Enable copy & assignment */
    IscsiData(IscsiData&&) = default;
    IscsiData(const IscsiData&) = default;
    IscsiData& operator=(IscsiData&&) = default;
    IscsiData& operator=(const IscsiData&) = default;

    /*!
     * @brief Set portal ip
     * @param portal_ip Portal ip address
     */
    void set_portal_ip(const std::string& portal_ip) {
        m_portal_ip = portal_ip;
    }

    /*!
     * @brief Get portal ip address
     * @return Portal ip
     */
    const std::string& get_portal_ip() const {
        return m_portal_ip;
    }

    /*!
     * @brief Set portal port
     * @param portal_port Portal port
     */
    void set_portal_port(unsigned portal_port) {
        m_portal_port = portal_port;
    }

    /*!
     * @brief Get portal port
     * @return Portal port
     */
    unsigned get_portal_port() const {
        return m_portal_port;
    }

    /*!
     * @brief Set initiator username
     * @param username    Username
     * */
    void set_username(const std::string& username) {
        m_username = username;
    }

    /*!
     * @brief Set initiator password
     * @param password    Password
     * */
    void set_password(const std::string& password) {
        m_password = password;
    }

    /*!
     * @brief Get initiator username
     * @return user name
     * */
    const std::string& get_username() const {
        return m_username;
    }

    /*!
     * @brief Get initiator password
     * @return password
     * */
    const std::string& get_password() const {
        return m_password;
    }

    /*!
     * @brief Set initiator
     * @param initiator initiator
     */
    void set_initiator(const std::string& initiator) {
        m_initiator = initiator;
    }

    /*!
     * @brief Get initiator
     * @return initiator
     */
    const std::string& get_initiator() const {
        return m_initiator;
    }

    /*!
     * @brief Set configuration path
     * @param conf_path configuration path, eg. /etc/tgt/conf.d/
     */
    void set_configuration_path(const std::string& conf_path) {
        m_conf_path = conf_path;
    }

    /*!
     * @brief Get configuration path
     * @return path to the configuration
     */
    const std::string& get_configuration_path() const {
        return m_conf_path;
    }


    /*!
     * @brief Set portal ip interface name
     * @param interface portal ip interface name, eg. eth0
     */
    void set_portal_interface(const std::string& interface) {
        m_portal_interface = interface;
    }


    /*!
     * @brief Get portal interface name
     * @return name of portal ip interface
     */
    const std::string& get_portal_interface() const {
        return m_portal_interface;
    }

    /*! Default destructor */
    ~IscsiData();

private:
    unsigned m_portal_port{};
    std::string m_portal_ip{};
    std::string m_username{};
    std::string m_password{};
    std::string m_initiator{};
    std::string m_conf_path{};
    std::string m_portal_interface{};
};

}
}
}
