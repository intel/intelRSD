/*!
 * @brief Rmm certificate manager class interface
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
 * @file certificate_manager.hpp
 */

#pragma once


#include "certificate_manager_interface.hpp"


namespace agent {
namespace rmm {

/*!
 * @brief Certificate management class for handling PODM and RMM certificates.
 *
 * This code is thread safe: it can be called concurrently from different threads of
 * execution without any issues.
 */
class CertificateManager : public CertificateManagerInterface {
public:
    CertificateManager();
    virtual ~CertificateManager();

    /*!
     * @brief Set certificate file path.
     *
     * For security reasons it is only allowed that file_path is absolute and
     * references a regular file, i.e. is not a device or a link.
     *
     * @param cert_type Type of certificate.
     * @param file_path Certificate absolute file path.
     */
    void set_cert_file_path(CertificateType cert_type, const std::string& file_path) override;


    /*!
     * @brief Update certificate for a particular client.
     *
     * This method communicates with a client and checks validity of its certificate. In
     * case it is outdated, it sends it a new one. The certificate file can be swapped in
     * runtime so that its change does not require restart of the service as long as the
     * certificate path is the same as that provided in the configuration file.
     *
     * @param certificate_type Type of certificate to update.
     * @param controller IPMI controller to communicate with a client.
     * @param bridge_info Bridge information for clients that need bridged connection.
     */
    void update_certificate(CertificateType certificate_type,
                            const ipmi::IpmiController::Ptr controller,
                            const ipmi::BridgeInfo& bridge_info = {}) override;


protected:
    /*!
     * @brief Get reference to certificate data
     *
     * Certificate data is locked when the reference is valid.
     * @return RAII reference to locked certificate data
     */
    CertificateDataReference get_certificate(CertificateType certificate_type) const;

    /*!
     * @brief Calculate certificate hash.
     *
     * This method is responsible for calculating certificate hash and updating the
     * corresponding certificate_data container in place.
     *
     * @param certificate_data Certificate data object for which the hash should be computed.
     */
    void calculate_certificate_hash(CertificateData& certificate_data);


    /*!
     * @brief Read certificate file and update certificate data.
     *
     * This method reads contents of a file and places it in the certificate data container.
     *
     * @param certificate_data Certificate data object in which the certificate should be placed.
     * @throws logical_error if any error is found
     * @return true if file was (externally) modified.
     */
    bool read_cert_file_and_update(CertificateData& certificate_data);


    /*!
     * @brief Get certificate hash from client.
     *
     * The value returned by this function should compared with the hash held by the
     * RMM agent to determine if the client certificate needs to be updated.
     *
     * @param certificate_type Type of certificate.
     * @param controller IPMI controller to communicate with a client.
     * @param bridge_info Bridge information for clients that need bridged connection.
     * @return Retrieved certificate hash.
     */
    CertificateData::CertificateHashType
    get_certificate_hash_from_client(CertificateType certificate_type,
                                     const ipmi::IpmiController::Ptr controller,
                                     const ipmi::BridgeInfo& bridge_info = {});


    /*!
     * @brief Sends certificate to client.
     *
     * Due to limited IPMB frame length, the certificate might be sent in chunks, but this behavior is
     * invisible to the client. In case of IPMB line being used or any other transimission errors,
     * a chunk is retransimtted up to MAX_RETRIES time before finally giving up. An exception
     * derived from std::exception is then thrown and should be handled bu upper layers. If this method returns,
     * however, it is guaranteed to successfully transmitted the certificate.
     *
     * @param certificate_type Type of certificate.
     * @param controller IPMI controller to communicate with a client.
     * @param bridge_info Bridge information for clients that need bridged connection.
     */
    void send_certificate_to_client(CertificateType certificate_type,
                                    const ipmi::IpmiController::Ptr controller,
                                    const ipmi::BridgeInfo& bridge_info = {});


private:
    std::vector<std::shared_ptr<CertificateData>> certificates;

    mutable std::mutex m_mutex{};
};

}
}
