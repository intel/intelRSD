/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file manager.hpp
 * @brief tgt manager
*/

#ifndef ISCSI_TGT_MANAGER_HPP
#define	ISCSI_TGT_MANAGER_HPP

#include <cstdint>
#include <string>
#include <map>
#include <mutex>

namespace agent {
namespace storage {
namespace iscsi {
namespace tgt {

class Request;
class Response;

/*! Manager class prepare and execute tgt commands */
class Manager {
public:
    using OptionMapper = std::map<std::string, std::string>;

    Manager() = default;
    /*! Disable copy */
    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

    /*!
     * @brief Execute create target command
     *
     * @param target_id New target id
     * @param target_name New taret name
     * @return On success true is returned, otherwise false
     */
    Response create_target(const std::int32_t target_id,
                                        const std::string& target_name) const;

    /*!
     * @brief Execute bind target command
     * @param target_id Target id
     * @param options Bind optons
     * @return On success true is returned, otherwise false
     */
    Response bind_target(const std::int32_t target_id,
                         const Manager::OptionMapper& options) const;

    /*!
     * @brief Execute unbind target command
     * @param target_id Target id
     * @param options Unbind options
     * @return On success true is returned, otherwise false
     */
    Response unbind_target(const std::int32_t target_id,
                         const Manager::OptionMapper& options) const;

    /**
     * @brief Execute update target command
     * @param target_id Target id
     * @param options Update options
     * @return On success true is returned, otherwise false
     */
    Response update_target(const std::int32_t target_id,
                         const Manager::OptionMapper& options) const;
    /**
     * @brief Execute create lun command
     *
     * @param target_id Target id
     * @param lun_id Lun id
     * @param device_path Backing-store path
     *
     * @return Response message object
     */
    Response create_lun(const std::int32_t target_id,
                                        const std::uint64_t lun_id,
                                        const std::string& device_path) const;
    /*!
     * @brief Execute destroy target command
     *
     * @param target_id Target id
     * @return On success true is returned, otherwise false
     */
    Response destroy_target(const std::int32_t target_id) const;

    /*!
     * @brief Execute show targets command
     */
    Response show_targets() const;

private:
    /*!
     * @brief Prepare create target request
     *
     * @param target_id Target id
     * @param target_name Target name
     * @return Target create Request object
     */
    Request create_target_request(const std::int32_t target_id,
                                    const std::string& target_name) const;

    /**
     * @brief Prepare create lun request
     * @param lun_id Lun id
     * @param device_path Backing-store path
     *
     * @return Lun create Request object
     */
    Request create_lun_request(const std::int32_t target_id,
                                    const std::uint64_t lun_id,
                                    const std::string& device_path) const;

    /*!
     * @brief Prepare destroy target request
     *
     * @param target_id Target id
     * @return Target destroy Request object
     */
    Request destroy_target_request(const std::int32_t target_id) const;

    /*!
     * @brief Prepare bind target request
     * @param target_id Target id
     * @param options Bind target options
     * @return Target bind request object
     */
    Request bind_target_request(const std::int32_t target_id,
                                   const OptionMapper& options) const;

    /*!
     * @brief Prepare unbind target request
     * @param target_id Target id
     * @param options Undind target options
     * @return Target unbind requets object
     */
    Request unbind_target_request(const std::int32_t target_id,
                                   const OptionMapper& options) const;

    /*!
     * @brief Prepare update target requet
     * @param target_id Target id
     * @param options Update target options
     * @return Target update request
     */
    Request update_target_request(const std::int32_t target_id,
                                   const OptionMapper& options) const;
    /*!
     * @brief Prepare show targets request
     * @return Targets show Request object
     */
    Request show_targets_request() const;


    /**
     * @brief Lock access to socket
     */
    mutable std::mutex m_mutex{};

};

}
}
}
}

#endif	/* ISCSI_TGT_MANAGER_HPP */

