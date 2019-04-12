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
 * @file err_code.hpp
 *
 * @brief LLDP exception error codes
 * */

#pragma once

namespace agent {
namespace network {
namespace api {
namespace lldp {
namespace exception {

/*!
 * @enum ErrCode
 * @brief LLDP exception error codes
 *
 * @var ErrCode::NOT_SPECIFIED
 * Error code isn't specified
 *
 * @var ErrCode::EOM
 * End Of Message
 *
 * @var ErrCode::NO_CONNECTION
 * NO connection to LLDP agent
 *
 * @var ErrCode::NO_INFO
 * Requested information isn't available
 *
 * */
enum class ErrCode {
    /* internal errors */
    NOT_SPECIFIED,
    EOM,
    /* user errors */
    NO_CONNECTION,
    NO_INFO
};

}
}
}
}
}
