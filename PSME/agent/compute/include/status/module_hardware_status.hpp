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
 * @file module_hardware_status.hpp
 * @brief Module hardware status implementation
 * */

#pragma once



#include "agent-framework/status/module_hardware_status.hpp"
#include "status/gpio/gpio_status.hpp"

/* Forward declaration */
namespace json { class Value; }

/*! Agent namespace */
namespace agent {
/*! Compute namespace */
namespace compute {
/*! Status namespace */
namespace status {

static constexpr uint8_t MODULE_NR_MAX = 4;
/*!
 * @brief ModuleHardwareStatus class.
 * */
class ModuleHardwareStatus : public agent_framework::status::ModuleHardwareStatus {
    uint32_t m_slot = 0;
    GpioStatus m_gpio_status;
    Status m_last_status{agent_framework::status::ModuleStatus::Status::NOT_PRESENT};
public:
    /*!
     * @brief Class default constructor.
     * */
    ModuleHardwareStatus(uint32_t slot) :
            m_slot(slot), m_gpio_status(slot) {
    }

    /*!
     * @brief Class destructor.
     * */
    ~ModuleHardwareStatus() { }

    /*!
     * @brief Method reads Hardware Status of Module.
     * */
    Status read_status();


private:
    Status read_status_from_file();

    bool went_up();
    bool went_down();
    void wait_for_smbios();
};

}
}
}

