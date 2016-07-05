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
 * @file utils.hpp
 * @brief tgt utils
*/

#ifndef ISCSI_TGT_UTILS_HPP
#define	ISCSI_TGT_UTILS_HPP

namespace agent {
namespace storage {
namespace iscsi {
namespace tgt {

/*! Request lld max size */
static constexpr unsigned LLD_SIZE = 64;

/*! TGT available operations */
enum class Operation {
    NEW,
    DELETE,
    SHOW,
    BIND,
    UNBIND,
    UPDATE,
    STATS,
    START,
    STOP
};

/*! TGT available modes */
enum class Mode {
    SYSTEM,
    TARGET,
    DEVICE,
    PORTAL,
    LLD,
    SESSION,
    CONNECTION,
    ACCOUNT
};

}
}
}
}

#endif	/* ISCSI_TGT_UTILS_HPP */

