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
 * @file command/stubs/get_authorization_certificate.cpp
 * @brief GetAuthorizationCertificate stub implementation
 * */

#include "agent-framework/command/chassis/get_authorization_certificate.hpp"
#include "configuration/configuration.hpp"
#include "base64/base64.hpp"

extern "C" {
#include <md5/md5.h>
}

#include <fstream>

using namespace agent_framework::command;
using namespace agent_framework::model;

namespace {
void make_md5_hash(const std::vector<std::uint8_t>& input,
                    std::vector<std::uint8_t>& output) {
    output.resize(16);

    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, input.data(), input.size());
    MD5_Final(output.data(), &context);
}
}

class GetAuthorizationCertificate : public agent_framework::command::chassis::
                                    GetAuthorizationCertificate {
public:
    using agent_framework::command::chassis::GetAuthorizationCertificate::execute;

    void execute(const Request& request, Response& response) {

        log_debug(GET_LOGGER("rpc"), "GetAuthorizationCertificate with parameters: certificate "
                                     << request.get_certificate());

        auto& config = configuration::Configuration::get_instance().to_json();

        if (!config["certs"].is_object() && !config["certs"]["ca"].is_string()) {
            THROW(::agent_framework::exceptions::CertificateError, "agent",
                                "Invalid config [certs][ca] path");
        }

        auto filename = config["certs"]["ca"].as_string();

        std::ifstream file;
        file.open(filename, std::ios::in);

        if (!file.is_open()) {
            THROW(::agent_framework::exceptions::CertificateError, "agent",
                                "Cannot open file [" + filename + "]");
        }

        std::vector<std::uint8_t> bytes(
                    (std::istreambuf_iterator<char>(file)),
                                        std::istreambuf_iterator<char>());

        file.close();

        std::vector<std::uint8_t> hash{};
        ::make_md5_hash(bytes, hash);

        AuthorizationCertificate certificate{};
        certificate.set_certificate(base64::Base64::encodeUrl(bytes));
        certificate.set_certificate_hash(hash);
        response.set_certificate(std::move(certificate));

    }

    ~GetAuthorizationCertificate();
};

GetAuthorizationCertificate::~GetAuthorizationCertificate() { }

static Command::Register<GetAuthorizationCertificate> g("Stubs");

