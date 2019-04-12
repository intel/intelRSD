/*!
 * @brief Rmm certificate manager interface
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file certificate_manager_interface.hpp
 */

#pragma once



#include "certificate_data.hpp"
#include "ipmi/ipmi_controller.hpp"



namespace agent {
namespace rmm {

/*!
 * @brief Certificate management class for handling PODM and RMM certificates.
 *
 * This code is thread safe: it can be called concurrently from different threads of
 * execution without any issues.
 */
class CertificateManagerInterface {
public:
    using CertificateType = CertificateData::CertificateType;

    CertificateManagerInterface() {}
    virtual ~CertificateManagerInterface() {}

    /*!
     * @brief Set certificate file path.
     *
     * For security reasons it is only allowed that file_path is absolute and
     * references a regular file, i.e. is not a device or a link.
     *
     * @param cert_type Type of certificate.
     * @param file_path Certificate absolute file path.
     */
    virtual void set_cert_file_path(CertificateData::CertificateType cert_type,
                                    const std::string& file_path) = 0;

    /*!
     * @brief Update certificate for a particular client.
     *
     * This method communicates with a client and checks validity of its certificate. In
     * case it is outdated, it sends it a new one. The certificate file can be swapped in
     * runtime so that its change does not require restart of the service.
     *
     * @param certificate_type Type of certificate to update.
     * @param controller IPMI controller to communicate with a client.
     * @param bridge_info Bridge information for clients that need bridged connection.
     */
    virtual void update_certificate(CertificateData::CertificateType certificate_type,
                                    const ipmi::IpmiController::Ptr controller,
                                    const ipmi::BridgeInfo& bridge_info) = 0;

};

}
}
