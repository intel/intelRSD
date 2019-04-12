/*!
 * @brief Definition of SPDK Error class.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file spdk_error.hpp
 */

#pragma once

#include "agent-framework/exceptions/gami_exception.hpp"



namespace agent_framework {
namespace exceptions {

/*! @brief SPDK error exception class */
class SpdkError final : public GamiException {
public:

    /*!
     * @brief Constructor
     *
     * @param[in] msg Error message
     */
    explicit SpdkError(const std::string& msg) : GamiException(ErrorCode::SPDK, msg) { }


    SpdkError(const SpdkError&) = default;


    SpdkError& operator=(const SpdkError&) = default;


    SpdkError(SpdkError&&) = default;


    SpdkError& operator=(SpdkError&&) = default;


    /*! @brief Destructor */
    virtual ~SpdkError();
};

}
}

