/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file lvm_erase_task.cpp
 * @brief LVM erase task implementation
 * */

#include "lvm/lvm_erase_task.hpp"
#include "logger/logger.hpp"

#include <mutex>
#include <random>
#include <algorithm>
#include <fstream>


using namespace storage::lvm;

namespace {
static std::mutex erase_mutex{};
}


LvmEraseTask::LvmEraseTask(const std::string& path, std::uint64_t size, bool fast) :
    m_destination_path(path), m_size(size), m_fast(fast) {}


void LvmEraseTask::operator()() {
    std::lock_guard<std::mutex> lock{erase_mutex};
    log_info("lvm", "Erase task started in " << (m_fast ? "fast" : "slow")
                                             << " mode for " << m_destination_path << " device.");

    try {
        static constexpr int BUFFER_SIZE = 1024 * 1024;
        std::vector<std::uint8_t> buffer(BUFFER_SIZE, 0);

        /* Fill data or leave it empty for fast mode */
        if (!m_fast) {
            auto seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::default_random_engine generator(seed);
            std::uniform_int_distribution<std::uint8_t> distribution(std::numeric_limits<std::uint8_t>::min(),
                                                                     std::numeric_limits<std::uint8_t>::max());
            std::generate(buffer.begin(), buffer.end(), [&distribution, &generator]() {
                return distribution(generator);
            });
        }

        std::fstream file(m_destination_path, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error(std::string{"Unable to open device file for writing: "} + m_destination_path);
        }
        file.seekp(0, std::ios::beg);
        auto remaining_bytes = m_size;

        do {
            auto bytes_to_write = std::min(std::uint64_t(BUFFER_SIZE), remaining_bytes);
            file.write(reinterpret_cast<const char*>(buffer.data()), bytes_to_write);
            if (file.bad()) {
                throw std::runtime_error("Unable to write desired number of bytes!");
            }
            else {
                remaining_bytes -= bytes_to_write;
            }
        }
        while (remaining_bytes > 0);
        file.close();
    }
    catch (const std::exception& error) {
        auto message = std::string{"Could not erase data from device: "} + error.what();
        log_error("lvm", message);
        throw std::runtime_error(message);
    }

    log_info("lvm", "Erase task finished.");
}
