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
 * @file del_lag_message.hpp
 *
 * @brief Switch Del Lag configuration
 * */

#pragma once
#include "lag_message.hpp"

namespace agent {
namespace network {
namespace tools {
namespace netlink {

/*! Delete Lag Message */
class DelLagMessage: public LagMessage {
public:

    /*!
     * @brief Default constructor.
     *
     * @param[in] lag Lag interface name.
     * */
    DelLagMessage(const std::string& lag);

    /*!
     * @brief Default destructor.
     * */
    virtual ~DelLagMessage();
};

}
}
}
}

