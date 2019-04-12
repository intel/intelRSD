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
 * @file cert_manager.hpp
 *
 * @brief Interface for certificate manager
 * */

#pragma once
#include "cert_loader.hpp"

#include <unordered_map>

namespace psme {
namespace rest {
namespace server {

/*!
 * @brief Certificate Manager. Singleton keeping Certs for all HTTPS Connectors.
 *
 * Each Connector is identified by port number on which it is listening.
 * */
class CertManager {
public:
    static CertManager* get_instance();

    virtual ~CertManager();

    /*
     * @brief Retrieves Certs for HTTPS Connector identified by @a server_port.
     * @param server_port HTTPS Connector port
     * @param ca_cert_required Flag indicating if CA's component of Certs should be loaded.
     * @param certs_dir Direcotory path containing SslConfig components.
     */
    void load_certs(const uint16_t server_port,
                         bool ca_cert_required,
                         const std::string& certs_dir);

    /*
     * @brief Getter for server key.
     * @param server_port HTTPS Connector port
     * @return Server private key.
     */
    const char* get_server_key(const uint16_t server_port) const;

    /*
     * @brief Getter for server certificate.
     * @param server_port HTTPS Connector port
     * @return Server key certificate.
     */
    const char* get_server_cert(const uint16_t server_port) const;

    /*
     * @brief Getter for CA's certificate.
     * @param server_port HTTPS Connector port
     * @return CA's certificate.
     */
    const char* get_ca_cert(const uint16_t server_port) const;

    /*
     * @brief Stores Certs for HTTPS Connector identified by @a server_port.
     * @param server_port HTTPS Connector port
     * @param certs ssl components to be stored.
     */
    void add_certs(const uint16_t server_port, const Certs& certs);

    /*
     * @brief Gets Certs for HTTPS Connector identified by @a server_port.
     * @param server_port HTTPS Connector port.
     * @return Certs for requested HTTPS Connector.
     */
    const Certs& get_certs(const uint16_t server_port) const;

private:
    enum CertsKey {
        SERVER_KEY = 0,
        SERVER_CERT = 1,
        CA_CERT = 2
    };

    CertManager() = default;
    CertManager(const CertManager&) = delete;
    CertManager& operator=(const CertManager&) = delete;

    using CertsMap = std::unordered_map<uint16_t, /* server port */
                                          Certs>;
    CertsMap m_certs_map{};
};

} /* namespace server */
} /* namespace rest */
} /* namespace psme */

