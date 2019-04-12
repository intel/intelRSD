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
 * */

#include "agent-framework/module/model/authorization_certificate.hpp"
#include "agent-framework/module/constants/chassis.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component AuthorizationCertificate::component =
    enums::Component::AuthorizationCertificate;
const enums::CollectionName AuthorizationCertificate::collection_name =
    enums::CollectionName::AuthorizationCertificates;

AuthorizationCertificate::AuthorizationCertificate(
    const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

AuthorizationCertificate::~AuthorizationCertificate() {}

json::Json AuthorizationCertificate::to_json() const {
    json::Json result = json::Json();
    result[literals::AuthorizationCertificate::CERTIFICATE] = get_certificate();
    result[literals::AuthorizationCertificate::ENCODING_METHOD] =
        get_encoding_method().to_string();
    result[literals::AuthorizationCertificate::HASH] = get_certificate_hash();
    result[literals::AuthorizationCertificate::HASH_METHOD] = get_hash_method().to_string();
    result[literals::AuthorizationCertificate::OEM] = get_oem().to_json();
    return result;
}

AuthorizationCertificate AuthorizationCertificate::from_json(const json::Json& json) {
    AuthorizationCertificate certificate{};

    certificate.set_certificate(json[literals::AuthorizationCertificate::CERTIFICATE]);
    certificate.set_encoding_method(
        enums::EncodingMethod::from_string(json[literals::AuthorizationCertificate::ENCODING_METHOD]));

    return certificate;
}
