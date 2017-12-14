/*!
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file rmm/discovery_thread.cpp
 */


#include "discovery_thread.hpp"

#include "discovery/discovery_manager.hpp"
#include "loader/ipmi_config.hpp"
#include "ptas_reader/ptas_reader.hpp"
#include "event_collector/event_collector.hpp"
#include "certificate_management/certificate_manager.hpp"
#include "telemetry/rmm_telemetry_service.hpp"
#include "discovery_thread.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include <logger/logger_factory.hpp>

#include <chrono>

using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::rmm::discovery;
using namespace agent::rmm::loader;
using namespace agent::rmm;

namespace {

constexpr unsigned int PTAS_TIMEOUT_SECONDS = 1;
constexpr unsigned int PTAS_CONNECTION_PORT = 5678;
constexpr unsigned int PTAS_FANS_SUPPORTED = 6;
constexpr unsigned int PTAS_TRAYS_SUPPORTED = 8;

}

DiscoveryThread::~DiscoveryThread() {
    m_is_running = false;
    m_condition.notify_one();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void DiscoveryThread::task() {
    log_info(GET_LOGGER("discovery-thread"), "DiscoveryThread: starting");

    helpers::DiscoveryContext dc;
    dc.net_reader = std::make_shared<agent::rmm::net_reader::NetReader>();
    dc.ptas = std::make_shared<agent::rmm::ptas::PtasReader>(
        PTAS_CONNECTION_PORT, PTAS_TIMEOUT_SECONDS, PTAS_FANS_SUPPORTED, PTAS_TRAYS_SUPPORTED);
    dc.event_collector = std::make_shared<agent::rmm::event_collector::EventCollector>();
    dc.certificate_manager = std::make_shared<agent::rmm::CertificateManager>();
    dc.telemetry_service = std::make_shared<RmmTelemetryService>();


    DiscoveryManager dm{};

    auto slots = IpmiConfig::get_instance().get_controller_slots();

    while (m_is_running) {
        std::unique_lock<std::mutex> lk(m_mutex);
        if (!m_is_discovery_finished
            || m_condition.wait_for(lk, std::chrono::seconds(m_interval)) == std::cv_status::timeout) {

            log_debug(GET_LOGGER("discovery-thread"), "DiscoveryThread: discovery loop started!");
            log_debug(GET_LOGGER("discovery-thread"), "DiscoveryThread: discovering rack");
            try {
                dm.rack_discovery(dc);
                log_debug(GET_LOGGER("discovery-thread"), "DiscoveryThread: rack discovery finished!");
            }
            catch (std::exception& e) {
                log_debug(GET_LOGGER("discovery-thread"), "DiscoveryThread: rack discovery FAILED: " << e.what());
            }

            for (const IpmiConfig::LocationOffset& slot : slots) {
                log_debug(GET_LOGGER("discovery-thread"), "DiscoveryThread: discovering slot " << unsigned(slot) << " started");
                try {
                    dc.ipmi = agent::rmm::loader::IpmiConfig::get_instance().get_controller(slot);
                    dm.zone_discovery(dc, slot);
                    log_debug(GET_LOGGER("discovery-thread"), "DiscoveryThread: discovering slot " << unsigned(slot) << " finished");
                }
                catch (std::exception& e) {
                    log_debug(GET_LOGGER("discovery-thread"), "DiscoveryThread: discovering slot " << unsigned(slot)
                        << " FAILED: " << e.what());
                }
            }
            log_debug(GET_LOGGER("discovery-thread"), "DiscoveryThread: discovery loop finished!");
            m_is_discovery_finished = true;
            dc.event_collector->send();
        }
    }

    log_debug(GET_LOGGER("discovery-thread"), "DiscoveryThread stopped");
}

void DiscoveryThread::start() {
    m_is_running = true;
    m_thread = std::thread(&DiscoveryThread::task, this);
}
