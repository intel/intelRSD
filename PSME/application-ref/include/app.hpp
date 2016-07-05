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
 *
 * @file app.hpp
 *
 * @brief Declares App class wrapping main program logic.
 * */

#ifndef PSME_APP_APP_HPP
#define PSME_APP_APP_HPP

#include "psme/eventing/eventing_server.hpp"
#include "psme/registration/registration_server.hpp"
#include "psme/rest-ref/rest_server.hpp"
#include "psme/rest-ref/eventing/event_service.hpp"

#include <memory>

/*! Forward declarations */
namespace json {
    class Value;
}

namespace jsonrpc {
    class HttpServer;
}

/*! Psme namespace */
namespace psme {
/*! Application namespace */
namespace app {

/*!
 * @brief Application class wrapping main program logic.
 *
 * Responsible for application initialization and execution.
 */
class App final {
public:
    /*!
     * @brief Constructor
     * @param argc number of arguments passed to program
     * @param argv array with program arguments
     */
    App(int argc, const char** argv);

    /*!
     * @brief Destructor
     *
     * Responsible for resource cleanup. Stops any running servers.
     */
    ~App();

    /*!
     * @brief Starts servers initialized in init method
     * and waits for interrupt signal.
     */
    void run();
private:
    /*!
     * @brief Initialization method
     *
     * Responsible for initialization of loggers, registration server,
     * eventing server and rest server. Does not start the mentioned servers.
     */
    void init();
    void init_logger();
    void init_registration_server();
    void init_eventing_server();
    void init_rest_event_service();
    void init_subscription_config();
    void init_rest_server();
    void cleanup();
    void statics_cleanup();

    const json::Value& m_configuration;
    std::unique_ptr<psme::app::eventing::EventingServer> m_eventing_server{};
    std::unique_ptr<jsonrpc::HttpServer> m_reg_server_connector{};
    std::unique_ptr<psme::app::registration::RegistrationServer> m_registration_server{};
    std::unique_ptr<psme::rest::server::RestServer> m_rest_server{};
    std::unique_ptr<psme::rest::eventing::EventService> m_rest_event_service{};
};

}
}

#endif /* PSME_APP_APP_HPP */
