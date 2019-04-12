/*!
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
 * @file nvmf-discovery/fabric_exception.cpp
 */

#include "nvmf-discovery/fabric_exception.hpp"

namespace nvmf_discovery {

FabricException::FabricException(const std::string& msg, ssize_t code)
    : runtime_error(msg), m_code(code) {
}

FabricException::~FabricException() {
}

NvmfException::NvmfException(const std::string& msg, nvme::NvmeStatus status)
    : FabricException(msg, 0), m_status(status) {
}

NvmfException::NvmfException(const std::string& msg, ssize_t code, nvme::NvmeStatus status)
    : FabricException(msg, code), m_status(status) {
}

NvmfException::~NvmfException() {
}

}
