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
 * @file iscsi_errors.hpp
 * @brief iSCSI TGT Errors interface
*/

#pragma once

#include "tgt/response.hpp"

#include <cstdint>
#include <string>

namespace agent {
namespace storage {
namespace utils {

/*! Class represents iSCSI TGT errors */
class IscsiErrors {
public:

    /*!
     * @brief Return error as string for given error code
     * @param ec Error code
     * @return Error description
     */
    static std::string get_error_message(const std::uint32_t ec);


    /*!
     * @brief Return error as string for given error type
     * @param error Error type
     * @return Error description
     */
    static std::string get_error_message(const tgt::ErrorType error);


    /*!
     * @brief Throws an exception for given error code
     * @param ec Error code
     * @param message Additional message to log
     * */
    static void throw_exception(const std::uint32_t ec,
                                const std::string& message = {});


    /*!
     * @brief Throws an exception for given error type
     * @param error Error type
     * @param message Additional message to log
     * */
    static void throw_exception(const tgt::ErrorType error,
                                const std::string& message = {});

};

}
}
}
