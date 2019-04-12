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
 *
 * @file rmm_cert_loader.hpp
 *
 * @brief Rmm certificate loader
 * */

#pragma once
#include "cert_loader.hpp"

namespace psme {
namespace rest {
namespace server {

/*!
 * @brief CertLoader implementation which loads server keys from files
 * and CA's certificate by querying Chassis agent via JSONRPC.
 * */
class RMMCertLoader final: public CertLoader {
public:
    /*! @brief Constructor
     * @param certs_dir Directory in which certificates/key files are stored.
     **/
    RMMCertLoader(const std::string& certs_dir);

private:
    std::string load_ca_cert() override;
};

} /* namespace server */
} /* namespace rest */
} /* namespace psme */

