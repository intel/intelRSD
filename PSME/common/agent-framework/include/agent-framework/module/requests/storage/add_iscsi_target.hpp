/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file requests/storage/get_target_info.hpp
 * @brief Storage AddISCSITarget request
 * */

#pragma once

#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/model/attributes/array.hpp"
#include "agent-framework/module/model/attributes/target_lun.hpp"
#include "agent-framework/module/enum/storage.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/utils/optional_field.hpp"


#include <vector>
#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! AddIscsiTarget request */
class AddIscsiTarget{
public:
    using TargetLuns = attribute::Array<attribute::TargetLun>;

    static std::string get_command() {
        return literals::Command::ADD_ISCSI_TARGET;
    }

    /*!
     * @brief Get target luns
     * @return Target luns
     */
    const TargetLuns& get_target_luns() const {
        return m_target_luns;
    }

    /*!
     * @brief Set target luns
     * @param[in] target_luns Array of Target LUNs
     */
    void set_target_luns(const TargetLuns& target_luns) {
        m_target_luns = target_luns;
    }

    /*!
     * @brief add Target Lun
     * @param target_lun the Target Lun to add
     */
    void add_target_lun(const attribute::TargetLun& target_lun) {
        m_target_luns.add_entry(target_lun);
    }

    /*!
     * @brief Get target iqn
     * @return Target iqn
     */
    const std::string& get_target_iqn() const {
        return m_target_iqn;
    }

    /*!
     * @brief Set target iqn
     * @param target_iqn the target IQN to set
     */
    void set_target_iqn(const std::string& target_iqn) {
        m_target_iqn = target_iqn;
    }

    /*!
     * @brief Get initiator iqn
     * @return Initiator iqn
     */
    const OptionalField<std::string>& get_initiator_iqn() const {
        return m_initiator_iqn;
    }

    /*!
     * @brief Set initiator iqn
     * @param[in] initiator_iqn the initiator IQN to set
     */
    void set_initiator_iqn(const OptionalField<std::string>& initiator_iqn) {
        m_initiator_iqn = initiator_iqn;
    }

    /*!
     * @brief Get authentication method
     * @return authentication method
     * */
    const OptionalField<enums::TargetAuthenticationMethod>& get_authentication_method() const {
        return m_authentication_method;
    }

    /*!
     * @brief Set authentication method
     * @param[in] authentication_method the authentication method
     * */
    void set_authentication_method(const OptionalField<enums::TargetAuthenticationMethod>& authentication_method) {
        m_authentication_method = authentication_method;
    }

    /*!
     * @brief Get CHAP username
     * @return CHAP username
     * */
    const OptionalField<std::string>& get_chap_username() const {
        return m_chap_username;
    }

    /*!
     * @brief Set CHAP username
     * @param[in] chap_username the CHAP username
     * */
    void set_chap_username(const OptionalField<std::string>& chap_username) {
        m_chap_username = chap_username;
    }

    /*!
     * @brief Get CHAP secret
     * @return CHAP secret
     * */
    const OptionalField<std::string>& get_chap_secret() const {
        return m_chap_secret;
    }

    /*!
     * @brief Set CHAP secret
     * @param[in] chap_secret the CHAP secret
     * */
    void set_chap_secret(const OptionalField<std::string>& chap_secret) {
        m_chap_secret = chap_secret;
    }

    /*!
     * @brief Get Mutual CHAP username
     * @return Mutual CHAP username
     * */
    const OptionalField<std::string>& get_mutual_chap_username() const {
        return m_mutual_chap_username;
    }

    /*!
     * @brief Set Mutual CHAP username
     * @param[in] mutual_chap_username the Mutual CHAP username
     * */
    void set_mutual_chap_username(const OptionalField<std::string>& mutual_chap_username) {
        m_mutual_chap_username = mutual_chap_username;
    }

    /*!
     * @brief Get Mutual CHAP secret
     * @return Mutual CHAP secret
     * */
    const OptionalField<std::string>& get_mutual_chap_secret() const {
        return m_mutual_chap_secret;
    }

    /*!
     * @brief Set Mutual CHAP secret
     * @param[in] mutual_chap_secret the Mutual CHAP secret
     * */
    void set_mutual_chap_secret(const OptionalField<std::string>& mutual_chap_secret) {
        m_mutual_chap_secret = mutual_chap_secret;
    }

    /*!
     * @brief Get OEM data
     * @return OEM data object
     */
    const attribute::Oem& get_oem() const {
        return m_oem;
    }

    /*!
     * @brief Set OEM data
     * @param[in] oem the OEM data to set
     * */
    void set_oem(const attribute::Oem& oem) {
        m_oem = oem;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create AddIscsiTarget form Json
     *
     * @param[in] json the input argument
     *
     * @return new AddIscsiTarget
     */
    static AddIscsiTarget from_json(const Json::Value& json);

    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                literals::IscsiTarget::TARGET_IQN, VALID_JSON_STRING,
                literals::IscsiTarget::INITIATOR_IQN, VALID_OPTIONAL(VALID_JSON_STRING),
                literals::IscsiTarget::TARGET_LUNS, VALID_ARRAY_OF(VALID_ATTRIBUTE(TargetLunSchema)),
                literals::IscsiTarget::AUTHENTICATION_METHOD, VALID_NULLABLE(VALID_ENUM(agent_framework::model::enums::TargetAuthenticationMethod)),
                literals::IscsiTarget::CHAP_USERNAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
                literals::IscsiTarget::CHAP_SECRET, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
                literals::IscsiTarget::MUTUAL_CHAP_USERNAME, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
                literals::IscsiTarget::MUTUAL_CHAP_SECRET, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_STRING)),
                literals::IscsiTarget::OEM, jsonrpc::JSON_OBJECT,
                nullptr
        };
        return procedure;
    }

private:
    class TargetLunSchema {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                literals::TargetLun::LUN, VALID_NUMERIC_TYPED(UINT32),
                literals::TargetLun::LOGICAL_DRIVE, VALID_UUID,
                nullptr
            };
            return procedure;
        }
    };

    OptionalField<std::string> m_initiator_iqn{};
    std::string m_target_iqn{};
    TargetLuns m_target_luns{};
    OptionalField<enums::TargetAuthenticationMethod> m_authentication_method{};
    OptionalField<std::string> m_chap_username{};
    OptionalField<std::string> m_chap_secret{};
    OptionalField<std::string> m_mutual_chap_username{};
    OptionalField<std::string> m_mutual_chap_secret{};
    attribute::Oem m_oem{};
};

}
}
}
