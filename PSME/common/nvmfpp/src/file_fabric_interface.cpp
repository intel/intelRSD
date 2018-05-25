/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file file_fabric_interface.cpp
 */

#include "nvmfpp/file_fabric_interface.hpp"


#include <stdexcept>


namespace {

static constexpr char FABRICS_PATH[] = "/dev/nvme-fabrics";
static constexpr char SYS_NVME[] = "/sys/class/nvme/";
static constexpr char SUBSYSTEM_NQN[] = "/subsysnqn";
static constexpr char DELETE_CONTROLLER[] = "/delete_controller";
static constexpr char TRUE[] = "1";

}

nvmfpp::FileFabricInterface::~FileFabricInterface() {}

void nvmfpp::FileFabricInterface::connect(const FabricProperties& fabric_properties) {
    if (fabric_properties.check_mandatory_connect_fields()) {
        std::string content = fabric_properties.pack();
        if (!content.empty()) {
            std::unique_ptr<nvmfpp::BaseFile> baseFile = m_factory->create_file(::FABRICS_PATH, FileMode::WRITE);
            baseFile->write_to_file(content);
        }
    }
    else {
        throw std::invalid_argument("Lack of mandatory field.");
    }
}

void nvmfpp::FileFabricInterface::disconnect_by_drive(const FabricProperties& fabric_properties) {
    if (!fabric_properties.get_device().empty()) {
        std::string delete_controller_path = ::SYS_NVME + fabric_properties.get_device() + ::DELETE_CONTROLLER;
        std::unique_ptr<nvmfpp::BaseFile> baseFile = m_factory->create_file(delete_controller_path, FileMode::WRITE);
        baseFile->write_to_file(::TRUE);
    }
    else {
        throw std::invalid_argument("Lack of mandatory device name.");
    }
}

void nvmfpp::FileFabricInterface::disconnect_by_nqn(const FabricProperties& fabric_properties,
                                                    const std::vector<std::string>& drives) {
    if (!fabric_properties.get_nqn().empty()) {
        bool success = false;
        std::string subsystemnqn_path{};
        std::string delete_controller_path{};
        for (const auto& drive : drives) {
            subsystemnqn_path = ::SYS_NVME + drive + ::SUBSYSTEM_NQN;
            try {
                std::unique_ptr<nvmfpp::BaseFile> subsystemnqn_file = m_factory->create_file(subsystemnqn_path,
                                                                                             FileMode::READ);
                if (fabric_properties.get_nqn() == subsystemnqn_file->read_from_file()) {
                    std::unique_ptr<nvmfpp::BaseFile> delete_controller = m_factory->create_file(
                            ::SYS_NVME + drive + ::DELETE_CONTROLLER, FileMode::WRITE);
                    delete_controller->write_to_file(::TRUE);
                    success = true;
                }
            }
            catch (const std::exception&) {
                continue;
            }
        }
        if (!success) {
            throw std::invalid_argument("There was no drive to disconnect.");
        }
    }
    else {
        throw std::invalid_argument("Lack of mandatory nqn name.");
    }
}
