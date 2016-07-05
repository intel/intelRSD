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
 * @file get_processor_info.hpp
 *
 * @brief Get Processor Info request and response.
 * */

#ifndef IPMI_COMMAND_SDV_GET_PROCESSOR_INFO_HPP
#define	IPMI_COMMAND_SDV_GET_PROCESSOR_INFO_HPP

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <map>

using std::size_t;
using std::uint8_t;
using std::uint16_t;
using std::vector;
using std::string;
using std::to_string;
using std::map;

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for Get Processor Info command. Many platforms support few CPUs so it's needed
 * to set the ID (number) of the checked processor.
 */
class GetProcessorInfo: public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetProcessorInfo();

    /*! Copy constructor. */
    GetProcessorInfo(const GetProcessorInfo&) = default;

    /*! Assignment operator */
    GetProcessorInfo& operator=(const GetProcessorInfo&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetProcessorInfo();

    /*!
     * @brief Sets ID of the processor.
     * @param id of the processor.
     */
    void set_id(uint8_t id) {
        m_cpu_id = id;
    }

    /*!
     * @brief Gets ID of the processor.
     * @return id of the processor.
     */
    uint8_t get_id() const {
        return m_cpu_id;
    }

    virtual void pack(vector<uint8_t>& data) const;

private:
    uint8_t m_cpu_id{};

};

}

namespace response {

/*!
 * @brief Response message for Get Processor Info command. Gets CPU type, presence, frequency.
 */
class GetProcessorInfo: public Response {
public:

    enum CPU_TYPE: uint8_t {
        CPU_TYPE_XEON = 0x18,
        CPU_TYPE_ATOM = 0x1a,
        CPU_TYPE_UNKNOWN = 0xff
    };

    /*!
     * @brief Default constructor.
     */
    GetProcessorInfo();

    /*! Copy constructor. */
    GetProcessorInfo(const GetProcessorInfo&) = default;

    /*! Assignment operator */
    GetProcessorInfo& operator=(const GetProcessorInfo&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetProcessorInfo();

    /*!
     * @brief Gets type of the CPU.
     * @return type of the CPU.
     */
    CPU_TYPE get_cpu_type() const {
        return m_cpu_type;
    }

    /*!
     * @brief Sets cpu type of the processor.
     * @param cpu_type of processor.
     */
    void set_cpu_type(const CPU_TYPE cpu_type){
        m_cpu_type = cpu_type;
    }

    /*!
     * @brief Gets CPU type name.
     * @return CPU type name.
     */
    const string& get_cpu_type_name() const;

    /*!
     * @brief Gets CPU frequency in MHz.
     * @return CPU frequency in MHz
     */
    uint16_t get_cpu_frequency() const {
        return m_cpu_frequency;
    }

    /*!
     * @brief Sets cpu frequency of the processor.
     * @param cpu_frequency of processor.
     */
    void set_cpu_frequency(const uint16_t cpu_frequency){
        m_cpu_frequency = cpu_frequency;
    }

    /*!
     * @brief Checks if CPU slot is occupied.
     * @return true if CPU is available, otherwise false.
     */
    bool is_present() {
        return m_presence;
    }

    /*!
     * @brief Sets presence of the processor.
     * @param presence of processor.
     */
    void set_presence(const bool presence) {
        m_presence = presence;
    }

    virtual void unpack(const vector<uint8_t>& data);

private:
    CPU_TYPE m_cpu_type{};
    uint16_t m_cpu_frequency{};
    bool m_presence{};

    static constexpr size_t RESPONSE_SIZE = 5;
    static constexpr size_t OFFSET_CPU_TYPE = 1;
    static constexpr size_t OFFSET_CPU_FREQUENCY = 2;
    static constexpr size_t OFFSET_CPU_PRESENCE = 4;

    uint16_t extract_cpu_frequency(const vector<uint8_t>& data) const;

    map<CPU_TYPE, string> m_cpu_type_name = {
        { CPU_TYPE_XEON, "Intel(R) Xeon(R)" },
        { CPU_TYPE_ATOM, "Intel(R) Atom(R)" },
        { CPU_TYPE_UNKNOWN, "Unknown" }
    };

};

}
}
}
}
#endif	/* IPMI_COMMAND_SDV_GET_PROCESSOR_INFO_HPP */
