/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "agent-framework/command/storage/get_manager_info.hpp"
#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/version.hpp"

using namespace agent_framework::generic;
using namespace agent_framework::command;

using std::string;
using namespace agent_framework::model;
using agent_framework::command::exception::NotFound;

/*! Dummy GetManagerInfo implementation */
class GetManagerInfo : public storage::GetManagerInfo {
public:
    GetManagerInfo() { }

    using storage::GetManagerInfo::execute;

    void execute(const Request& request, Response& response) {
        auto uuid = request.get_component();
        auto module = ModuleManager::get_module(uuid);
        auto submodule = ModuleManager::get_submodule(uuid);

        std::string ipv4 = "";

        if (nullptr != module) {
            ipv4 = module->get_ip_address();
        }
        else {
            if (nullptr != submodule) {
                ipv4 = submodule->get_ip_address();
            }
            else {
                throw NotFound();
            }
        }
        agent_framework::model::Manager manager;

        manager.set_status(attribute::Status(enums::State::Enabled,
                                             enums::Health::OK));
        manager.set_firmware_version(Version::to_string());
        manager.set_ipv4_address(ipv4);

        attribute::SerialConsole console;
        console.set_signal_type(enums::SerialConsoleSignalType::Rs232);
        console.set_bitrate(115200);
        console.set_parity(enums::SerialConsoleParity::None);
        console.set_data_bits(8);
        console.set_stop_bits(1);
        console.set_flow_control(enums::SerialConsoleFlowControl::None);
        console.set_pin_out(enums::SerialConsolePinOut::Cisco);
        console.set_enabled(true);
        manager.set_serial_console(console);

        manager.add_collection(attribute::Collection(
                                   enums::CollectionName::StorageServices,
                                   enums::CollectionType::StorageServices,
                                   ""));
        manager.set_oem(attribute::Oem());
        response.set_manager(manager);
    }

    ~GetManagerInfo();
};

GetManagerInfo::~GetManagerInfo() { }

static Command::Register<GetManagerInfo> g("ConfigurationBased");
