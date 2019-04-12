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
 * @file cert_loader.cpp
 *
 * */

#include "psme/rest/server/certs/cert_loader.hpp"
#include "logger/logger_factory.hpp"

#include <fstream>

using namespace psme::rest::server;

constexpr const char CertLoader::SERVER_KEY_FILENAME[];
constexpr const char CertLoader::SERVER_CERT_FILENAME[];
constexpr const char CertLoader::CA_CERT_FILENAME[];

std::string psme::rest::server::read_file(const std::string& filename) {
    std::ifstream file(filename.c_str());

    if (file) {
        std::string content{};
        content.assign(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
        log_info("rest", "Loaded : " << filename);
        return content;
    }

    throw std::runtime_error("Can't open file: " + filename);
}

CertLoader::CertLoader(const std::string& certs_dir)
    : m_certs_dir{certs_dir} {
    if ('/' != m_certs_dir.back()) {
        m_certs_dir += '/';
    }
}

CertLoader::~CertLoader() { }

Certs CertLoader::load_certs(bool ca_cert_required) {
    std::string ca_cert{};
    if (ca_cert_required) {
        ca_cert = load_ca_cert();
    }

    return Certs{
        read_file(m_certs_dir + SERVER_KEY_FILENAME),
        read_file(m_certs_dir + SERVER_CERT_FILENAME),
        ca_cert
    };
}
