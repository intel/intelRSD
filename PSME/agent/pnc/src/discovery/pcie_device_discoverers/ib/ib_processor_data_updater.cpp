/*!
 * @brief Implementation of class used for updating ib processor data represented later in Redfish as Processor
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
 * @file ib_processor_data_updater.cpp
 */


#include "agent-framework/module/utils/to_hex_string.hpp"
#include "discovery/device_discoverers//ib/ib_processor_data_updater.hpp"
#include "uuid/uuid.hpp"



namespace agent {
namespace pnc {
namespace discovery {
namespace device_discoverers {
namespace data_updater {

using namespace agent::pnc::sysfs;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model::utils;


IBProcessorDataUpdater::IBProcessorDataUpdater(const SysfsDevice& sysfs_device,
                                               const Processor& processor) :
    m_sysfs_device(sysfs_device),
    m_processor(processor) {

    try {

        opaepp::PcieDeviceAddress pcie_device_address{sysfs_device.id.bus_num,
                                                      sysfs_device.id.device_num,
                                                      sysfs_device.id.function_num};

        m_opae_devices = opaepp::OpaeCppDeviceReader().get_devices(pcie_device_address);
    }
    catch (const std::exception& ex) {
        log_error("pnc-discovery", "IB OPAE Updater - Reader error: " << ex.what());
    }
    catch (...) {
        log_error("pnc-discovery", "IB OPAE Updater - Unknown error!");
    }
}


const Processor&
IBProcessorDataUpdater::update_processor_data() {

    set_external_interfaces();

    set_fw_id();

    set_fw_version();

    set_host_interface();

    set_model();

    set_pcie_virtual_functions();

    set_programmable_from_host();

    set_reconfiguration_slots_details();

    set_type();

    set_vendor_id_and_fw_manufacturer();

    set_processor_type();

    return get_processor();
}


void
IBProcessorDataUpdater::set_external_interfaces() {

}


void
IBProcessorDataUpdater::set_fw_id() {

    for (const auto& opae_device : get_opae_devices()) {

        if (opae_device.get_device_type() == FPGA_DEVICE) {

            Fpga fpga = get_processor().get_fpga();

            if (opae_device.get_blue_bitstream_id().has_value()) {
                auto bbs_id = to_hex_string<8, true>(opae_device.get_blue_bitstream_id().value());

                fpga.set_firmware_id(bbs_id);
            }

            get_processor().set_fpga(fpga);

            return;
        }
    }
}


void
IBProcessorDataUpdater::set_fw_version() {

    for (const auto& opae_device : get_opae_devices()) {

        if (opae_device.get_device_type() == FPGA_DEVICE) {

            Fpga fpga = get_processor().get_fpga();

            fpga_version bbs_version = opae_device.get_blue_bitstream_version();

            std::stringstream bbs_version_ss;
            bbs_version_ss << static_cast<uint32_t>(bbs_version.major) << "."
                           << static_cast<uint32_t>(bbs_version.minor) << "."
                           << static_cast<uint32_t>(bbs_version.patch);

            fpga.set_firmware_version(bbs_version_ss.str());

            get_processor().set_fpga(fpga);

            return;
        }
    }
}


void
IBProcessorDataUpdater::set_host_interface() {

}


void
IBProcessorDataUpdater::set_model() {

}


void
IBProcessorDataUpdater::set_pcie_virtual_functions() {

    SysfsDecoder decoder = SysfsDecoder::make_instance(SysfsReader{});

    std::vector<SysfsFunction> sysfs_functions = decoder.get_functions(get_sysfs_device());

    auto counter = std::count_if(sysfs_functions.cbegin(), sysfs_functions.cend(),
                                 [](const SysfsFunction& func) { return func.is_virtual; });

    Fpga fpga = get_processor().get_fpga();

    fpga.set_pcie_virtual_functions(counter);

    get_processor().set_fpga(fpga);
}


void
IBProcessorDataUpdater::set_programmable_from_host() {

    Fpga fpga = get_processor().get_fpga();

    fpga.set_programmable_from_host(true);

    get_processor().set_fpga(fpga);
}


void
IBProcessorDataUpdater::set_reconfiguration_slots_details() {

    for (const auto& opae_device : get_opae_devices()) {

        if (opae_device.get_device_type() == FPGA_ACCELERATOR) {

            Fpga fpga = get_processor().get_fpga();

            std::string uuid_str = unparse_uuid((const void*) opae_device.get_guid());

            FpgaReconfigurationSlot slot;
            slot.set_uuid(uuid_str);

            Array<FpgaReconfigurationSlot> array{slot};

            fpga.set_reconfiguration_slots(array);

            get_processor().set_fpga(fpga);

            return;
        }
    }
}


void
IBProcessorDataUpdater::set_type() {

    Fpga fpga = get_processor().get_fpga();

    fpga.set_type(enums::FpgaType::Discrete);

    get_processor().set_fpga(fpga);
}


void
IBProcessorDataUpdater::set_vendor_id_and_fw_manufacturer() {

    for (const auto& opae_device : get_opae_devices()) {

        if (opae_device.get_device_type() == FPGA_DEVICE) {

            CpuId cpu_id = get_processor().get_cpu_id();
            Fpga fpga = get_processor().get_fpga();

            if (opae_device.get_vendor_id().has_value()) {
                auto vendor_id = to_hex_string<2, true>(
                    opae_device.get_vendor_id().value());

                cpu_id.set_vendor_id(vendor_id);
                get_processor().set_cpu_id(cpu_id);

                fpga.set_firmware_manufacturer(vendor_id);
            }

            get_processor().set_fpga(fpga);

            return;
        }
    }
}


void
IBProcessorDataUpdater::set_processor_type() {

    get_processor().set_processor_type(enums::ProcessorType::FPGA);
}


const SysfsDevice
IBProcessorDataUpdater::get_sysfs_device() const {

    return m_sysfs_device;
}


Processor&
IBProcessorDataUpdater::get_processor() {

    return m_processor;
}


const IBProcessorDataUpdater::OpaeDevices&
IBProcessorDataUpdater::get_opae_devices() const {

    return m_opae_devices;
}

} // data_collectors
} // device_discovery
} // namespace discovery
} // namespace pnc
} // namespace agent
