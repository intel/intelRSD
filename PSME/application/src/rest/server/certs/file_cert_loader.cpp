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
 * @file file_cert_loader.cpp
 *
 * */

#include "psme/rest/server/certs/file_cert_loader.hpp"
#include "logger/logger_factory.hpp"

using namespace psme::rest::server;

FileCertLoader::FileCertLoader(const std::string& certs_dir)
    : CertLoader{certs_dir} { }

std::string FileCertLoader::load_ca_cert() {
    return read_file(CertLoader::get_certs_dir()
                    + CertLoader::CA_CERT_FILENAME);
}
