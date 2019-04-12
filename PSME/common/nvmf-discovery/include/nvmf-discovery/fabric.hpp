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
 * @file nvmf-discovery/fabric.hpp
 */

#pragma once

#include "rdma/fabric.h"
#include "rdma/fi_domain.h"
#include "rdma/fi_cm.h"
#include "rdma/fi_eq.h"
#include "rdma/fi_rma.h"

#include <memory>
#include <chrono>

namespace nvmf_discovery {

/* forward declaration */
struct Interface;

constexpr uint32_t FI_VER = FI_VERSION(1, 0);

template<typename T>
struct FidDeleter {
    void operator()(T* fid) const {
        fi_close(&fid->fid);
    }
};

struct FiInfoDeleter {
    void operator()(fi_info* info) const {
        fi_freeinfo(info);
    }
};

template<typename T>
using FidPtr = std::unique_ptr<T, FidDeleter<T>>;

using FiInfo = std::unique_ptr<fi_info, FiInfoDeleter>;
using FidFabric = FidPtr<fid_fabric>;
using FidDomain = FidPtr<fid_domain>;
using FidPep = FidPtr<fid_pep>;
using FidEp = FidPtr<fid_ep>;
using FidEq = FidPtr<fid_eq>;
using FidCq = FidPtr<fid_cq>;
using FidMr = FidPtr<fid_mr>;

using TimeoutMs = std::chrono::duration<int, std::milli>;

std::string get_cq_error_string(fid_cq* cq, ssize_t ec);
std::string get_eq_error_string(fid_eq* eq, ssize_t ec);

/*!
 * Converts error code to string representation
 * @param err_code error code
 */
std::string fi_error_to_string(int err_code);

/*!
 * Obtains fabric interface information structure describing hints
 *
 * @param provider OFI provider name
 */
FiInfo prepare_fabric_hints(const std::string& provider);

/*!
 * Obtains fabric interface information structure.
 *
 * A wrapper on fi_getinfo.
 *
 * @param iface Interface description
 * @param hints specifies criteria for selecting the returned fabric information
 * @param flags operation flags for the fi_getinfo call
 * @return FiInfo information about available fabric services for reaching node or service,
 * subject to any provided hints.
 * @throws FabricException in case of error including when there is no FiInfo available to return
 */
FiInfo get_fi_info(const Interface& iface, fi_info& hints, uint64_t flags);

/*!
 * Registers memory buffer with domain
 * @param buf buffer to be registered
 * @param size size of the buffer
 * @param access Access flags
 * @return Pointer to fid_mr structure
 */
FidMr register_memory_buffer(fid_domain& domain, void* buf, size_t size, uint64_t access);

}
