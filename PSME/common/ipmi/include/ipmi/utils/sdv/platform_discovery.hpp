/*!
 * @brief Function discovers BMC interface with particular platform support.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ipmi/utils/sdv/platform_discovery.hpp
 */

#pragma once

namespace ipmi {
namespace sdv {

/*!
 * @brief Discovers BMC interface.
 * @param rsp GetDeviceId command response.
 * @return Bmc interface enum with support for this platform.
 */
ipmi::command::generic::BmcInterface platform_discovery(const ipmi::command::generic::response::GetDeviceId& rsp);

}
}
