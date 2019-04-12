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
 * @file cert_loader.hpp
 *
 * @brief Interface for certificate loader
 * */

#pragma once
#include <tuple>

namespace psme {
namespace rest {
namespace server {

/*!
 * @brief Tuple consisting of server private key, server certificate
 *        and CA certificate.
 */
using Certs = std::tuple<const std::string, /* server key */
                         const std::string, /* server cert */
                         const std::string  /* ca cert */>;

/*!
 * @brief Helper function loading file content to string.
 */
std::string read_file(const std::string& filename);

/*!
 * @brief Certificate Loader interface.
 *
 * Responsible for retrieving components (server key, server certificate , CA certificate)
 * required to initialize HTTPS Connector.
 * */
class CertLoader {
public:
    /*! @brief Server's private key filename */
    static constexpr const char SERVER_KEY_FILENAME[] = "server.key";
    /*! @brief Server's certificate filename */
    static constexpr const char SERVER_CERT_FILENAME[] = "server.crt";
    /*! @brief CA's certificate filename */
    static constexpr const char CA_CERT_FILENAME[] = "ca.crt";

    /*! @brief Constructor
     * @param certs_dir Directory in which certificates/key files are stored.
     **/
    explicit CertLoader(const std::string& certs_dir);

    /*! @brief Destructor */
    virtual ~CertLoader();

    /*! @brief Default copy constructor */
    CertLoader(const CertLoader&) = default;

    /*! @brief Default assignment operator */
    CertLoader& operator=(const CertLoader&) = default;

    /*! @brief Loads Certs components.
     *
     * @return Certs tuple with loaded components.
     * */
    Certs load_certs(bool ca_cert_required);

    /*! @brief Getter for directory containing Certs components.
     * @return Directory path containing Certs components.
     */
    const std::string& get_certs_dir() const {
        return m_certs_dir;
    }

protected:
    /*!
     * @brief Loads Certificate Authority's certificate used to verify REST server
     * clients.
     */
    virtual std::string load_ca_cert() = 0;

private:
    std::string m_certs_dir{};
};

} /* namespace server */
} /* namespace rest */
} /* namespace psme */

