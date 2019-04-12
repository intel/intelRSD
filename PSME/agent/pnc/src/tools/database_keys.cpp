/*!
 * @brief Definition of all property keys used by pnc agent.
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
 * @file database_keys.cpp
 */

#include "tools/database_keys.hpp"



namespace agent {
namespace pnc {
namespace tools {
namespace db_keys {

extern const char DEVICE[]        = "device_uuid";
extern const char DEVICE_PORT[]   = "device_port_uuid";
extern const char ENDPOINTS[]     = "endpoints_uuids";
extern const char ZONES[]         = "zones_uuids";
extern const char ENDPOINT_ROLE[] = "endpoint_role";
extern const char PORT[]          = "port_uuid";

} // namespace db_keys
} // namespace tools
} // namespace pnc
} // namespace agent