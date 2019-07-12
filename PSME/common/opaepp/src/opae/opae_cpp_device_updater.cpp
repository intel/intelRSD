/*!
 * @brief Class for updating FPGAs with green bitstreams.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file opae_cpp_device_updater.cpp
 */


#include "logger/logger.hpp"
#include "opaepp/opae/opae_cpp_device_updater.hpp"
#include "opaepp/opae/opae_cpp_device_reader.hpp"
#include "opaepp/device.hpp"
#include "opae/cxx/core/properties.h"
#include "opae/cxx/core/token.h"
#include "opae/cxx/core/handle.h"
#include "uuid/uuid.hpp"

#include <chrono>
#include <thread>



using namespace opae::fpga::types;

namespace opaepp {

void OpaeCppDeviceUpater::update(uint8_t bus, uint8_t device, uint8_t function,
                                 const uint8_t* bitstream, size_t bitstream_size,
                                 uint32_t slot) {

    auto property = opae::fpga::types::properties::get();

    property->bus = bus;
    property->device = device;
    property->function = function;

    // Reconfigure slot works on FPGA devices (accelerator is the slot)
    property->type = FPGA_DEVICE;

    std::vector<properties::ptr_t> properties{property};

    // On one PCIe address there will be only one FPGA
    auto tokens = token::enumerate(properties);

    for (int i = 0; i < 3; i++) {
        if (tokens.empty()) {
            log_error("agent", "FPGA not detected, sleeping for 10 sec");
            std::this_thread::sleep_for(std::chrono::seconds(10));
            tokens = token::enumerate(properties);
        }
    }

    if (tokens.empty()) {
        log_error("agent", "FPGA not detected after 30 sec");
        throw std::runtime_error("FPGA not detected after 30 sec");
    }

    auto token = tokens.front();

    auto handle = handle::open(token, 0);

    if (handle) {
        handle->reconfigure(slot, bitstream, bitstream_size, FPGA_RECONF_FORCE);
    }
    else {
        log_error("agent", "Could not open device on: " << int(bus) << int(device) << int(function));
        throw std::runtime_error("Could not open device");
    }

    log_info("agent", "Successfully updated bitstream on the FPGA");
}
}

