/*!
 * @brief Set OOB Reset Boot Options command declaration.
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
 * @file set_oob_reset_boot_options.hpp
 */

#pragma once



#include "set_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI SetOOBReset Boot Options.
 */
class SetOobResetBootOptions : public SetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobResetBootOptions();


    /*! Copy constructor */
    SetOobResetBootOptions(const SetOobResetBootOptions&) = default;


    /*! Assignment operator */
    SetOobResetBootOptions& operator=(const SetOobResetBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobResetBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const override;

    virtual const char* get_command_name() const override {
        return "SetOobResetBootOptions";
    }

private:

    static constexpr std::uint8_t CLEAR = 0x01;

};

}

namespace response {

/*!
 * @brief Response message for IPMI SetOOBReset Boot option.
 */
class SetOobResetBootOptions : public SetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobResetBootOptions();


    /*! Copy constructor */
    SetOobResetBootOptions(const SetOobResetBootOptions&) = default;


    /*! Assignment operator */
    SetOobResetBootOptions& operator=(const SetOobResetBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobResetBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) override;

    virtual const char* get_command_name() const override {
        return "SetOobResetBootOptions";
    }

private:
    static constexpr size_t RESPONSE_SIZE = 1;
};

}

}
}
}
