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

#include "agent-framework/command/storage/get_collection.hpp"
#include "agent-framework/module/module_manager.hpp"

using std::vector;

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::generic;

/*! GetCollection implementation */
class GetCollection : public storage::GetCollection {
public:
    GetCollection() {}

    using storage::GetCollection::execute;

    void execute(const Request& request, Response& response) {

        auto uuid = request.get_component();
        auto collection_name = request.get_name();

        // if we are looking for storage_services - check manager for submodules
        if (StorageServicesCollections::STORAGE_SERVICES_NAME == collection_name) {
            auto module = ModuleManager::get_module(uuid);
            if (nullptr != module) {
                for (const auto& submodule : module->get_submodules()) {
                    response.add_subcomponent(
                        agent_framework::model::attribute::SubcomponentEntry(
                            submodule->get_name()));
                }
            }
            else {
                THROW(agent_framework::exceptions::InvalidUuid, "storage-agent",
                    "Unknown UUID provided: '" + uuid + "'.");
            }
            return;
        }

        // next - we look for iSCSI Targets, Logical drives and Hard Drives
        auto submodule = ModuleManager::get_submodule(uuid);
        if (nullptr != submodule) {
            m_add_collection(submodule, collection_name, response);
            return;
        }

        auto logical_drive = ModuleManager::find_logical_drive(uuid).lock();
        if (logical_drive) {
            m_add_collection(logical_drive.get(), collection_name, response);
            return;
        }

        auto target = ModuleManager::find_target(uuid).lock();
        if (target) {
            m_add_collection(target.get(), collection_name, response);
            return;
        }

        THROW(agent_framework::exceptions::InvalidUuid, "storage-agent",
            "Unknown UUID provided: '" + uuid + "'.");
    }

    ~GetCollection();

private:

    template<typename T>
    void m_add_collection(const T* component, const string& collection_name,
                          Response& response) {
        if (StorageServicesCollections::PHYSICAL_DRIVES_NAME ==
            collection_name) {
            m_add_physical_drives(component, response);
        }
        else if (StorageServicesCollections::LOGICAL_DRIVES_NAME ==
                 collection_name) {
            m_add_logical_drives(component, response);
        }
        else if (StorageServicesCollections::ISCSI_TARGETS_NAME ==
                 collection_name) {
            m_add_iscsi_targets(component, response);
        }
        else {
            THROW(::agent_framework::exceptions::InvalidParameters,
                  "storage-agent",
                  "Invalid collection name '" + collection_name + "'." );
        }

        log_debug(GET_LOGGER("storage-agent"), "Getting collection "
                  << collection_name
                  << " is successfully finished.");
    }

    template<typename T>
    void m_add_physical_drives(const T* component, Response& response) {
        for (const auto& drive : component->get_hard_drives()) {
            response.add_subcomponent(
                agent_framework::model::attribute::SubcomponentEntry(
                    drive->get_uuid()));
        }
    }

    template<typename T>
    void m_add_logical_drives(const T* component, Response& response) {
        for (const auto& logical_drive : component->get_logical_drives()) {
          response.add_subcomponent(
              agent_framework::model::attribute::SubcomponentEntry(
                  logical_drive->get_uuid()));
        }
    }

    template<typename T>
    void m_add_iscsi_targets(const T* component, Response& response) {
        for (const auto& target : component->get_targets()) {
          response.add_subcomponent(
              agent_framework::model::attribute::SubcomponentEntry(
                  target->get_uuid()));
        }
    }
};

GetCollection::~GetCollection() {}

template<>
void GetCollection::m_add_iscsi_targets(const LogicalDrive*, Response&) {}

template<>
void GetCollection::m_add_iscsi_targets(const Target*, Response&) {}

static Command::Register<GetCollection> g("ConfigurationBased");
