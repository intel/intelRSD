/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @file processor.hpp
 * @brief Processor model interface
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_PROCESSOR_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_PROCESSOR_HPP

#include "agent-framework/module/resources.hpp"
#include "agent-framework/module/oem_data.hpp"

namespace agent_framework {
namespace generic {

class Processor : public Resources {
public:
    Processor() = default;
    Processor(const Processor&) = default;
    Processor& operator=(const Processor&) = default;
    Processor(Processor&&) = default;
    Processor& operator=(Processor&&) = default;
    ~Processor();

    /*! Class representing CPUID generic
     *  for GetProcessorInfo Response */
    class CpuId {
        std::string m_vendor_id{};
        std::uint32_t m_numeric_id{};
        std::uint32_t m_family{};
        std::uint32_t m_model{};
        std::uint32_t m_step{};
        std::uint32_t m_microcode_info{};
    public:
        const std::string& get_vendor_id() const {
            return m_vendor_id;
        }

        std::uint32_t get_numeric_id() const {
            return m_numeric_id;
        }

        std::uint32_t get_family() const {
            return m_family;
        }

        std::uint32_t get_model() const {
            return m_model;
        }

        std::uint32_t get_step() const {
            return m_step;
        }

        std::uint32_t get_microcode_info() const {
            return m_microcode_info;
        }

        /*!
        * @brief Set vendor id for response
        * @param[in] vendor_id  Vendor id
        * */
        void set_vendor_id(const std::string& vendor_id) {
            m_vendor_id = vendor_id;
        }

        /*!
         * @brief Set numeric id for response
         * @param[in] numeric_id Numeric id
         * */
        void set_numeric_id(std::uint32_t numeric_id) {
            m_numeric_id = numeric_id;
        }

        /*!
         * @brief Set processor family for response
         * @param[in] family Processor family
         * */
        void set_family(std::uint32_t family) {
            m_family = family;
        }

        /*!
         * @brief Set processor model for response
         * @param[in] model Processor model
         * */
        void set_model(std::uint32_t model) {
            m_model = model;
        }

        /*!
         * @brief Set processor step for response
         * @param[in] step Processor step
         * */
        void set_step(std::uint32_t step) {
            m_step = step;
        }

        /*!
         * @brief Set microcode info for response
         * @param[in] microcode_info Microcode info
         * */
        void set_microcode_info(std::uint32_t microcode_info) {
            m_microcode_info = microcode_info;
        }
    };

    std::uint32_t get_max_speed_mhz() const {
        return m_max_speed_mhz;
    }

    std::uint32_t get_total_cores() const {
        return m_total_cores;
    }

    std::uint32_t get_enabled_cores() const {
        return m_enabled_cores;
    }

    std::uint32_t get_total_threads() const {
        return m_total_threads;
    }

    std::uint32_t get_enabled_threads() const {
        return m_enabled_threads;
    }

    const std::string& get_socket() const {
        return m_socket;
    }

    const std::string& get_processor_type() const {
        return m_processor_type;
    }

    const std::string& get_processor_architecture() const {
        return m_processor_architecture;
    }

    const std::string& get_instruction_set() const {
        return m_instruction_set;
    }

    const std::string& get_manufacturer() const {
        return m_manufacturer;
    }

    const std::string& get_model() const {
        return m_model;
    }

    const generic::OEMData& get_oem_data() const {
        return m_oem;
    }

    const CpuId& get_cpu_id() const {
        return m_cpu_id;
    }

    /*!
     * @brief Set socket ("CPU 1", ...)
     * @param[in] socket Socket
     * */
    void set_socket(const std::string& socket) {
        m_socket = socket;
    }

    /*!
     * @brief Set processor type (CPU, GPU, FPGA, ...)
     * @param[in] processor_type Processor type
     * */
    void set_processor_type(const std::string& processor_type) {
        m_processor_type = processor_type;
    }

    /*!
     * @brief Set processor architecture
     * @param[in] processor_architecture Processor architecture
     * */
    void set_processor_architecture(const std::string& processor_architecture) {
        m_processor_architecture = processor_architecture;
    }

    /*!
     * @brief Set instruction set
     * @param[in] instruction_set Instruction set
     * */
    void set_instruction_set(const std::string& instruction_set) {
        m_instruction_set = instruction_set;
    }

    /*!
     * @brief Set manufacturer
     * @param[in] manufacturer Manufacturer
     * */
    void set_manufacturer(const std::string& manufacturer) {
        m_manufacturer = manufacturer;
    }

    /*!
     * @brief Set model
     * @param[in] model Model
     * */
    void set_model(const std::string& model) {
        m_model = model;
    }

    /*!
     * @brief Set processor max speed in MHz
     * @param[in] max_speed_mhz Processor max speed in MHz
     * */
    void set_max_speed_mhz(std::uint32_t max_speed_mhz) {
        m_max_speed_mhz = max_speed_mhz;
    }

    /*!
     * @brief Set processor total cores
     * @param[in] total_cores Processor total cores
     * */
    void set_total_cores(std::uint32_t total_cores) {
        m_total_cores = total_cores;
    }

    /*!
     * @brief Set processor enabled cores
     * @param[in] enabled_cores Processor enabled cores
     * */
    void set_enabled_cores(std::uint32_t enabled_cores) {
        m_enabled_cores = enabled_cores;
    }

    /*!
     * @brief Set processor total threads
     * @param[in] total_threads Processor total threads
     * */
    void set_total_threads(std::uint32_t total_threads) {
        m_total_threads = total_threads;
    }

    /*!
     * @brief Set processor enabled threads
     * @param[in] enabled_threads Processor enabled threads
     * */
    void set_enabled_threads(std::uint32_t enabled_threads) {
        m_enabled_threads = enabled_threads;
    }

    /*!
     * @brief Set CPUID for response
     * @param cpu_id CpuId object
     */
    void set_cpu_id(const CpuId& cpu_id) {
        m_cpu_id = cpu_id;
    }

    /*!
     * @brief Set OEM specific data for response
     * @param[in] oem OEM specific data
     * */
    void set_oem(const generic::OEMData& oem) {
        m_oem = oem;
    }

private:
    std::uint32_t m_max_speed_mhz{};
    std::uint32_t m_total_cores{};
    std::uint32_t m_enabled_cores{};
    std::uint32_t m_total_threads{};
    std::uint32_t m_enabled_threads{};
    CpuId m_cpu_id{};
    generic::OEMData m_oem{};
    std::string m_socket{};
    std::string m_processor_type{};
    std::string m_processor_architecture{};
    std::string m_instruction_set{};
    std::string m_manufacturer{};
    std::string m_model{};
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_NEIGHBOR_INFO_HPP */
