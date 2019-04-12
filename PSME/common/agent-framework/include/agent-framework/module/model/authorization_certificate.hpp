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
 * @file authorization_certificate.hpp
 * @brief AuthorizationCertificate model interface
 * */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/chassis.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! AuthorizationCertificate Class */
class AuthorizationCertificate : public Resource {
public:
    explicit AuthorizationCertificate(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);
    ~AuthorizationCertificate();

    AuthorizationCertificate(const AuthorizationCertificate&) = default;
    AuthorizationCertificate& operator=(const AuthorizationCertificate&) =
        default;
    AuthorizationCertificate(AuthorizationCertificate&&) = default;
    AuthorizationCertificate& operator=(AuthorizationCertificate&&) = default;

    /*!
     * @brief Create an object from Json
     *
     * @param json Json object
     * @return Authorization Certificate object
     */
    static AuthorizationCertificate from_json(const json::Json& json);

    /*!
     * @brief Convert an object to json
     * @return Json object
     */
    json::Json to_json() const;

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return AuthorizationCertificate::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return AuthorizationCertificate::component;
    }

    /*!
     * @brief Get certificate
     *
     * @return certificate
     * */
    const std::string& get_certificate() const {
        return m_certificate;
    }

    /*!
     * @brief Set certificate
     *
     * @param[in] certificate certificate as base64 encoded string
     * */
    void set_certificate(const std::string& certificate) {
        m_certificate = certificate;
    }

    /*!
     * @brief Get encoding method
     *
     * @return encoding method
     * */
    enums::EncodingMethod get_encoding_method() const {
        return m_encoding_method;
    }

    /*!
     * @brief Set encoding method
     *
     * @param[in] encoding_method encoding method
     * */
    void set_encoding_method(enums::EncodingMethod encoding_method) {
        m_encoding_method = encoding_method;
    }

    /*!
     * @brief Get certificate hash
     *
     * @return certificate hash
     * */
    const std::string& get_certificate_hash() const {
        return m_certificate_hash;
    }

    /*!
     * @brief Set certificate hash
     *
     * @param hash hash
     * */
    void set_certificate_hash(const std::vector<uint8_t>& hash) {
        std::stringstream stream;
        for (auto& number : hash) {
            stream << std::hex << std::setw(2) << std::setfill('0') <<
                static_cast<uint32_t>(number);
        }
        m_certificate_hash.append(stream.str());
    }
    /*!
     * @brief Set hash method
     *
     * @param[in] hash_method Hash method
     * */
    void set_hash_method(enums::HashMethod hash_method) {
        m_hash_method = hash_method;
    }

    /*!
     * @brief Get hash method
     *
     * @return hash method
     * */
    enums::HashMethod get_hash_method() const {
        return m_hash_method;
    }

private:

    std::string m_certificate{};
    enums::EncodingMethod  m_encoding_method{enums::EncodingMethod::BASE64};
    std::string m_certificate_hash{};
    enums::HashMethod m_hash_method{enums::HashMethod::MD5};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};
}
}

