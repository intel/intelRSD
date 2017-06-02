/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file registration_server.hpp
 * @brief Registration Server declaration.
 * */

#include "psme/command/command_json_server.hpp"
#include "logger/logger_factory.hpp"

#include <jsonrpccpp/server.h>

#include <memory>
#include <string>
#include <functional>

/*! PSME namespace */
namespace psme {
/*! Application namespace */
namespace app {
/*! Registration namespace */
namespace registration {

using namespace command;
using namespace jsonrpc;
using namespace std;

/*! @brief Registration Server */
class RegistrationServer : public CommandJsonServer {
public:
    /*!
     * @brief Default constructor.
     *
     * @param   connector   Server connector
     * @param   type        JSON RPC version
     * */
    RegistrationServer(AbstractServerConnector& connector,
            serverVersion_t type = JSONRPC_SERVER_V2) :
        CommandJsonServer(connector, type){}

    /*! @brief Destructor */
    virtual ~RegistrationServer();
};

}
}
}
