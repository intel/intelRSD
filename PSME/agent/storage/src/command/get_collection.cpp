/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/storage_commands.hpp"
#include "agent-framework/module/constants/common.hpp"

using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {

    template<typename T>
    bool add_collection(const CollectionName& expected_collection_name,
                        const string& received_collection_name,
                        bool is_storage_service,
                        const string& parent_uuid,
                        GetCollection::Response& response) {

        if (expected_collection_name.to_string() == received_collection_name) {
            for (const auto& uuid : is_storage_service ?
                    get_manager<T>().get_keys() :
                    get_manager<T>().get_keys(parent_uuid)) {
                response.add_entry(uuid);
            }
            return true;
        }
        return false;
    }

    void get_collection(const GetCollection::Request& request, GetCollection::Response& response) {
        auto uuid = request.get_uuid();
        auto collection_name = request.get_name();
        CollectionName storage_services{CollectionName::StorageServices};
        CollectionName physical_drives{CollectionName::PhysicalDrives};
        CollectionName logical_drives{CollectionName::LogicalDrives};
        CollectionName iscsi_targets{CollectionName::iSCSITargets};

        if (storage_services.to_string() == collection_name) {
            // check if given uuid exists, throw exception if not
            CommonComponents::get_instance()->get_module_manager().get_entry(uuid);

            for (const auto& storage_service_uuid : get_manager<StorageServices>().get_keys(uuid)) {
                response.add_entry(storage_service_uuid);
            }
            return;
        }

        bool is_storage_service = get_manager<StorageServices>().entry_exists(uuid);

        // if not storage service, it must be one of logical drive or iSCSI target
        if (!is_storage_service &&
            !get_manager<LogicalDrive>().entry_exists(uuid) &&
            !get_manager<IscsiTarget>().entry_exists(uuid)) {
            THROW(agent_framework::exceptions::InvalidUuid, "storage-agent",
                  "Unknown UUID provided '" + uuid + "'.");
        }

        if (add_collection<PhysicalDrive>(physical_drives, collection_name,
                                          is_storage_service, uuid, response)) {
            return;
        }

        if (add_collection<LogicalDrive>(logical_drives, collection_name,
                                         is_storage_service, uuid, response)) {
            return;
        }

        if (add_collection<IscsiTarget>(iscsi_targets, collection_name,
                                        is_storage_service, uuid, response)) {
            return;
        }

        THROW(agent_framework::exceptions::InvalidCollection, "storage-agent",
            "Unknown collection requested: '" + collection_name + "'.");
    }

}

REGISTER_COMMAND(GetCollection, ::get_collection);
