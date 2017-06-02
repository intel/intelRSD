/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#pragma once

#include "agent-framework/discovery/discovery.hpp"

#include <memory>

namespace ipmi {
class ManagementController;
}

namespace agent {
namespace compute {
namespace discovery {

/*!
 * @brief Implementation of initial discovery.
 *
 * Gets component id, translates it to IP and port number.
 * After that performs discovery of the module.
 */
class DiscoveryManager : public ::agent_framework::discovery::Discovery {
private:
    std::unique_ptr<ipmi::ManagementController> m_mc;

public:

    /*!
     * @brief Default constructor.
     * Initializes ipmitool based implementation of ManagementController
     */
    DiscoveryManager();

    /*!
     * @brief Constructor.
     * Sets the ipmi::ManagementController implementation.
     *
     * @param mc Unique pointer of IPMI management controller
     */
    DiscoveryManager(std::unique_ptr<ipmi::ManagementController>& mc);


    /*!
     * @brief Default destructor.
     */
    ~DiscoveryManager();

    virtual void discovery(const std::string& module) override;

protected:

    /* Returns std::tuple<status, region data stored in std::vector> */
    std::tuple<bool, std::vector<std::uint8_t>> read_mdr_data(std::uint16_t bytes_to_read);

    /* Returns std::tuple<status, region size to read> */
    std::tuple<bool, std::uint16_t> get_mdr_data_region();
};

}
}
}
