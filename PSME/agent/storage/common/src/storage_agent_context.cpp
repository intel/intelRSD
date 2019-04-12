/*!
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
 * @file storage_agent_context.cpp
 */

#include "storage_agent_context.hpp"
#include "storage_config.hpp"

#include "sysfs/sysfs_interface.hpp"

#ifdef TGT_STORAGE
#include "drive_handling/local_drive_reader.hpp"
#include "drive_handling/local_drive_handler_factory.hpp"
#include "interface-reader/single_interface_reader.hpp"
#else
#include "interface-reader/udev_interface_reader.hpp"
#include "drive_handling/nvme_drive_handler_factory.hpp"
#include "drive_handling/nvme_drive_reader.hpp"
#include "nvme/nvme_interface.hpp"
#endif

using namespace agent::storage;

void StorageAgentContext::initialize(std::shared_ptr<loader::AgentConfiguration> configuration_ptr) {
    this->configuration = configuration_ptr;

    sysfs_interface = std::make_shared<sysfs::SysfsInterface>(sysfs::SysfsInterface{});
    lvm_api = std::make_shared<::storage::lvm::LvmApi>(::storage::lvm::LvmApi{});

#ifdef TGT_STORAGE
    drive_reader = std::make_shared<LocalDriveReader>(sysfs_interface);
    drive_handler_factory = std::make_shared<LocalDriveHandlerFactory>(sysfs_interface);
    interface_reader = std::make_shared<interface_reader::SingleInterfaceReader>(configuration->get_iscsi_data().get_portal_interface());
#else
    auto nvme_interface = std::make_shared<nvme::NvmeInterface>(nvme::NvmeInterface{});
    drive_reader = std::make_shared<NvmeDriveReader>(sysfs_interface);
    drive_handler_factory = std::make_shared<NvmeDriveHandlerFactory>(nvme_interface);
    interface_reader = std::make_shared<interface_reader::UdevInterfaceReader>(configuration->get_nic_drivers());
#endif
}