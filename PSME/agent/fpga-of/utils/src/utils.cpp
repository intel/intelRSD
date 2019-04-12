/*!
 * @brief Defines utils for FPGAoF agent
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file utils.cpp
 * */



#include "utils.hpp"
#include "agent-framework/eventing/utils.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/database/database_keys.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::eventing;
using namespace agent_framework::database;
using namespace agent::fpgaof;
using namespace agent::fpgaof::utils;

namespace {

void update_processor_status(const std::string& processor_uuid, const attribute::Status& status) {

    auto processor = get_manager<Processor>().get_entry_reference(processor_uuid);

    if (processor->get_is_being_erased()) {
        // we should not overwrite this states
        return;
    }

    attribute::Status prev_status = processor->get_status();
    if (!processor->get_is_in_warning_state() &&
        (prev_status.get_health() != status.get_health() || prev_status.get_state() != status.get_state())) {

        log_debug("agent", "Processor (" << processor_uuid << ") status changed: "
                                         << status.get_health() << ", " << status.get_state());
        processor->set_status(status);
        send_event(processor_uuid, enums::Component::Processor, enums::Notification::Update,
                   processor->get_parent_uuid());
    }
}


void update_processor_erased_db(const std::string& processor_uuid, bool erased) {
    ProcessorEntity(processor_uuid).put(PROCESSOR_ERASED_PROPERTY, std::to_string(static_cast<unsigned>(erased)));
}

}

namespace agent {
namespace fpgaof {
namespace utils {

void partition_target_and_initiator_endpoints(const std::vector<Uuid>& input_endpoints,
                                              std::vector<Uuid>& target_endpoints,
                                              std::vector<Uuid>& initiator_endpoints) {
    for (const auto& endpoint_uuid : input_endpoints) {
        if (Endpoint::is_target(endpoint_uuid)) {
            target_endpoints.push_back(endpoint_uuid);
        }
        else {
            initiator_endpoints.push_back(endpoint_uuid);
        }
    }
}


void set_processor_status(const std::string& processor_uuid,
                          const agent_framework::model::attribute::Status& status) {
    update_processor_status(processor_uuid, status);
}


void set_processor_erased(Processor& processor, bool erased) {
    auto fpga = processor.get_fpga();
    fpga.set_erased(erased);
    processor.set_fpga(fpga);
}


void set_processor_erased_in_model(const Uuid& processor_uuid, bool erased) {
    auto fpga = get_manager<Processor>().get_entry_reference(processor_uuid)->get_fpga();
    fpga.set_erased(erased);
    get_manager<Processor>().get_entry_reference(processor_uuid)->set_fpga(fpga);
}


void update_processor_erased(const std::string& processor_uuid, bool erased) {
    set_processor_erased_in_model(processor_uuid, erased);
    update_processor_erased_db(processor_uuid, erased);
}


void set_processor_is_in_warning_state(const std::string& processor_uuid, bool is_in_warning_state) {
    get_manager<Processor>().get_entry_reference(processor_uuid)->set_is_in_warning_state(is_in_warning_state);
}


void set_processor_is_being_erased(const std::string& processor_uuid, bool is_being_erased) {
    get_manager<Processor>().get_entry_reference(processor_uuid)->set_is_being_erased(is_being_erased);
}


void sync_processors_with_db(const Uuid& system_uuid, std::vector<agent_framework::model::Processor>& processors) {

    auto db_uuids = SystemEntity(system_uuid).get_multiple_values(PROCESSORS_PROPERTY);

    for (auto& processor : processors) {
        auto processor_in_db = std::find_if(std::begin(db_uuids), std::end(db_uuids),
                                            [&processor](const auto& db_uuid) {
                                                return db_uuid == processor.get_uuid();
                                            }) != std::end(db_uuids);
        if (processor_in_db) {
            bool erased = ProcessorEntity(processor.get_uuid()).get(PROCESSOR_ERASED_PROPERTY) == "1";
            set_processor_erased(processor, erased);
        }
        else {
            ProcessorEntity(processor.get_uuid()).put(PROCESSOR_ERASED_PROPERTY, std::to_string(
                static_cast<unsigned>(processor.get_fpga().get_erased())));
            SystemEntity(system_uuid).append(PROCESSORS_PROPERTY, processor.get_uuid());
        }
    }
}

}
}
}
