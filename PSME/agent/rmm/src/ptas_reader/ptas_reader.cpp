/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file rmm/ptas_reader/ptas_reader.cpp
 */

#include "ptas_reader/ptas_reader.hpp"

#include "logger/logger_factory.hpp"

/* a lot of tricks to deal with problems of the included file */
extern "C" {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include "ptas_reader/ptas_sdk/ptas_sdk.h"
#pragma GCC diagnostic pop

}

using namespace agent::rmm::ptas;

namespace {

constexpr unsigned PTAS_MAX_FANS = PTASDICT_MAXFAN;
constexpr unsigned PTAS_MAX_TRAYS = PTASDICT_MAXTRAY;
constexpr unsigned PTAS_MAX_SLEDS = SLED_PERTRAY;

}

std::atomic<unsigned> PtasReader::sequence_number{0};

std::atomic<bool> PtasReader::is_initialized{false};

PtasReader::PtasReader(int port, int timeout, int fan_count, int drawer_count) {

    m_statistics = std::make_shared<PTAS_Statistic>();
    if (!PtasReader::is_initialized) {
        /* PTAS requires number of trays (3 entries per tray), we model one drawer to consist of two trays */
          PTAS_config_t config = {(unsigned short)(port), (unsigned char)(fan_count), (unsigned char)(2*drawer_count)};
          libptas_init(&config, NULL);
        PtasReader::is_initialized = true;
    }
    m_timeout = timeout;
}

void PtasReader::reset_data(int global_power) {
    m_is_data_valid = false;
    m_statistics->glb_power_sum = global_power;
    m_statistics->sequence_num = ++sequence_number;
    for (unsigned i = 0; i < PTAS_MAX_FANS; ++i) {
        m_statistics->fan_rpm[i].present = 0;
        m_statistics->fan_rpm[i].rpm = 0;
    }
    for (unsigned i = 0; i < PTAS_MAX_TRAYS; ++i) {
        m_statistics->tray_statistic[i].present = 0;
        m_statistics->tray_statistic[i].switch_power = 0;
        for (unsigned j = 0; j < PTAS_MAX_SLEDS; ++j) {
            m_statistics->tray_statistic[i].sled_statis[j].present = 0;
            m_statistics->tray_statistic[i].sled_statis[j].temperature = 0;
        }
    }
}

void PtasReader::add_fan_data(unsigned slot_id, unsigned fan_rpm) {
    if (slot_id < PTAS_MAX_FANS) {
        m_statistics->fan_rpm[slot_id].present = 1;
        m_statistics->fan_rpm[slot_id].rpm = fan_rpm;
    }
}

void PtasReader::add_drawer_data(unsigned slot_id, unsigned power, const std::vector<SledData>& sled_data) {
    if (slot_id < PTAS_MAX_TRAYS) {
        m_statistics->tray_statistic[slot_id].present = 1;
        m_statistics->tray_statistic[slot_id].switch_power = power;
        for (const auto& data : sled_data) {
            unsigned sled_slot = std::get<0>(data);
            if (sled_slot < PTAS_MAX_SLEDS) {
                m_statistics->tray_statistic[slot_id].sled_statis[sled_slot].present = 1;
                m_statistics->tray_statistic[slot_id].sled_statis[sled_slot].temperature = std::get<1>(data);
            }
        }
    }
}

bool PtasReader::send_receive_data() {

    PTAS_Output_t output{0, 0, 0.0, 0.0f};
    int response = PTAS_wait_timeout(m_statistics.get(), &output, m_timeout);
    switch (response) {
    case -1:
        log_error("ptas-handler", "PTAS error, cannot get response.");
        return false;
    case 1:
        log_error("ptas-handler", "PTAS timeout, cannot get response.");
        return false;
    case 0:
        log_debug("ptas-handler", "PTAS data read succesfully.");
        break;
    default:
        log_warning("ptas-handler", "Uknown PTAS status code: " << response);
        break;
    }
    log_debug("ptas-handler", "PTAS response: ");
    log_debug("ptas-handler", "\treturn code: " << (int)output.rc);
    log_debug("ptas-handler", "\tvolumetric airflow: " << output.Q);
    log_debug("ptas-handler", "\toutlet temperature: " << output.outlet_temp);

    if (output.sequence_num != sequence_number) {
        log_warning("ptas-handler", "Invalid PTAS sequence number: " << output.sequence_num << ", expected: " << sequence_number);
    }

    m_is_data_valid = true;
    m_volumetric_airflow = output.Q;
    m_outlet_temperature = output.outlet_temp;
    return true;
}
