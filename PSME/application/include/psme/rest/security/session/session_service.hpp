/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file session_service.hpp
 *
 * @brief Declaration of SessionService class
 *
 * */


#pragma once



#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include "agent-framework/threading/thread.hpp"



namespace psme {
namespace rest {
namespace security {
namespace session {

/*!
 * @brief SessionService declaration
 */
class SessionService : public agent_framework::threading::Thread {
public:

    /*! @brief Start REST session service */
    void start();


    /*! @brief Stop REST session service */
    void stop();


    /*! @brief Execute function */
    void execute() { handle_sessions(); }


    /*! @brief Destructor */
    ~SessionService();


private:
    void handle_sessions();


    const std::chrono::milliseconds m_session_clenup_interval{1000};
};

}
}
}
}
