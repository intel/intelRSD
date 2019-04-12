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
 * @file mhd_ssl_connector.hpp
 *
 * @brief Declaration of HTTPS connector based on
 *  <a href="https://www.gnu.org/software/libmicrohttpd">Libmicrohttpd</a>.
 *        This file is only included if ENABLE_HTTPS is set.
 * */

#pragma once



#include "mhd_connector.hpp"



namespace psme {
namespace rest {
namespace server {

/*!
 * @brief HTTPS connector implementation based on
 * <a href="https://www.gnu.org/software/libmicrohttpd">Libmicrohttpd</a>.
 * */
class MHDSSLConnector : public MHDConnector {
public:
    /*!
     * @brief Constructor
     * @param[in] options ConnectorOptions for Connector initialization.
     */
    MHDSSLConnector(const ConnectorOptions& options);


    /*! @brief Destructor */
    ~MHDSSLConnector();


    bool is_access_allowed(struct MHD_Connection* connection) override;


private:
    MHDSSLConnector(const MHDSSLConnector&) = delete;


    MHDSSLConnector& operator=(const MHDSSLConnector&) = delete;
};

}
}
}

