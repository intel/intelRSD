/*!
 * @brief Set OOB Heap Boot Options command declaration.
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
 * @file set_oob_heap_boot_options.hpp
 */

#pragma once



#include "set_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI SetOOBHeap Boot Options.
 */
class SetOobHeapBootOptions : public SetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobHeapBootOptions();


    /*! Copy constructor */
    SetOobHeapBootOptions(const SetOobHeapBootOptions&) = default;


    /*! Assignment operator */
    SetOobHeapBootOptions& operator=(const SetOobHeapBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobHeapBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const override;


    void set_offset(uint16_t offset) { m_offset = offset; }


    void set_data(const std::vector<uint8_t>& data) { m_data = data; }


    static constexpr auto MAX_BLOCK_SIZE = 200;

    virtual const char* get_command_name() const override {
        return "SetOobHeapBootOptions";
    }

private:

    uint16_t m_offset{0};
    std::vector<uint8_t> m_data{};
};

}

namespace response {

/*!
 * @brief Response message for IPMI SetOOBHeap Boot option.
 */
class SetOobHeapBootOptions : public SetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobHeapBootOptions();


    /*! Copy constructor */
    SetOobHeapBootOptions(const SetOobHeapBootOptions&) = default;


    /*! Assignment operator */
    SetOobHeapBootOptions& operator=(const SetOobHeapBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobHeapBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) override;

    virtual const char* get_command_name() const override {
        return "SetOobHeapBootOptions";
    }

private:
    static constexpr size_t RESPONSE_SIZE = 1;
};

}

}
}
}
