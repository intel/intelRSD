/*!
 * @brief Constants for RSD extensions IPMI commands.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file constants.hpp
 */


#pragma once



#include <cstdint>



namespace ipmi {
namespace command {
namespace sdv {
namespace constants {

constexpr std::size_t GROUP_EXTENSION_ID_OFFSET = 1;

static constexpr std::size_t DISABLE_TPM = 0x00;

}
}
}
}
