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
 * @file request.hpp
 * @brief tgt request interface
*/

#pragma once

#include <string>
#include <limits>
#include <vector>

namespace tgt {

/*! Request lld max size */
static constexpr unsigned LLD_SIZE = 64;

/*! TGT available modes */
enum class Mode {
    SYSTEM,
    TARGET,
    DEVICE,
    PORTAL,
    LLD,
    SESSION,
    CONNECTION,
    ACCOUNT
};

/*! TGT available operations */
enum class Operation {
    NEW,
    DELETE,
    SHOW,
    BIND,
    UNBIND,
    UPDATE,
    STATS,
    START,
    STOP
};

/*! POD request data */
struct RequestData {
    Mode m_mode{};                  // mode type
    Operation m_operation{};        // operation type
    char m_lld[LLD_SIZE];           // driver type eg. iscsi
    std::uint32_t m_length{0};      // request length
    std::int32_t m_target_id{-1};   // target id
    std::uint64_t m_sid{0};         // session id
    std::uint64_t m_lun{std::numeric_limits<std::uint64_t>::max()}; // logical unit
    std::uint32_t m_cid{0};         // connection id
    std::uint32_t m_host_no{0};     // host number
    std::uint32_t m_device_type{0}; // device type
    std::uint32_t m_ac_dir{0};      // account mode
    std::uint32_t m_pack{0};        // pack mode
    std::uint32_t m_force{0};       // force tgt action
};

/*! Class represents tgtadm request */
class Request {
public:
    /*! Request additional data type */
    using extra_data_t = std::vector<char>;

    /*!
     * @brief Get request data
     * @return Request data
     */
    std::vector<char> get_request_data();

    /*!
     * @brief Set request mode
     * @param mode Request mode
     */
    void set_mode(const Mode mode);

    /*!
     * @brief Set request operation
     * @param operation Request operation
     */
    void set_operation(const Operation operation);

    /*!
     * @brief Set request lld
     * @param lld Request lld
     */
    void set_lld(const std::string& lld);

    /*!
     * @brief Set requset target id
     * @param target_id Rquest target id
     */
    void set_target_id(const std::int32_t target_id);

    /*!
     * @brief Set request sid
     * @param sid Request sid
     */
    void set_sid(const std::uint64_t sid);

    /*!
     * @brief Set request lun
     * @param lun Request lun
     */
    void set_lun(const std::uint64_t lun);

    /*!
     * @brief Set request cid
     * @param cid Request cid
     */
    void set_cid(const std::uint32_t cid);

    /*!
     * @brief Set request host number
     * @param host_no Requst host number
     */
    void set_host_no(const std::uint32_t host_no);

    /*!
     * @brief Set request device type
     * @param device_type Request device type
     */
    void set_device_type(const std::uint32_t device_type);

    /*!
     * @brief Set request ac dir
     * @param ac_dir Request ac dir
     */
    void set_ac_dir(const std::uint32_t ac_dir);

    /*!
     * @brief Set request pack
     * @param pack Request pack
     */
    void set_pack(const std::uint32_t pack);

    /*!
     * @brief Set request force
     * @param force Request force
     */
    void set_force(const std::uint32_t force);

    /*!
     * @brief Add extra data to requet
     * @param data Additional data
     */
    void add_extra(const std::string& data);

    /*!
     * @brief Get request mode
     * @return Request mode
     */
    Mode get_mode() const;

    /*!
     * @brief Get request operation
     * @return Request operation
     */
    Operation get_operation() const;

    /*!
     * @brief Get request target id
     * @return Request target id
     */
    std::int32_t get_target_id() const;

    /*!
     * @brief Get request length
     * @return Request length
     */
    std::uint32_t get_length() const;

    /*!
     * @brief Get request sid
     * @return Request sid
     */
    std::uint64_t get_sid() const;

    /*!
     * @brief Get request lun
     * @return Request lun
     */
    std::uint64_t get_lun() const;

    /*!
     * @brief Get request cid
     * @return Request cid
     */
    std::uint32_t get_cid() const;

    /*!
     * @brief Get request host no
     * @return Request host no
     */
    std::uint32_t get_host_no() const;

    /*!
     * @brief Get request device type
     * @return Request device type
     */
    std::uint32_t get_device_type() const;

    /*!
     * @brief Get request ac dir
     * @return Request ac dir
     */
    std::uint32_t get_ac_dir() const;

    /*!
     * @brief Get request pack
     * @return Request pack
     */
    std::uint32_t get_pack() const;

    /*!
     * @brief Get request force
     * @return Request force
     */
    std::uint32_t get_force() const;

private:
    RequestData m_request_data{};
    extra_data_t m_extra_data{};

};

}


