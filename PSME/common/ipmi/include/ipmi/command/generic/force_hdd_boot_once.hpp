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
 * @file force_hdd_boot_once.hpp
 *
 * @brief Force local HDD once boot command declaration.
 * */

#ifndef IPMI_COMMAND_GENERIC_FORCE_HDD_BOOT_ONCE_HPP
#define IPMI_COMMAND_GENERIC_FORCE_HDD_BOOT_ONCE_HPP

#include "ipmi/command/generic/set_system_boot_options.hpp"

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for IPMI Force HDD Once Boot.
 */
class ForceHddBootOnce: public request::SetSystemBootOptions {
public:
    /*!
     * @brief Default constructor.
     */
    ForceHddBootOnce();

    /*! Copy constructor */
    ForceHddBootOnce(const ForceHddBootOnce&) = default;

    /*! Assginment operator */
    ForceHddBootOnce& operator=(const ForceHddBootOnce&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~ForceHddBootOnce();

    virtual void pack(vector<uint8_t>& data) const;

private:
    static constexpr std::uint8_t OPTION_BOOT_FLAGS_VALID = 0x80;
    static constexpr std::uint8_t OPTION_BOOT_DEVICE_FORCE_HDD = 0x08;
    static constexpr std::uint8_t OPTION_SYSTEM_DEFAULT = 0x00;
    static constexpr std::uint8_t OPTION_BIOS_DEFAULT = 0x00;
    static constexpr std::uint8_t OPTION_BOOT_DEVICE_INSTANCE_FORCE_HDD = 0x02;
};

}

namespace response {

/*!
 * @brief Response message for IPMI Force HDD Boot.
 */
class ForceHddBootOnce: public response::SetSystemBootOptions  {
public:

    /*!
     * @brief Default constructor.
     */
    ForceHddBootOnce() = default;

    /*!
     * @brief Copy constructor.
     * @param orig object to be copied.
     */
    ForceHddBootOnce(const ForceHddBootOnce& orig) = default;


    /*! Assignment operator */
    ForceHddBootOnce& operator=(const ForceHddBootOnce&) = default;

    /*!
     * @brief Default destructor.
     */
    ~ForceHddBootOnce();

    void unpack(const vector<uint8_t>& data) override;

private:
    static constexpr size_t RESPONSE_SIZE = 1;
};

}
}
}
}
#endif	/* IPMI_COMMAND_GENERIC_FORCE_HDD_BOOT_HPP */
