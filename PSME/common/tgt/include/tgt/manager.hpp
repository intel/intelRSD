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
 * @file manager.hpp
 * @brief TGT manager
*/

#pragma once



#include <cstdint>
#include <string>
#include <map>
#include <mutex>

#include "tgt/target_data.hpp"
#include "tgt/iscsi_data.hpp"


namespace tgt {

class Request;
class Response;

/*! @brief Manager class prepare and execute tgt commands */
class Manager {
public:
    using OptionMapper = std::map<std::string, std::string>;


    /*!
     * @brief Constructor.
     * @param socket Path to TGT daemon socket.
     */
    Manager(const std::string& socket) : m_socket_name(socket) { }


    /*! Disable copy */
    Manager(const Manager&) = delete;


    Manager& operator=(const Manager&) = delete;


    /*!
     * @brief Execute create target command
     *
     * @param target_id New target id
     * @param target_name New target name
     *
     * @return Response message object
     */
    Response create_target(const TargetData::Id target_id, const std::string& target_name) const;


    /*!
     * @brief Execute bind target command
     * @param target_id Target id
     * @param options Bind options
     * @return Response message object
     */
    Response bind_target(const TargetData::Id target_id,
                         const Manager::OptionMapper& options) const;


    /*!
     * @brief Execute unbind target command
     * @param target_id Target id
     * @param options Unbind options
     * @return Response message object
     */
    Response unbind_target(const TargetData::Id target_id,
                           const Manager::OptionMapper& options) const;


    /*!
     * @brief Execute update target command
     * @param target_id Target id
     * @param options Update options
     * @return Response message object
     */
    Response update_target(const TargetData::Id target_id,
                           const Manager::OptionMapper& options) const;


    /*!
     * @brief Execute create lun command
     *
     * @param target_id Target id
     * @param lun_id Lun id
     * @param device_path Backing-store path
     *
     * @return Response message object
     */
    Response create_lun(const TargetData::Id target_id,
                        const LunData::Id lun_id,
                        const std::string& device_path) const;


    /*!
     * @brief Execute destroy target command
     *
     * @param target_id Target id
     * @return Response message object
     */
    Response destroy_target(const TargetData::Id target_id) const;


    /*!
     * @brief Execute create chap account command
     * @param username Chap account username
     * @param password Chap account password
     * @return Response message object
     */
    Response create_chap_account(const std::string& username, const std::string& password) const;


    /*!
     * @brief Execute delete chap account command
     * @param username Chap account username
     * @return Response message object
     */
    Response delete_chap_account(const std::string& username) const;


    /*!
     * @brief Execute bind chap account command
     * @param target_id Target id
     * @param username Chap account username
     * @param mutual Chap mode: true if mutual, otherwise one-way
     * @return Response message object
     */
    Response bind_chap_account(const TargetData::Id target_id,
                               const std::string& username,
                               const bool mutual = false) const;


    /*!
     * @brief Execute unbind chap account command
     * @param target_id Target id
     * @param username Chap account username
     * @param mutual Chap mode: true if mutual, otherwise one-way
     * @return Response message object
     */
    Response unbind_chap_account(const TargetData::Id target_id,
                                 const std::string& username,
                                 const bool mutual = false) const;


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
    Request create_target_request(const TargetData::Id target_id,
                                  const std::string& target_name) const;


    /*!
     * @brief Prepare create lun request
     * @param lun_id Lun id
     * @param device_path Backing-store path
     *
     * @return Lun create Request object
     */
    Request create_lun_request(const TargetData::Id target_id,
                               const LunData::Id lun_id,
                               const std::string& device_path) const;


    /*!
     * @brief Prepare destroy target request
     *
     * @param target_id Target id
     *
     * @return Target destroy Request object
     */
    Request destroy_target_request(const TargetData::Id target_id) const;


    /*!
     * @brief Prepare bind target request
     * @param target_id Target id
     * @param options Bind target options
     * @return Target bind request object
     */
    Request bind_target_request(const TargetData::Id target_id,
                                const OptionMapper& options) const;


    /*!
     * @brief Prepare unbind target request
     * @param target_id Target id
     * @param options Unbind target options
     * @return Target unbind request object
     */
    Request unbind_target_request(const TargetData::Id target_id,
                                  const OptionMapper& options) const;


    /*!
     * @brief Prepare update target request
     * @param target_id Target id
     * @param options Update target options
     * @return Target update request
     */
    Request update_target_request(const TargetData::Id target_id,
                                  const OptionMapper& options) const;


    /*!
     * @brief Prepare show targets request
     * @return Targets show Request object
     */
    Request show_targets_request() const;


    /*!
     * @brief Prepare create chap account request
     *
     * @param username Chap account username
     * @param password Chap account password
     * @return Account create request object
     */
    Request create_chap_account_request(const std::string& username,
                                        const std::string& password) const;


    /*!
     * @brief Prepare delete chap account request
     *
     * @param username Chap account username
     *
     * @return Account delete request object
     */
    Request delete_chap_account_request(const std::string& username) const;


    /*!
     * @brief Prepare bind one-way chap account request
     * @param target_id Target id
     * @param username Chap account username
     * @param mutual Chap mode: true if mutual, otherwise one-way
     * @return Account bind request object
     */
    Request bind_chap_account_request(const TargetData::Id target_id,
                                      const std::string& username,
                                      const bool mutual) const;


    /*!
     * @brief Prepare unbind one-way chap account request
     * @param target_id Target id
     * @param username Chap account username
     * @param mutual Chap mode: true if mutual, otherwise one-way
     * @return Account unbind request object
     */
    Request unbind_chap_account_request(const TargetData::Id target_id,
                                        const std::string& username,
                                        const bool mutual) const;


    /*! @brief Lock access to socket */
    mutable std::mutex m_mutex{};

    /*! @brief Path to TGT daemon socket */
    std::string m_socket_name{};

};

}


