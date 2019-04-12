/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file responder.hpp
 * @brief Responder (BMC) MUX handler interface
 *
 */

#pragma once
#include "ipmb/mux.hpp"

namespace agent {
namespace chassis {
namespace ipmb {

/*! General class for multiplex connectivity with I2C device */
class Responder : public Mux {
public:
    /*! Default constructor */
    Responder() = default;

    /*! disable copy constructor */
    Responder(const Responder&) = delete;

    /*! disableassignment operator */
    Responder& operator=(const Responder&) = delete;

    /*! Mode or type of MUX connection -- responder */
    byte_vec_t mode() const;
};

}
}
}


