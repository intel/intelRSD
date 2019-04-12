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
 * @file mhd_options.hpp
 *
 * @brief Declaration of Options for Libmicrohttpd based Connector.
 * */

#pragma once



#include "psme/rest/server/connector/connector_options.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include <memory>



/*! forward declarations */
struct MHD_OptionItem;
class MHDConnectorOptionsImpl;

namespace psme {
namespace rest {
namespace server {

/*!
 * @class MHDConnectorOptions
 * @brief Set of options used to initialize
 * <a href="https://www.gnu.org/software/libmicrohttpd">Libmicrohttpd</a>
 * based Connector: MHDConnector, MHDSSLConnector
 * */
class MHDConnectorOptions final {
public:
    /*!
     * @brief Constructor.
     * @param[in] options ConnectorOptions translated to MHDConnectorOptions
     * understood by <a href="https://www.gnu.org/software/libmicrohttpd">Libmicrohttpd</a>
     * MHDConnector and MHDSSLConnector.
     * */
    explicit MHDConnectorOptions(const ConnectorOptions& options);


    MHDConnectorOptions(const MHDConnectorOptions&) = default;


    MHDConnectorOptions& operator=(const MHDConnectorOptions&) = default;


    MHDConnectorOptions(MHDConnectorOptions&&) = default;


    MHDConnectorOptions& operator=(MHDConnectorOptions&&) = default;


    /*!
     * @return OR-ed combination of MHD_FLAG values for MHDConnector initialization.
     * See <a href="https://www.gnu.org/software/libmicrohttpd">Libmicrohttpd</a>
     * for details.
     */
    unsigned int get_flags() const;


    /*!
     * @return MHD_OptionItem array for MHDConnector initialization.
     * See <a href="https://www.gnu.org/software/libmicrohttpd">Libmicrohttpd</a>
     * for details.
     */
    MHD_OptionItem* get_options_array();


private:
    std::shared_ptr<MHDConnectorOptionsImpl> m_options_impl;
};

}
}
}

