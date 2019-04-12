/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file ipmb/dispatcher.hpp
 * @brief IPMI message dispatcher.
 * */

#pragma once



#include <ipmb/utils.hpp>
#include <ipmb/ipmi_message.hpp>

#include <agent-framework/generic/singleton.hpp>

#include <condition_variable>

/*! Agent namespace */
namespace agent {
/*! Chassis namespace */
namespace chassis {
/*! IPMB namespace */
namespace ipmb {

/*! GPIO Reader */
class Gpio : public agent_framework::generic::Singleton<Gpio> {
public:
    std::mutex presence_mutex{};
    std::condition_variable presence_cv{};


    /*!
     * Returns GPIO presence byte
     * @return GPIO presence byte
     * */
    uint8_t get_presence();

    /*!
     * @brief Returns presence flag of sled on given slot
     * @param slot Slot number
     * @return true if Sled is present, false otherwise.
     */
    bool is_present(std::uint8_t slot);

    /*!
     * @return Minimal update interval between presence readings.
     */
    std::chrono::milliseconds get_minimal_update_interval() const;


    /*!
     * Read GPIOs and sets GPIO presence byte
     * @param presence GPIO presence byte
     * */
    void set_presence(const uint8_t presence);


    virtual ~Gpio();


protected:
    uint8_t read_bdc_r_gpio() const;


    uint8_t read_presence() const;


private:
    volatile uint8_t m_presence{};
};

}
}
}

