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

#include "agent-framework/command/storage/get_iscsi_target_info.hpp"
#include "agent-framework/module/module_manager.hpp"

using namespace agent_framework::model;
using namespace agent_framework::command;
using namespace agent_framework::generic;

/*! Dummy GetIscsiTargetInfo implementation */
class GetIscsiTargetInfo : public storage::GetIscsiTargetInfo {
public:
    GetIscsiTargetInfo() {}

    using storage::GetIscsiTargetInfo::execute;

    void execute(const Request& request, Response& response) {
       const auto target = ModuleManager::find_target(
                               request.get_target()).lock();
        if (!target) {
            THROW(agent_framework::exceptions::InvalidUuid, "storage-agent",
                  "iSCSI Target = '" + request.get_target() + "' not found");
        }

        IscsiTarget itarget;
        itarget.set_status(attribute::Status(
                           target->get_status().get_state(),
                           target->get_status().get_health()));
        itarget.set_initiator_iqn(target->get_initiator_iqn());
        itarget.set_target_address(target->get_target_address());
        std::stringstream stream;
        stream << target->get_target_port();
        itarget.set_target_port(stream.str());
        itarget.set_target_iqn(target->get_target_iqn());
        for (const auto& lun : target->get_target_lun()) {
            itarget.add_target_lun(attribute::TargetLun(
                                   uint32_t(lun.get_lun_id()),
                                   lun.get_logical_drive_uuid()));
        }
        itarget.set_oem(attribute::Oem());
        response.set_target(itarget);
    }

    ~GetIscsiTargetInfo();
};

GetIscsiTargetInfo::~GetIscsiTargetInfo() {}


static Command::Register<GetIscsiTargetInfo> g("ConfigurationBased");
