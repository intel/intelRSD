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
 * @file file_fabric_interface.hpp
 */

#pragma once


#include "base_fabric_interface.hpp"
#include "base_file_factory.hpp"
#include "fabric_properties.hpp"



namespace nvmfpp {

class FileFabricInterface final : public nvmfpp::BaseFabricInterface {
public:
    FileFabricInterface(std::unique_ptr<nvmfpp::BaseFileFactory> bff) : m_factory(std::move(bff)) {}

    ~FileFabricInterface();

    /*!
     * @brief Connects to nvme drive.
     * @param fabric_properties Required and optional parameters to setup connection.
     */
    virtual void connect(const nvmfpp::FabricProperties& fabric_properties) override;

    /*!
     * @brief Disconnects only one nvme drive by name.
     * @param fabric_properties Required parameter to break the connection.
     */
    virtual void disconnect_by_drive(const nvmfpp::FabricProperties& fabric_properties) override;

    /*!
     * @brief Disconnects nvme drive by subsystem nqn. Could disconnect more then one drive.
     * @param fabric_properties Required parameter to break the connection.
     * @param drives Required list of all nvme drives name on host.
     */
    virtual void disconnect_by_nqn(const nvmfpp::FabricProperties& fabric_properties, const std::vector<std::string>& drives) override;

private:
    std::unique_ptr<nvmfpp::BaseFileFactory> m_factory{};
};

}
