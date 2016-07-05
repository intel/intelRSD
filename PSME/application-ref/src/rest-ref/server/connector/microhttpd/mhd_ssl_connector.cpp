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
 * @file mhd_ssl_connector.cpp
 *
 * @brief  SSL connector with certificate verification functions. This file is only
 *         compiled if ENABLE_HTTPS is set.
 *
 * */

#include "psme/rest-ref/server/connector/microhttpd/mhd_ssl_connector.hpp"
extern "C" {
#include "microhttpd.h"
#include <gnutls/gnutls.h>
}
#include "logger/logger_factory.hpp"
#include <sstream>

namespace {

std::string certificate_verification_status_to_string(unsigned int status,
        gnutls_certificate_type_t type) {
    std::ostringstream str;

#if (GNUTLS_VERSION_MAJOR >= 3 && GNUTLS_VERSION_MINOR >= 1 && GNUTLS_VERSION_PATCH >= 4)
    gnutls_datum_t out;
    if (0 == gnutls_certificate_verification_status_print(status, type, &out, 0)) {
        str << out.data;
        gnutls_free(out.data);
    }
#else
    (void) type;
    if (status == 0) {
        str << "The certificate is trusted. ";
    }
    else {
        str << "The certificate is NOT trusted. ";
    }
#endif
    return str.str();
}

int verify_certificate(gnutls_session_t tls_session) {
    unsigned int cert_status;

    if (nullptr == tls_session) {
        return GNUTLS_E_CERTIFICATE_ERROR;
    }

    auto ret = gnutls_certificate_verify_peers2(tls_session, &cert_status);
    if (ret < 0) {
        log_error(GET_LOGGER("rest"),
                "Failed to verify certificate: " << gnutls_strerror(ret));
        return ret;
    }

    if (cert_status != 0) {
        auto cert_type = gnutls_certificate_type_get(tls_session);
        auto status_as_string = certificate_verification_status_to_string(
                                                    cert_status, cert_type);
        log_error(GET_LOGGER("rest"),
                "Failed to verify certificate: " << status_as_string);
        return GNUTLS_E_CERTIFICATE_ERROR;
    }

    return GNUTLS_E_SUCCESS;
}
} // namespace

using namespace psme::rest::server;

MHDSSLConnector::MHDSSLConnector(const ConnectorOptions& options)
    : MHDConnector(options) { }

MHDSSLConnector::~MHDSSLConnector() { }

bool MHDSSLConnector::is_access_allowed(struct MHD_Connection* connection) {
    if (get_options().is_client_cert_required()) {
        auto* ci = MHD_get_connection_info(connection,
                                           MHD_CONNECTION_INFO_GNUTLS_SESSION);
        if (nullptr != ci) {
            gnutls_session_t tls_session =
                                static_cast<gnutls_session_t>(ci->tls_session);
            return (0 == verify_certificate(tls_session));
        } else {
            log_debug(GET_LOGGER("rest"), "Can't obtain ssl session handle.");
            return false;
        }
    }
    return true;
}
