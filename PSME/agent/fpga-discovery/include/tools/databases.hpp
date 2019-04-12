/*!
 * @brief Definition of different FPGA discovery databases classes.
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file databases.hpp
 */

#pragma once

#include "tools/fpga_discovery_database.hpp"

namespace agent {
namespace fpga_discovery {
namespace tools {

class ZoneDatabase final : public FpgaDiscoveryDatabase {
public:
    /*!
     * @brief Constructor.
     *
     * @param zone Zone's Uuid.
     */
    ZoneDatabase(const Uuid& zone);
};

class FabricDatabase final : public FpgaDiscoveryDatabase {
public:
    /*!
     * @brief Constructor.
     *
     * @param fabric Fabric's Uuid.
     */
    FabricDatabase(const Uuid& fabric);
};

class EndpointDatabase final : public FpgaDiscoveryDatabase {
public:
    /*!
     * @brief Constructor.
     *
     * @param endpoint Endpoint's Uuid.
     */
    EndpointDatabase(const Uuid& endpoint);
};

class VolumeDatabase final : public FpgaDiscoveryDatabase {
public:
    /*!
     * @brief Constructor.
     *
     * @param volume Volumes's Uuid.
     */
    VolumeDatabase(const Uuid& volume);
};

class ChassisDatabase final : public FpgaDiscoveryDatabase {
public:
    /*!
     * @brief Constructor.
     *
     * @param chassis Chassis' Uuid.
     */
    ChassisDatabase(const Uuid& chassis);
};

}
}
}
