/*!
 * @brief Definition of all property keys used by agents.
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
 * @file database_keys.hpp
 */

#pragma once

namespace agent_framework {
namespace database {

extern const char CHASSIS_ASSET_TAG_PROPERTY[];
extern const char ENDPOINT_NQN_PROPERTY[];
extern const char ENDPOINT_ROLE_PROPERTY[];
extern const char ZONE_ENDPOINTS_PROPERTY[];

extern const char RDMA_PORT_PROPERTY[];
extern const char UUID_PROPERTY[];
extern const char IPV4_PROPERTY[];
extern const char FPGA_DEVICE_UUID_PROPERTY[];
extern const char ZONES_PROPERTY[];

extern const char PROCESSOR_ERASED_PROPERTY[];
extern const char PROCESSORS_PROPERTY[];

}
}