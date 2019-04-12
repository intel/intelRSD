/*!
 * @brief GetCpuAndMemoryTemperature IPMI command request and response.
 *
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
 * @file ipmi/command/sdv/nm/get_cpu_and_memory_temperature.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "reading.hpp"
#include <array>


namespace ipmi {
namespace command {
namespace sdv {

namespace request {
constexpr const std::uint8_t CPU_INDICATOR_MASK = 0xF0;
constexpr const std::uint8_t CPU_INDICATOR_BITS = 0x0F;
constexpr const std::uint8_t CPU_SET_MASK = 0xCF;
constexpr const std::uint8_t CPU_SET_BITS = 0x03;
constexpr const std::uint8_t CPU_SET_OFFSET = 4;
constexpr const std::uint8_t MEMORY_CHANNEL_BITS = 0xBF;
constexpr const std::uint8_t MEMORY_CHANNEL_OFFSET = 6;
constexpr const std::uint8_t FRAME_FORMAT_OFFSET = 7;
constexpr const std::uint8_t FRAME_FORMAT_BITS = 0x7F;

enum CPU_MASK : std::uint8_t {
    CPU0 = 1u,
    CPU1 = 1u << 1,
    CPU2 = 1u << 2,
    CPU3 = 1u << 3
};

enum CPU_SET : std::uint8_t {
    CPU0_TO_3 = 0u,
    CPU4_TO_7 = 1u
};

enum class CHANNEL_SET {
    CHANNELS_0_TO_3,
    CHANNELS_4_TO_7
};

enum MEMORY_MASK : std::uint16_t {
    CHANNEL0_DIMM0 = 1u,
    CHANNEL0_DIMM1 = 1u << 1,
    CHANNEL0_DIMM2 = 1u << 2,
    CHANNEL0_DIMM3 = 1u << 3,
    CHANNEL1_DIMM0 = 1u << 4,
    CHANNEL1_DIMM1 = 1u << 5,
    CHANNEL1_DIMM2 = 1u << 6,
    CHANNEL1_DIMM3 = 1u << 7,
    CHANNEL2_DIMM0 = 1u << 8,
    CHANNEL2_DIMM1 = 1u << 9,
    CHANNEL2_DIMM2 = 1u << 0xA,
    CHANNEL2_DIMM3 = 1u << 0xB,
    CHANNEL3_DIMM0 = 1u << 0xC,
    CHANNEL3_DIMM1 = 1u << 0xD,
    CHANNEL3_DIMM2 = 1u << 0xE,
    CHANNEL3_DIMM3 = 1u << 0xF,
};


/*!
 * @brief Request message for GetCpuAndMemoryTemperature command.
 */
class GetCpuAndMemoryTemperature : public Request {
public:

    static constexpr const size_t NUM_CPUS = 4;

    /*!
     * @brief Constructor.
     */
    GetCpuAndMemoryTemperature();


    GetCpuAndMemoryTemperature(const GetCpuAndMemoryTemperature&) = default;


    GetCpuAndMemoryTemperature(GetCpuAndMemoryTemperature&&) = default;


    virtual ~GetCpuAndMemoryTemperature();


    const char* get_command_name() const override {
        return "GetCpuAndMemoryTemperature";
    }


    /*
     * @brief CPU indicator mask setter.
     * @param cpu_mask Bits 3:0 represent CPUs from which temperature readings are requested
     */
    void set_cpu_mask(std::uint8_t cpu_mask) {
        m_cpus = static_cast<std::uint8_t>((m_cpus & CPU_INDICATOR_MASK) | (cpu_mask & CPU_INDICATOR_BITS));
    }


    /*
     * @brief CPU set selector.
     * @param cpu_set Bits 1:0 represent CPU set which temperature readings are requested
     */
    void select_cpu_set(std::uint8_t cpu_set) {
        m_cpus = static_cast<std::uint8_t>((m_cpus & CPU_SET_MASK) | ((cpu_set & CPU_SET_BITS) << CPU_SET_OFFSET));
    }


    /*
     * @brief Selects memory channels
     * @param channel_set CHANNELS_0_TO_3 or CHANNELS_4_TO_7.
     */
    void select_memory_channels(CHANNEL_SET channel_set) {
        if (CHANNEL_SET::CHANNELS_0_TO_3 == channel_set) {
            m_cpus &= MEMORY_CHANNEL_BITS;
        }
        else if (CHANNEL_SET::CHANNELS_4_TO_7 == channel_set) {
            m_cpus |= (1u << MEMORY_CHANNEL_OFFSET);
        }
    }


    /*
     * @brief Sets standard request frame format - default setting (up to 4 DIMM per channel).
     */
    void set_standard_frame_format() {
        m_cpus &= FRAME_FORMAT_BITS;
    }


    /*
     * @brief Sets extended request frame format (up to 8 DIMM per channel).
     */
    void set_extended_frame_format() {
        m_cpus |= (1u << FRAME_FORMAT_OFFSET);
    }


    /*
     * @brief Checks if extended request frame format is enabled.
     * @return true if extended request frame format is enabled, false otherwise.
     */
    bool is_extended_frame_format() const {
        return m_cpus & (1u << FRAME_FORMAT_OFFSET);
    }

    /*
     * @brief Memory channel and DIMM setter for CPU.
     * @param cpu_idx CPU index in range <0,NUM_CPUS)
     * @param memory_channel_mask Each bit represents memory channels and DIMMs which temperature readings are requested
     */
    void set_memory_channel_mask_for_cpu(int cpu_idx, std::uint32_t memory_channel_mask) {
        m_memory_channel_mask_for_cpu.at(cpu_idx) = memory_channel_mask;
    }

private:
    std::uint8_t m_cpus{0};
    std::array<std::uint32_t, NUM_CPUS> m_memory_channel_mask_for_cpu{{}};

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Response message for GetCpuAndMemoryTemperature command.
 */
class GetCpuAndMemoryTemperature : public Response {
public:

    /*!
     * @brief Constructor.
     */
    GetCpuAndMemoryTemperature();


    GetCpuAndMemoryTemperature(const GetCpuAndMemoryTemperature&) = default;


    GetCpuAndMemoryTemperature(GetCpuAndMemoryTemperature&&) = default;


    virtual ~GetCpuAndMemoryTemperature();


    const char* get_command_name() const override {
        return "GetCpuAndMemoryTemperature";
    }


    using RawReadingValues = std::vector<std::uint8_t>;


    /*!
     * @brief Returns metric reading collection.
     * @return Collection of metric readings.
     */
    const RawReadingValues& get_readings() const {
        return m_raw_readings;
    }


private:
    static constexpr const std::size_t MIN_RESPONSE_SIZE = 1;
    RawReadingValues m_raw_readings{};


    void unpack(const IpmiInterface::ByteBuffer& data) override;


    void throw_error_on_completion_code(CompletionCode completion_code) const override;
};

}

}
}
}
