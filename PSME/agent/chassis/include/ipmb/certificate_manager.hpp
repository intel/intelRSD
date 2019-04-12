/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file rmm/certificate_manager.hpp
 * @brief Certificate manager.
 * */


#pragma once
#include <ipmb/certificate.hpp>
#include <agent-framework/generic/singleton.hpp>
#include <map>

namespace agent {
namespace chassis {
namespace ipmb {


/*!
 * @brief Certificate manager class
 * */
class CertificateManager : public agent_framework::generic::Singleton<CertificateManager> {
public:

    virtual ~CertificateManager();

    /*!
     * @brief Get certificate
     *
     * WARNING!
     * If certain certificate type does not exist in m_certificates
     * empty entry will be created. Certificate type and correctness
     * should be checked after calling the command.
     *
     * @param type Type of requested certificate
     *
     * @return Reference to requested certificate
     * */
    Certificate& get_certificate(Certificate::Type type) {
        return m_certificates[type];
    }

private:

    std::map<Certificate::Type, Certificate> m_certificates{};

};

}
}
}


