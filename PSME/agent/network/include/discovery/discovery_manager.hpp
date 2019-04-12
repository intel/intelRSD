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
 * @file discovery_manager.hpp
 *
 * @brief Initial discovery implementation.
 * */

#pragma once

#include <string>

namespace agent {
namespace network {
namespace discovery {

/*! @brief Implementation of initial discovery. */
class DiscoveryManager final {
public:

    /*! @brief Default constructor. */
    DiscoveryManager();

    /*! @brief Copy constructor */
    DiscoveryManager(const DiscoveryManager&) = default;

    /*! @brief Assignment operator */
    DiscoveryManager& operator=(const DiscoveryManager&) = default;

    /*! @brief Default destructor. */
    virtual ~DiscoveryManager();

    virtual void discovery(const std::string& uuid);

    /*!
     * @brief Get switch port prefix name
     *
     * @return Port prefix.
     * */
    static const std::string get_port_prefix() {
        return std::string("swp");
    }
};

}
}
}

