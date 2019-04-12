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
 * @file certificate_error.hpp
 * @brief Declaration of Certificate error interface
 * */

#pragma once



#include "agent-framework/exceptions/gami_exception.hpp"



namespace agent_framework {
namespace exceptions {

/*! @brief Certificate error exception class */
class CertificateError final : public GamiException {
public:

    /*!
     * @brief Constructor
     *
     * @param[in] msg Error message
     */
    explicit CertificateError(const std::string& msg) :
        GamiException{ErrorCode::CERTIFICATE, msg} {}


    CertificateError(const CertificateError&) = default;


    CertificateError& operator=(const CertificateError&) = default;


    CertificateError(CertificateError&&) = default;


    CertificateError& operator=(CertificateError&&) = default;


    /*! @brief Destructor */
    ~CertificateError();
};

}
}

