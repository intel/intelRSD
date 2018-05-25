/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file target.hpp
 * @brief Target model interface
 * */
#pragma once

#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/storage.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! Target */
class IscsiTarget : public Resource {
public:
    explicit IscsiTarget(const std::string& parent_uuid = {},
                         enums::Component parent_type = enums::Component::StorageService);

    virtual ~IscsiTarget();

    IscsiTarget(const IscsiTarget&) = default;
    IscsiTarget& operator=(const IscsiTarget&) = default;
    IscsiTarget(IscsiTarget&&) = default;
    IscsiTarget& operator=(IscsiTarget&&) = default;

    using TargetLuns = attribute::Array<attribute::TargetLun>;

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return IscsiTarget::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return IscsiTarget::component;
    }

    /*!
     * @brief construct an object of class IscsiTarget from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed IscsiTarget object
     */
    static IscsiTarget from_json(const json::Json& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;

    /*!
     * @brief return an object initiator_iqn of class string
     *
     * @return the initiator_iqn value
     */
    const OptionalField<std::string>& get_initiator_iqn() const {
        return m_initiator_iqn;
    }

    /*!
     * @brief setter for initiator_iqn attribute
     *
     * @param initiator_iqn of type string
     */
    void set_initiator_iqn(const OptionalField<std::string>& initiator_iqn) {
        m_initiator_iqn = initiator_iqn;
    }

    /*!
     * @brief return an object target_address of class string
     *
     * @return the target_address value
     */
    const OptionalField<std::string>& get_target_address() const {
        return m_target_address;
    }

    /*!
     * @brief setter for target_address attribute
     *
     * @param target_address of type string
     */
    void set_target_address(const OptionalField<std::string>& target_address) {
        m_target_address = target_address;
    }

    /*!
     * @brief return an object target_iqn of class string
     *
     * @return the target_iqn value
     */
    const OptionalField<std::string>& get_target_iqn() const {
        return m_target_iqn;
    }

    /*!
     * @brief setter for target_port attribute
     *
     * @param target_port of type string
     */
    void set_target_port(const OptionalField<std::string>& target_port) {
        m_target_port = target_port;
    }

    /*!
     * @brief return an object target_port of class string
     *
     * @return the target_port value
     */
    const OptionalField<std::string>& get_target_port() const {
        return m_target_port;
    }

    /*!
     * @brief setter for target_iqn attribute
     *
     * @param target_iqn of type string
     */
    void set_target_iqn(const OptionalField<std::string>& target_iqn) {
        m_target_iqn = target_iqn;
    }

    /*!
     * @brief return an object target_lun of class TargetLuns
     *
     * @return the target_lun value
     */
    const TargetLuns& get_target_lun() const {
        return m_target_lun;
    }

    /*!
     * @brief setter for target_lun attribute
     *
     * @param target_lun of type TargetLuns
     */
    void set_target_lun(const TargetLuns& target_lun) {
        m_target_lun = target_lun;
    }

    /*!
     * @brief adds TargetLUN to the target_luns array
     *
     * @param target_lun of type TargetLun
     */
    void add_target_lun(const attribute::TargetLun& target_lun) {
        m_target_lun.add_entry(target_lun);
    }


    /*!
     * @brief setter for target_id (TID) attribute
     *
     * @param target_id of type int32_t
     */
    void set_target_id(const std::int32_t target_id) {
        m_target_id = target_id;
    }


    /*!
     * @brief return a target ID (TID)
     *
     * @return the target_id value
     */
    std::int32_t get_target_id() const {
        return m_target_id;
    }

    /*!
     * @brief return an object authentication_method of class string
     *
     * @return the authentication_method value
     */
    const OptionalField<std::string>& get_authentication_method() const {
        return m_authentication_method;
    }

    /*!
     * @brief setter for authentication_method attribute
     *
     * @param authentication_method of type string
     */
    void set_authentication_method(const OptionalField<std::string>& authentication_method) {
        m_authentication_method = authentication_method;
    }

    /*!
     * @brief return an object chap_username of class string
     *
     * @return the chap_username value
     */
    const OptionalField<std::string>& get_chap_username() const {
        return m_chap_username;
    }

    /*!
     * @brief setter for chap_username attribute
     *
     * @param chap_username of type string
     */
    void set_chap_username(const OptionalField<std::string>& chap_username) {
        m_chap_username = chap_username;
    }

    /*!
     * @brief return an object chap_secret of class string
     *
     * @return the chap_secret value
     */
    const OptionalField<std::string>& get_chap_secret() const {
        return m_chap_secret;
    }

    /*!
     * @brief setter for chap_secret attribute
     *
     * @param chap_secret of type string
     */
    void set_chap_secret(const OptionalField<std::string>& chap_secret) {
        m_chap_secret = chap_secret;
    }

    /*!
     * @brief return an object mutual_chap_username of class string
     *
     * @return the mutual_chap_username value
     */
    const OptionalField<std::string>& get_mutual_chap_username() const {
        return m_mutual_chap_username;
    }

    /*!
     * @brief setter for mutual_chap_username attribute
     *
     * @param mutual_chap_username of type string
     */
    void set_mutual_chap_username(const OptionalField<std::string>& mutual_chap_username) {
        m_mutual_chap_username = mutual_chap_username;
    }

    /*!
     * @brief return an object mutual_chap_secret of class string
     *
     * @return the mutual_chap_secret value
     */
    const OptionalField<std::string>& get_mutual_chap_secret() const {
        return m_mutual_chap_secret;
    }

    /*!
     * @brief setter for mutual_chap_secret attribute
     *
     * @param mutual_chap_secret of type string
     */
    void set_mutual_chap_secret(const OptionalField<std::string>& mutual_chap_secret) {
        m_mutual_chap_secret = mutual_chap_secret;
    }

private:

    OptionalField<std::string> m_initiator_iqn{};
    OptionalField<std::string> m_target_address{};
    OptionalField<std::string> m_target_port{};
    OptionalField<std::string> m_target_iqn{};
    std::int32_t m_target_id{};
    TargetLuns m_target_lun{};
    OptionalField<std::string> m_authentication_method{};
    OptionalField<std::string> m_chap_username{};
    OptionalField<std::string> m_chap_secret{};
    OptionalField<std::string> m_mutual_chap_username{};
    OptionalField<std::string> m_mutual_chap_secret{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

