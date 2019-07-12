/*!
 * @brief Class used for updating ib processor data represented later in Redfish as Processor
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file ib_processor_data_updater.hpp
 */

#include "agent-framework/module/model/processor.hpp"
#include "sysfs/sysfs_decoder.hpp"
#include "sysfs/sysfs_reader.hpp"
#include "opaepp/opae/opae_cpp_device_reader.hpp"



#pragma once

namespace agent {
namespace pnc {
namespace discovery {
namespace device_discoverers {
namespace data_updater {

class IBProcessorDataUpdater {

public:

    using OpaeDevices = std::vector<opaepp::Device>;


    /*!
     * @brief In-bound processor updater (using opae library)
     * @param[in] sysfs_device Sysfs data of the device
     * @param[in] processor Processor which needs to be updated
     */
    IBProcessorDataUpdater(const agent::pnc::sysfs::SysfsDevice& sysfs_device,
                           const agent_framework::model::Processor& processor);


    virtual ~IBProcessorDataUpdater() = default;


    /*!
     * @brief update processor data after receiving required data with use of opae api
     * @param[in] sysfs_device Sysfs data of the device
     * @param[in] processor which needs to be updated
    */
    const agent_framework::model::Processor& update_processor_data();


private:

    // setters:

    void set_external_interfaces();


    void set_fw_id();


    void set_fw_version();


    void set_host_interface();


    void set_model();


    void set_pcie_virtual_functions();


    void set_programmable_from_host();


    void set_reconfiguration_slots_details();


    void set_type();


    void set_vendor_id_and_fw_manufacturer();


    void set_processor_type();

    // getters:

    const agent::pnc::sysfs::SysfsDevice get_sysfs_device() const;


    agent_framework::model::Processor& get_processor();


    const OpaeDevices& get_opae_devices() const;

    // members:

    agent::pnc::sysfs::SysfsDevice m_sysfs_device;
    agent_framework::model::Processor m_processor;
    OpaeDevices m_opae_devices{};
};

} // data_collectors
} // device_discovery
} // namespace discovery
} // namespace pnc
} // namespace agent