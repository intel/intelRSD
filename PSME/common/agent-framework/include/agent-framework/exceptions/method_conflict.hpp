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
 *
 * @file method_conflict.hpp
 *
 * @brief Declaration of Method Conflict Json RPC exception
 * */

#pragma once



#include "agent-framework/exceptions/gami_exception.hpp"



namespace agent_framework {
namespace exceptions {

/*! @brief MethodConflict exception class */
class MethodConflict final : public GamiException {
public:

    /*! @brief Exception JSON field */
    static const constexpr char TASK[] = "task";

    /*!
     * @brief Constructor
     *
     * @param[in] msg Error message
     */
    explicit MethodConflict(const std::string& msg, const std::string& task_uuid);

    /*!
     * @brief Get task_uuid value from JSON data.
     * @param data Exception's JSON data.
     * @return The conflicting task's uuid as string.
     * */
    static std::string get_task_uuid(const json::Json& data);


    /*! @brief Destructor */
    ~MethodConflict();
};

}
}

