/*!
 * @brief Get OOB Heap Boot Options command declaration.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_oob_heap_boot_options.hpp
 */

#pragma once



#include "get_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI GetOOBHeap Boot Options.
 */
class GetOobHeapBootOptions : public GetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    GetOobHeapBootOptions(uint16_t offset, uint16_t size);


    /*! Copy constructor */
    GetOobHeapBootOptions(const GetOobHeapBootOptions&) = default;


    /*! Assignment operator */
    GetOobHeapBootOptions& operator=(const GetOobHeapBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobHeapBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const override;


    uint16_t get_offset() const { return m_offset; }


    uint16_t get_size() const { return m_size; }

    virtual const char* get_command_name() const override {
        return "GetOobHeapBootOptions";
    }

private:

    uint16_t m_offset{0};
    uint16_t m_size{0};
};

}

namespace response {

/*!
 * @brief Response message for IPMI GetOOBHeap Boot option.
 */
class GetOobHeapBootOptions : public GetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    GetOobHeapBootOptions();


    /*! Copy constructor */
    GetOobHeapBootOptions(const GetOobHeapBootOptions&) = default;


    /*! Assignment operator */
    GetOobHeapBootOptions& operator=(const GetOobHeapBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobHeapBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) override;


    std::vector<uint8_t> get_heap() const { return m_heap; }


    size_t v2_payload_size() override { return OFFSET_HEAP_OFFSET + m_heap.size(); }


    std::string what() override { return "Heap"; }

    virtual const char* get_command_name() const override {
        return "GetOobHeapBootOptions";
    }


private:

    void set_heap(std::vector<uint8_t>& heap) { m_heap = heap; }


    std::vector<uint8_t> m_heap{};

    static constexpr size_t OFFSET_HEAP_OFFSET = OFFSET_VERSION;
};

}

}
}
}
