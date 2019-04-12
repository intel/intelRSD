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
 * @file nvmf-discovery/fabric.cpp
 */

#include "nvmf-discovery/fabric.hpp"
#include "nvmf-discovery/interface.hpp"
#include "nvmf-discovery/fabric_exception.hpp"
#include "logger/logger_factory.hpp"
#include <sstream>
#include <cstring>

namespace nvmf_discovery {

std::string fi_error_to_string(int err_code) {
    return fi_strerror(err_code >= 0 ? err_code : -err_code);
}

std::string get_cq_error_string(fid_cq* cq, ssize_t ec) {
    fi_cq_err_entry entry = {};
    std::stringstream error{};

    if (ec < 0) {
        ec = -ec;
    }

    if (ec != FI_EAVAIL) {
        error << "fi_cq_sread error: " << fi_error_to_string(int(ec)) << "(" << ec << ") ";
    }

    auto rc = fi_cq_readerr(cq, &entry, 0);
    if (rc < 0) {
        error << "fi_cq_readerr error: " << fi_error_to_string(int(rc)) << "(" << rc << ")";
    }
    else {
        error << "fi_cq_readerr provider error: " <<
                fi_cq_strerror(cq, entry.prov_errno, entry.err_data, nullptr, 0)
                << "(" << entry.prov_errno << ") error: " << fi_error_to_string(entry.err);
    }

    return error.str();
}

std::string get_eq_error_string(fid_eq* eq, ssize_t ec) {
    fi_eq_err_entry entry = {};
    std::stringstream error{};

    if (ec < 0) {
        ec = -ec;
    }

    if (ec != FI_EAVAIL) {
        error << "fi_eq_sread error: " << fi_error_to_string(int(ec)) << "(" << ec << ") ";
    }

    auto rc = fi_eq_readerr(eq, &entry, 0);
    if (rc < 0) {
        error << "fi_eq_readerr error: " << fi_error_to_string(int(rc)) << "(" << rc << ")";
    }
    else {
        error << "fi_eq_readerr provider_error: " <<
                fi_eq_strerror(eq, entry.prov_errno, entry.err_data, nullptr, 0)
                << "(" << entry.prov_errno << ") error: " << fi_error_to_string(entry.err);
    }

    return error.str();
}

FiInfo prepare_fabric_hints(const std::string& provider) {
    FiInfo hints{fi_allocinfo()};
    if (!hints) {
        throw std::bad_alloc();
    }

    hints->caps = FI_MSG | FI_RMA;
    hints->domain_attr->mr_mode = FI_MR_BASIC;
    hints->domain_attr->threading = FI_THREAD_SAFE;
    hints->mode = FI_CONTEXT | FI_LOCAL_MR | FI_RX_CQ_DATA;
    hints->addr_format = FI_SOCKADDR;
    // reliable, connection-oriented data transfer service
    // with flow control that maintains message boundaries
    hints->ep_attr->type = FI_EP_MSG;
    // NOTE: prov_name is deallocated by fi_freeinfo
    auto prov_name = strdup(provider.c_str());
    if (!prov_name) {
        throw std::bad_alloc();
    }
    hints->fabric_attr->prov_name = prov_name;

    return hints;
}

FiInfo get_fi_info(const Interface& iface, fi_info& hints, uint64_t flags) {
    fi_info* info{};
    auto ret = fi_getinfo(FI_VER, iface.traddr.c_str(), iface.trsvcid.c_str(), flags, &hints, &info);
    if (ret) {
        throw FabricException("fi_getinfo error: " + fi_error_to_string(ret), ret);
    }
    return FiInfo(info);
}

FidMr register_memory_buffer(fid_domain& domain, void* buf, size_t size, uint64_t access) {
    fid_mr* mem_reg{};
    auto ret = fi_mr_reg(&domain, buf, size, access, 0, 0, 0, &mem_reg, nullptr);
    if (ret) {
        throw FabricException("fi_mr_reg error: " + fi_error_to_string(int(ret)), ret);
    }
    return FidMr{mem_reg};
}

}
