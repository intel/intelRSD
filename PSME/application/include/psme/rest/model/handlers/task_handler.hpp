/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * */

#pragma once



#include "agent-framework/module/model/task.hpp"
#include "agent-framework/module/requests/common/get_task_info.hpp"
#include "agent-framework/module/requests/common/get_task_result_info.hpp"
#include "agent-framework/module/responses/common/get_task_result_info.hpp"
#include "agent-framework/module/requests/common/get_tasks_collection.hpp"
#include "psme/rest/server/error/error_factory.hpp"



namespace psme {
namespace rest {
namespace model {
namespace handler {

using agent_framework::model::requests::GetTaskInfo;
using agent_framework::model::requests::GetTaskResultInfo;
using agent_framework::model::Task;
using psme::rest::error::ErrorFactory;
using TaskHandlerBase = GenericHandler <GetTaskInfo, Task,
IdPolicy<agent_framework::model::enums::Component::Task,
    NumberingZone::SHARED>>;

/*! @brief TaskHandler template specialization */
class TaskHandler : public TaskHandlerBase {
public:
    TaskHandler() : TaskHandlerBase() {}


    virtual ~TaskHandler();


protected:
    template<typename T>
    using Array = agent_framework::model::attribute::Array<T>;
    using Tasks = Array<agent_framework::model::attribute::TaskEntry>;
    using SubcomponentEntry =
    agent_framework::model::attribute::SubcomponentEntry;


    Array<SubcomponentEntry> fetch_sibling_uuid_list(Context& ctx,
                                                     const std::string& parent_uuid,
                                                     const std::string& collection_name) override {

        try {
            if (Component::None == ctx.get_parent_component()) {

                log_debug(GET_LOGGER("rest"), ctx.indent
                    << "[" << static_cast<char>(ctx.mode) << "] "
                    << "Fetching list of all managers from agent "
                    << ctx.agent->get_gami_id());

                agent_framework::model::requests::GetTasksCollection
                    collection{};
                auto manager_entries = ctx.agent->execute<Tasks>(
                    collection);

                // Convert array of TaskEntry to Array of SubcomponentEntry
                Array<SubcomponentEntry> res;
                for (const auto& entry : manager_entries) {
                    res.add_entry(entry.get_task());
                }
                return res;
            }
            else {
                return TaskHandlerBase::fetch_sibling_uuid_list(ctx,
                                                                parent_uuid, collection_name);
            }
        }
        catch (const jsonrpc::JsonRpcException& e) {
            log_error(GET_LOGGER("rest"), ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Agent exception while fetching list of all components of "
                    "type [" << component_s() << "] for parent "
                << parent_uuid << e.what());
            return Array<SubcomponentEntry>();
        }
    }


private:
    Task fetch_entry(Context& ctx, const std::string& parent, const std::string& uuid) override {
        GetTaskInfo request{uuid};
        log_debug(GET_LOGGER("rest"), ctx.indent << "[" << char(ctx.mode) << "] "
                                                 << "Fetching [" << component_s() << " " << uuid << "]");

        try {
            auto elem = ctx.agent->execute<Task>(request);
            if (agent_framework::module::get_manager<Task>().entry_exists(uuid)) {
                const auto& task = agent_framework::module::get_manager<Task>().get_entry(uuid);
                elem.set_messages(task.get_messages());
                elem.set_completion_notifiers(task.get_completion_notifiers());
            }
            if (elem.get_state() == agent_framework::model::enums::TaskState::Exception) {
                try {
                    GetTaskResultInfo result_request{uuid};
                    ctx.agent->execute<agent_framework::model::responses::GetTaskResultInfo>(result_request);
                }
                catch (const psme::rest::error::ServerException& e) {
                    // Clear all messages to avoid duplication them infinitely
                    elem.clear_messages();
                    for (const auto& message_json : e.get_error().get_extended_messages()) {
                        agent_framework::model::attribute::Message message =
                            agent_framework::model::attribute::Message::from_json(message_json);
                        // hardcoded for now; to be fixed
                        message.set_related_properties({{"#/Id"}});
                        elem.add_message(message);
                    }
                }
            }
            elem.set_parent_uuid(parent);
            elem.set_uuid(uuid);
            elem.set_parent_type(ctx.get_parent_component());
            return elem;
        }
        catch (const jsonrpc::JsonRpcException& e) {
            log_error(GET_LOGGER("rest"),
                      ctx.indent << "[" << char(ctx.mode) << "] "
                                 << "RPC Error while fetching [" << component_s() << " - "
                                 << uuid << "]: " << e.what());
            throw;
        }
        catch (const psme::core::agent::AgentUnreachable& e) {
            log_error(GET_LOGGER("rest"), ctx.indent << "[" << char(ctx.mode) << "] "
                                                     << "RPC Error (AgentUnreachable) while fetching ["
                                                     << component_s() << " " << uuid << "] " << e.what());
            throw;
        }
        catch (...) {
            log_error(GET_LOGGER("rest"), "Other exception while fetching a "
                << component_s());
            throw;
        }
    }
};


TaskHandler::~TaskHandler() {}

}
}
}
}

