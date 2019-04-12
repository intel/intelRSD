/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file network_error.hpp
 *
 * @brief Declaration of network error interface
 * */

#pragma once



#include "agent-framework/exceptions/gami_exception.hpp"



namespace agent_framework {
namespace exceptions {

/*! @brief Network exception class */
class NetworkError final : public GamiException {
public:

    /*!
     * @brief Constructor
     *
     * @param[in] msg Error message
     * */
    explicit NetworkError(const std::string& msg) :
        GamiException(ErrorCode::NETWORK, msg) {}


    NetworkError(const NetworkError&) = default;


    NetworkError& operator=(const NetworkError&) = default;


    NetworkError(NetworkError&&) = default;


    NetworkError& operator=(NetworkError&&) = default;


    /*! @brief Destructor */
    ~NetworkError();
};

} // namespace exceptions
} // namespace agent_framework

