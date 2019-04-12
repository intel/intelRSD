/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http_//www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * @file processor_instructions_set.hpp
 * */

#pragma once

#include <cstdint>
#include <string>
#include <cstring>
#include <vector>
#include <ostream>

namespace agent_framework {
namespace model {
namespace enums {

/*!
 * @brief Instruction set enum for processors
 * */
class ProcessorInstructionSet {
public:

    using underlying_type = std::uint32_t;

    /*!
     * @brief Enum constructor
     * */
    enum ProcessorInstructionSetEnum : std::uint32_t {
        x86_64, x86, IA_64, ARM_A32, ARM_A64, MIPS32, MIPS64, OEM
    };

    /*!
     * @brief to string
     *
     * @return pointer to an array of chars
     * */
    const char* to_string() const;

    /*!
     * @brief From string
     *
     * @param string Reference to a string that is going to be processed into enum
     *
     * @return InstructionSet object
     * */
    static ProcessorInstructionSet from_string(const std::string& string);

    /*!
     * @brief Gets all available Enum values as a vector of strings.
     * @return Returns std::vector of std::string containing Enum values.
     * */
    static std::vector<std::string> get_values();

    /*!
     * @brief Default constructor
     * */
    ProcessorInstructionSet() = delete;

    /*!
     * @brief Constructor
     *
     * @param value uint32_t value of enum
     * */
    constexpr ProcessorInstructionSet(ProcessorInstructionSetEnum value) : m_value(value) {}

    /*!
     * @brief Operation of getting enums
     *
     * @return Enum
     * */
    constexpr operator ProcessorInstructionSetEnum() const {
        return static_cast<ProcessorInstructionSetEnum>(m_value);
    }

    /*!
     * @brief Is string an allowable value of this enum
     * @param string Value to be checked
     * @return true if the string is a valid value of enum, false otherwise
     * */
    static bool is_allowable_value(const std::string& string);

    friend std::ostream& operator<<(std::ostream& out_stream, const ProcessorInstructionSet& in_enum) {
        out_stream << in_enum.to_string();
        return out_stream;
    }
private:
    std::uint32_t m_value{};
};
}
}
}
