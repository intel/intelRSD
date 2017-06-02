/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/compute.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

class Processor : public Resource {
public:
    explicit Processor(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::System);
    Processor(const Processor&) = default;
    Processor& operator=(const Processor&) = default;
    Processor(Processor&&) = default;
    Processor& operator=(Processor&&) = default;
    ~Processor();

    using Capabilities = attribute::Array<std::string>;

    /*!
     * @brief construct an object of class Processor from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed Processor object
     */
    static Processor from_json(const Json::Value& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return Processor::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return Processor::component;
    }

    /*!
     * @brief Get max speed in MHz
     * @return max speed in MHz
     * */
    OptionalField<std::uint32_t> get_max_speed_mhz() const {
        return m_max_speed_mhz;
    }

    /*!
     * @brief Get total cores
     * @return total cores
     *  */
    OptionalField<std::uint32_t> get_total_cores() const {
        return m_total_cores;
    }

    /*!
     * @brief Get enabled cores
     * @return enabled cores
     * */
    OptionalField<std::uint32_t> get_enabled_cores() const {
        return m_enabled_cores;
    }

    /*!
     * @brief Get total threads
     * @return total threads
     * */
    OptionalField<std::uint32_t> get_total_threads() const {
        return m_total_threads;
    }

    /*!
     * @brief Get enabled threads
     * @return enabled threads
     * */
    OptionalField<std::uint32_t> get_enabled_threads() const {
        return m_enabled_threads;
    }

    /*!
     * @brief Get socket
     * @return Socket
     * */
    const OptionalField<std::string>& get_socket() const {
        return m_socket;
    }

    /*!
     * @brief Get processor type
     * @return processor type
     * */
    OptionalField<enums::ProcessorType> get_processor_type() const {
        return m_processor_type;
    }

    /*!
     * @brief Get processor architecture
     * @return processor architecture
     * */
    OptionalField<enums::ProcessorArchitecture> get_processor_architecture() const {
        return m_processor_architecture;
    }

    /*!
     * @brief Get instruction set
     * @return instruction set
     * */

    OptionalField<enums::ProcessorInstructionSet> get_instruction_set() const {
        return m_instruction_set;
    }

    /*!
     * @brief Get manufacturer
     * @return manufacturer
     * */
    const OptionalField<std::string>& get_manufacturer() const {
        return m_manufacturer;
    }

    /*!
     * @brief Get model
     * @return model
     * */
    OptionalField<enums::ProcessorModel> get_model() const {
        return m_model;
    }

    /*!
     * @brief Get model name
     * @return model_name
     * */
    const OptionalField<std::string>& get_model_name() const {
        return m_model_name;
    }

    /*!
     * @brief Get CpuId
     * @return CpucId
     * */
    const attribute::CpuId& get_cpu_id() const {
        return m_cpu_id;
    }

    /*!
     * @brief Set socket ("CPU 1", ...)
     * @param[in] socket Socket
     * */
    void set_socket(const OptionalField<std::string>& socket) {
        m_socket = socket;
    }

    /*!
     * @brief Set processor type (CPU, GPU, FPGA, ...)
     * @param[in] processor_type Processor type
     * */
    void set_processor_type(const OptionalField<enums::ProcessorType> processor_type) {
        m_processor_type = processor_type;
    }

    /*!
     * @brief Set processor architecture
     * @param[in] processor_architecture Processor architecture
     * */
    void set_processor_architecture(const OptionalField<enums::ProcessorArchitecture> processor_architecture) {
        m_processor_architecture = processor_architecture;
    }

    /*!
     * @brief Set instruction set
     * @param[in] instruction_set Instruction set
     * */
    void set_instruction_set(const OptionalField<enums::ProcessorInstructionSet> instruction_set) {
        m_instruction_set = instruction_set;
    }

    /*!
     * @brief Set manufacturer
     * @param[in] manufacturer Manufacturer
     * */
    void set_manufacturer(const OptionalField<std::string>& manufacturer) {
        m_manufacturer = manufacturer;
    }

    /*!
     * @brief Set model
     * @param[in] model Model
     * */
    void set_model(const OptionalField<enums::ProcessorModel> model) {
        m_model = model;
    }

    /*!
     * @brief Set model name
     * @param[in] model_name Model name
     * */
    void set_model_name(const OptionalField<std::string>& model_name) {
        m_model_name = model_name;
    }

    /*!
     * @brief Set processor max speed in MHz
     * @param[in] max_speed_mhz Processor max speed in MHz
     * */
    void set_max_speed_mhz(OptionalField<std::uint32_t> max_speed_mhz) {
        m_max_speed_mhz = max_speed_mhz;
    }

    /*!
     * @brief Set processor total cores
     * @param[in] total_cores Processor total cores
     * */
    void set_total_cores(OptionalField<std::uint32_t> total_cores) {
        m_total_cores = total_cores;
    }

    /*!
     * @brief Set processor enabled cores
     * @param[in] enabled_cores Processor enabled cores
     * */
    void set_enabled_cores(OptionalField<std::uint32_t> enabled_cores) {
        m_enabled_cores = enabled_cores;
    }

    /*!
     * @brief Set processor total threads
     * @param[in] total_threads Processor total threads
     * */
    void set_total_threads(OptionalField<std::uint32_t> total_threads) {
        m_total_threads = total_threads;
    }

    /*!
     * @brief Set processor enabled threads
     * @param[in] enabled_threads Processor enabled threads
     * */
    void set_enabled_threads(OptionalField<std::uint32_t> enabled_threads) {
        m_enabled_threads = enabled_threads;
    }

    /*!
     * @brief Set CPUID for response
     * @param cpu_id CPUID object
     * */
    void set_cpu_id(const attribute::CpuId& cpu_id) {
        m_cpu_id = cpu_id;
    }

    /*!
     * @brief Set processor capabilities
     *
     * @param capabilities Vector of capabilities
     * */
    void set_capabilities(const Capabilities& capabilities) {
        m_capabilities = capabilities;
    }

    /*!
     * @brief Get an array of processot capabilities
     *
     * @return array of processor capabilities
     */
    const Capabilities& get_capabilities() const {
        return m_capabilities;
    }

    /*!
     * @brief Adds single processor capability
     *
     * @param capability processor capability
     * */
    void add_capability(const std::string& capability) {
        m_capabilities.add_entry(capability);
    }

private:

    OptionalField<std::string> m_socket{};
    OptionalField<enums::ProcessorType> m_processor_type{};
    OptionalField<enums::ProcessorArchitecture> m_processor_architecture{};
    OptionalField<enums::ProcessorInstructionSet> m_instruction_set{};
    Capabilities m_capabilities{};
    OptionalField<std::string> m_manufacturer{};
    OptionalField<enums::ProcessorModel> m_model{};
    OptionalField<std::string> m_model_name{};
    attribute::CpuId m_cpu_id{};
    OptionalField<std::uint32_t> m_max_speed_mhz{};
    OptionalField<std::uint32_t> m_total_cores{};
    OptionalField<std::uint32_t> m_enabled_cores{};
    OptionalField<std::uint32_t> m_total_threads{};
    OptionalField<std::uint32_t> m_enabled_threads{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

