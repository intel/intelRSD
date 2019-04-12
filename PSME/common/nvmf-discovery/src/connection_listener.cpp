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
 * @file nvmf-discovery/connection_listener.cpp
 */

#include "nvmf-discovery/connection_listener.hpp"
#include "nvmf-discovery/fabric_exception.hpp"
#include "nvmf-discovery/interface.hpp"
#include "logger/logger_factory.hpp"

#include <stdexcept>

namespace nvmf_discovery {

FiInfo obtain_connection_listener_provider(const Interface& iface) {
    auto hints = prepare_fabric_hints(iface.provider);
    return get_fi_info(iface, *hints, FI_SOURCE);
}

ConnectionListener::ConnectionListener(FiInfo provider)
    : m_provider{std::move(provider)} {

    fid_fabric* fab{};
    auto ret = fi_fabric(m_provider->fabric_attr, &fab, NULL);
    m_fabric.reset(fab);
    if (ret) {
        throw FabricException("fi_fabric error: " + fi_error_to_string(ret), ret);
    }

    fid_domain* dom{};
    ret = fi_domain(m_fabric.get(), m_provider.get(), &dom, NULL);
    m_domain.reset(dom);
    if (ret) {
        throw FabricException("fi_domain error: " + fi_error_to_string(ret), ret);
    }
}

void ConnectionListener::listen() {
    fi_eq_attr eq_attr = {};
    fid_eq* event_queue{};
    auto ret = fi_eq_open(m_fabric.get(), &eq_attr, &event_queue, NULL);
    m_peq.reset(event_queue);
    if (ret) {
        throw FabricException("fi_eq_open error: " + fi_error_to_string(ret), ret);
    }
    fid_pep* passive_endpoint{};
    ret = fi_passive_ep(m_fabric.get(), m_provider.get(), &passive_endpoint, this);
    m_pep.reset(passive_endpoint);
    if (ret) {
        throw FabricException("fi_passive_ep error: " + fi_error_to_string(ret), ret);
    }
    ret = fi_pep_bind(m_pep.get(), &m_peq->fid, 0);
    if (ret) {
        throw FabricException("fi_pep_bind error: " + fi_error_to_string(ret), ret);
    }
    ret = fi_listen(m_pep.get());
    if (ret) {
        throw FabricException("fi_listen error: " + fi_error_to_string(ret), ret);
    }
}

FiInfo ConnectionListener::wait_for_connection_event(TimeoutMs timeout) noexcept {
    FiInfo info{};
    fi_eq_cm_entry entry{};
    uint32_t event{};

    auto ret = fi_eq_sread(m_peq.get(), &event, &entry, sizeof(entry), timeout.count(), 0);

    if (ret == sizeof(entry)) {
        if (FI_CONNREQ == event) {
            info.reset(entry.info);
        }
        else {
            log_error("nvmf-discovery", "Unexpected CM event " <<  event);
        }
    }
    else if (ret != -EAGAIN && ret != -EINTR) {
        log_error("nvmf-discovery", "Host connection failed: " << get_eq_error_string(m_peq.get(), ret));
    }

    return info;
}

}
