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
#include "command/ConfigurationBased/storage_services_collections.hpp"

#include "agent-framework/command/storage/get_storage_services_info.hpp"
#include "agent-framework/module/module_manager.hpp"

using namespace agent_framework::command;
using namespace agent_framework::generic;
using namespace agent_framework::model;

using std::out_of_range;

/*! GetStorageServicesInfo implementation */
class GetStorageServicesInfo : public storage::GetStorageServicesInfo {
public:

    GetStorageServicesInfo() {}

    using storage::GetStorageServicesInfo::execute;

    void execute(const Request& request, Response& response) {
        auto services = request.get_services();

        auto submodule = ModuleManager::get_submodule(services);
        if (nullptr == submodule) {
            THROW(agent_framework::exceptions::InvalidUuid, "storage-agent",
                  "Services uuid = '" + services + "' not found.");
        }

        StorageServices storage_services;
        storage_services.set_status(attribute::Status(enums::State::Enabled,
                                                      enums::Health::OK));
        storage_services.add_collection(attribute::Collection(
                                   enums::CollectionName::iSCSITargets,
                                   enums::CollectionType::iSCSITargets,
                                   ""));
        storage_services.add_collection(attribute::Collection(
                                  enums::CollectionName::LogicalDrives,
                                  enums::CollectionType::LogicalDrives,
                                  ""));
        storage_services.add_collection(attribute::Collection(
                                  enums::CollectionName::PhysicalDrives,
                                  enums::CollectionType::PhysicalDrives,
                                  ""));
        storage_services.set_oem(attribute::Oem());
        response.set_services(storage_services);
    }

    ~GetStorageServicesInfo();
};

GetStorageServicesInfo::~GetStorageServicesInfo() {}


static Command::Register<GetStorageServicesInfo> g("ConfigurationBased");
