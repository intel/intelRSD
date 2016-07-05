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

#include "agent-framework/command/storage/add_iscsi_target.hpp"
#include "agent-framework/module/module_manager.hpp"
#include "agent-framework/module/iscsi_data.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/tgt/config/tgt_config.hpp"
#include "hotswap/hotswap_event.hpp"

using namespace agent_framework::command;
using namespace agent_framework::generic;
using namespace agent::storage::iscsi::tgt::config;

constexpr const char* TGT_INITIATOR_ADDRESS_ALL = "ALL";

/*! AddIscsiTarget implementation */
class AddIscsiTarget : public storage::AddIscsiTarget {
public:
    AddIscsiTarget() {}

    using storage::AddIscsiTarget::execute;

    void execute(const Request& request, Response& response) {
        if (ModuleManager::get_modules().empty()) {
            THROW(agent_framework::exceptions::IscsiError,
                 "storage-agent", "Module not found!");
        }

        auto& module = ModuleManager::get_modules().front();

        if (module->get_submodules().empty()) {
            THROW(agent_framework::exceptions::IscsiError,
                 "storage-agent", "Submodule not found!");
        }

        auto& submodule = module->get_submodules().front();
        auto& target_manager = submodule->get_target_manager();
        const auto target_id = target_manager.get_new_target_id();
        agent::storage::iscsi::tgt::Manager manager;

        create_target(manager, target_id, request.get_target_iqn());
        set_initiator_name(manager, target_id, request.get_initiator_iqn());
        set_initiator_address(manager, target_id, TGT_INITIATOR_ADDRESS_ALL);

        const auto& iscsi_data = submodule->get_iscsi_data();
        auto target = create_target_obj(target_id,
                                        request,
                                        iscsi_data);

        try {
            create_luns(manager, target, request);
        }
        catch(const std::exception&) {
            delete_target(manager, target_id);
            throw;
        }

        target_manager.add_target(target);
        log_debug(GET_LOGGER("storage-agent"), "iSCSI Target added: " <<
                                               target->get_target_iqn());

        TgtConfig tgtConfig(iscsi_data.get_configuration_path());
        try {
            tgtConfig.add_target(target);
        }
        catch (const std::exception& ex) {
            log_warning(GET_LOGGER("tgt"), "Unable to create TGT target " <<
                    "config file: " << ex.what());
        }

        response.set_target(target->get_uuid());
        response.set_oem(agent_framework::model::attribute::Oem());
    }

    void create_target(agent::storage::iscsi::tgt::Manager& manager,
                                const std::int32_t target_id,
                                const std::string& target_iqn) {
        auto target_res = manager.create_target(target_id, target_iqn);
        if (!target_res.is_valid()) {
            agent::storage::iscsi::tgt::Errors::throw_exception(
                    target_res.get_error());
        }
    }

    void delete_target(agent::storage::iscsi::tgt::Manager& manager,
                                        const std::int32_t target_id) {
        auto response = manager.destroy_target(target_id);

        if (!response.is_valid()) {
            agent::storage::iscsi::tgt::Errors::throw_exception(
                    response.get_error(), "Destroy target error: ");
        }
    }

    void set_initiator_name(agent::storage::iscsi::tgt::Manager& manager,
                                const std::int32_t target_id,
                                const std::string& initiator_name) {
        if (initiator_name.empty()) {
            log_debug(GET_LOGGER("storage-agent"), "No target initiator-name set");
            return;
        }
        agent::storage::iscsi::tgt::Manager::OptionMapper options;
        options.emplace(std::make_pair("initiator-name", initiator_name));
        auto response = manager.bind_target(target_id, options);
        if (!response.is_valid()) {
            agent::storage::iscsi::tgt::Errors::throw_exception(
                    response.get_error(),
                    "Cannot bind initiator-name to target: ");
        }
    }
    void set_initiator_address(agent::storage::iscsi::tgt::Manager& manager,
                                const std::int32_t target_id,
                                const std::string& initiator_address) {
        if (initiator_address.empty()) {
            log_debug(GET_LOGGER("storage-agent"), "No target initiator set");
            return;
        }
        agent::storage::iscsi::tgt::Manager::OptionMapper options;
        options.emplace(std::make_pair("initiator-address", initiator_address));
        auto response = manager.bind_target(target_id, options);
        if (!response.is_valid()) {
            agent::storage::iscsi::tgt::Errors::throw_exception(
                    response.get_error(),
                    "Cannot bind initiator-address to target: ");
        }
    }

    void create_luns(agent::storage::iscsi::tgt::Manager& manager,
                                                    TargetSharedPtr& target,
                                                    const Request& request) {
        for (const auto& target_lun : request.get_target_luns()) {
            auto drive =
                    ModuleManager::find_logical_drive(target_lun.get_logical_drive());
            auto drive_ptr = drive.lock();
            if (!drive_ptr) {
                THROW(agent_framework::exceptions::InvalidUuid,
                      "storage-agent", "Invalid logical drive UUID!");
            }

            auto lun_res = manager.create_lun(target->get_target_id(),
                                              target_lun.get_lun(),
                                            drive_ptr->get_device_path());
            if (!lun_res.is_valid()) {
                agent::storage::iscsi::tgt::Errors::throw_exception(
                        lun_res.get_error(),
                        "Create lun error: ");
            }
            target->add_target_lun(create_lun_obj(
                        target_lun.get_lun(),
                        drive_ptr));

            target->add_logical_drive(drive_ptr);
        }
    }

    Target::Lun
    create_lun_obj(const std::uint64_t lun,
                   const LogicalDrive::LogicalDriveSharedPtr& drive) {
        Target::Lun lun_obj;
        lun_obj.set_lun_id(lun);
        lun_obj.set_device_path(drive->get_device_path());
        lun_obj.set_logical_drive_uuid(drive->get_uuid());
        return lun_obj;
    }

    Target::TargetSharedPtr
    create_target_obj(const int32_t target_id,
                      const Request& request,
                      const IscsiData& iscsi_data) {
        auto target = std::make_shared<Target>();
        target->set_target_id(target_id);
        target->set_target_iqn(request.get_target_iqn());
        target->set_initiator_iqn(request.get_initiator_iqn());
        target->set_status({"Enabled", "OK"});
        target->set_target_address(iscsi_data.get_portal_ip());
        target->set_target_port(iscsi_data.get_portal_port());
        return target;
    }

    ~AddIscsiTarget();
};

AddIscsiTarget::~AddIscsiTarget() {}


static Command::Register<AddIscsiTarget> g("ConfigurationBased");
