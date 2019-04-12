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
 * @file connector_factory.hpp
 *
 * @brief Declaration of connector factory interface for REST server.
 * */
#pragma once



#include "connector.hpp"



namespace psme {
namespace rest {
namespace server {

/*!
 * @brief Connector factory.
 *
 * Creates new Connector instances based on provided options.
 */
class ConnectorFactory {
public:
    /*! @brief Destructor */
    virtual ~ConnectorFactory();


    /*!
     * @brief Creates new Connector instance.
     * @param[in] options Connector options
     * @param[in] callback Requests handler
     * @return Pointer to created Connector instance.
     */
    ConnectorUPtr create_connector(const ConnectorOptions& options,
                                   const Connector::Callback& callback) const;
};

}
}
}
