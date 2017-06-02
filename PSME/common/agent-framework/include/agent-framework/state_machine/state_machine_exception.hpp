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
 * @file state_machine_exception.hpp
 * @brief %StateMachine error exception.
 * */

#pragma once
#include <exception>

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace state_machine {

/*!
 * @brief %StateMachine error exception.
 */
class StateMachineError : public std::exception {
    const char * _msg = nullptr;
    StateMachineError();
    StateMachineError & operator=(const StateMachineError &) = delete;
public:
    /*!
     * @brief Class constructor.
     * @param msg Error message.
     */
    StateMachineError(const char * msg) noexcept : _msg(msg) {}

    /*!
     * @brief Class conversion constructor.
     */
    StateMachineError(const StateMachineError & error)  { _msg = error._msg; }

    /*!
     * @brief Returns error message.
     * @return Error message.
     */
    const char * what() const noexcept { return _msg; }

    /*!
     * @brief Class destructor.
     */
    ~StateMachineError();
};

}
}


