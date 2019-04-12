/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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



#include "agent-framework/module/managers/generic_manager_registry.hpp"
#include "psme/rest/model/handlers/handler_interface.hpp"
#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/id_policy.hpp"
#include "psme/rest/eventing/event.hpp"
#include "psme/rest/server/error/server_exception.hpp"
#include "psme/core/agent/agent_unreachable.hpp"



namespace psme {
namespace rest {

namespace endpoint {
class RollupTest;
}

namespace model {
namespace handler {

/*!
 * @brief Class template that is instantiated for each Model (derived from Resource class).
 *
 * @tparam Request class that represents message that is sent to agent to retrieve data
 * @tparam Model class of Resource that is returned by agent
 * @tparam IdPolicy Different resources have different rules of assigning rest IDs.
 *
 * How it works:
 * 1. This class template is being instantiated for every type of Resource.
 * (e.g. 'System' has its own handler, 'Processor' has its own handler and so on.)
 * 2. when event arrives, Watcher passes it to appropriate handler. When event says that
 * System was added, it finds SystemHandler and executes handle(...) method
 */
template<typename Request, typename Model, typename IdPolicy>
class GenericHandler : public HandlerInterface {
public:
    using GetCollectionReq = agent_framework::model::requests::GetCollection;
    using IdMap = std::map<std::string, std::uint64_t>;
    using Event = psme::rest::eventing::Event;
    using SubscriptionManager = psme::rest::eventing::manager::SubscriptionManager;
    using UpdateStatus = typename agent_framework::module::GenericManager<Model>::UpdateStatus;
    using SubcomponentEntry = agent_framework::model::attribute::SubcomponentEntry;


    /*!
     * @brief default constructor
     */
    GenericHandler() {}


    virtual ~GenericHandler() {}


    /* methods inheriting doxygen comments from base class: */

    virtual void remove(const std::string& uuid) override;


    virtual void remove_agent_data(Context& ctx, const std::string& gami_id) override;


    virtual bool handle(JsonAgentSPtr agent, const EventData& event, EventVec& northbound_events) override;


    virtual void poll(JsonAgentSPtr agent,
                      const std::string& parent, const Component parent_component,
                      const std::string& uuid) override;


    virtual std::uint64_t load(JsonAgentSPtr agent,
                               const std::string& parent, const Component parent_component,
                               const std::string& uuid, bool recursively = false) override;


    virtual void load_collection(JsonAgentSPtr agent,
                                 const std::string& parent, const Component parent_component,
                                 const CollectionType& collection_type, bool recursively) override;


    virtual std::uint64_t get_manager_epoch() override;


    virtual std::uint64_t
    do_load(Context& ctx, const std::string& parent, const std::string& uuid, bool recursively) override;


protected:

    /* methods inheriting doxygen comments from base class: */

    virtual Component get_component() override {
        return Model::get_component();
    }


    virtual void
    fetch_siblings(Context& ctx, const std::string& parent_uuid, const std::string& collection_name) override;


    virtual agent_framework::model::attribute::Array<agent_framework::model::attribute::SubcomponentEntry>
    fetch_sibling_uuid_list(Context& ctx, const std::string& parent_uuid, const std::string& collection_name) override;


    virtual void remove_untouched(Context& ctx, const std::string& parent_uuid, std::uint64_t epoch) override;


    virtual std::uint64_t update(Context& ctx, const std::string& parent, const std::string& uuid) override;


    virtual void remove_all(Context& ctx, const std::string& parent) override;

    /* other methods: */

    /*!
     * @brief Implements actions that will be taken after ADD event from agent
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent UUID of parent node
     * @param[in] uuid UUID of the node to reload
     * @param[in] recursively Do you want to retrieve descendants as well?
     */
    virtual std::uint64_t add(Context& ctx, const std::string& parent, const std::string& uuid, bool recursively);


    /*!
     * @brief Downloads recursively all subcollections
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent uuid of parent node
     * @param[in] collections Array of collections to download
     */
    void fetch_subcomponents(Context& ctx, const std::string& parent, const Array <Collection>& collections);


    /*!
     * @brief removes entity with given uuid
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] uuid uuid of entity to remove
     */
    void remove_single(Context& ctx, const std::string& uuid);


    /*!
     * @brief removes given uuid and all its descendants
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] uuid uuid of entity to remove
     */
    virtual void do_remove(Context& ctx, const std::string& uuid);


    /*!
     * @brief Guard to make sure that event is handled by correct handler
     *
     * @param[in] event event to be processed
     *
     * @return true is the handler is correct addressee
     */
    bool is_event_for_me(const EventData& event) const {
        return event.get_type() == Model::get_component();
    }


    /*!
     * @brief Text version of component.
     *
     * @return component as text
     */
    const std::string component_s() const {
        return Model::get_component().to_string();
    }


    /*!
     * @brief Hierarchy is not known upfront so handler needs to learn to be able
     * to support 'remove'
     *
     * @param[in] sub_handler Handler of sub-component to remember
     */
    void remember_sub_handler(HandlerInterface* sub_handler) {
        m_sub_components.insert(sub_handler->get_component());
    }


    /*!
     * @brief assigns rest id number to new resource
     *
     * @param[in] uuid uuid of resource that needs id
     * @param[in] parent_uuid uuid of parent of resource that needs id
     *
     * @return rest id that should be assigned to resource
     */
    std::uint64_t get_id(const std::string& uuid, const std::string& parent_uuid) {
        return m_id_policy.get_id(uuid, parent_uuid);
    }


    virtual void accept(ResourceVisitor& visitor, const std::string& uuid) override;


    virtual bool do_accept(ResourceVisitor& visitor, const std::string& parent_uuid) override;


    virtual bool do_accept_recursively(ResourceVisitor& visitor,
                                       const std::string& parent_uuid,
                                       const Component parent_component) override;


    /*!
     * @brief Function template compares two sets of uuid and creates vectors of mappings to add and to remove
     * @param fetched List of fetched uuids
     * @param parent_uuid parent uuid
     * @param many2many manager that holds mappings
     * @param out_to_add uuids to add will be added to this vector
     * @param out_to_remove uuids to remove will be added to this vector
     * */
    void what_to_add_what_to_remove(
        agent_framework::model::attribute::Array<agent_framework::model::attribute::SubcomponentEntry> fetched,
        const std::string& parent_uuid, agent_framework::module::managers::ManyToManyManager& many2many,
        std::vector<std::string>& out_to_add,
        std::vector<std::string>& out_to_remove);


    /*!
     * @brief Fetches weak collection children
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent_uuid uuid of parent node
     * @param[in] collection_name name of collection of elements to fetch
     * @param[in] many2many manager for weak realtions
     */
    void
    fetch_parent_children(psme::rest::model::handler::HandlerInterface::Context& ctx,
                          const std::string& parent_uuid,
                          const std::string& collection_name,
                          agent_framework::module::managers::ManyToManyManager& many2many);


    /*!
     * @brief finds component enum value of given uuid.
     *
     * To implement this, it iterates over registered GenericManagers
     * @param[in] uuid uuid of resource to look up

     * @return component of given resource or None if not found
     */
    agent_framework::model::enums::Component find_component(const std::string& uuid);


    std::set<Component> m_sub_components{}; // each handler learns set of possible subcomponents
    agent_framework::model::enums::CollectionName m_collection_name{Model::get_collection_name()};
    IdPolicy m_id_policy{};

    friend class HandlerTest;
    friend class FabricHandlersTest;
    friend class psme::rest::endpoint::RollupTest;

private:

    Model fetch(Context& ctx, const std::string& parent, const std::string& uuid, UpdateStatus& update_status);


    virtual Model fetch_entry(Context& ctx, const std::string& parent, const std::string& uuid);


    UpdateStatus add_to_model(Context& ctx, Model& entry);
};


template<typename Request, typename Model, typename IdPolicy>
bool GenericHandler<Request, Model, IdPolicy>::handle(JsonAgentSPtr agent, const EventData& event,
    EventVec& northbound_events) {

    assert(is_event_for_me(event));

    Context ctx;
    ctx.agent = agent.get();
    ctx.mode = Context::Mode::EVENT;
    Component parent_component = find_component(event.get_parent());
    ctx.stack.emplace(parent_component);

    log_info("rest", ctx.indent << ctx.indent << "[" << char(ctx.mode) << "] "
                                            << "Agent event received: " << event.get_notification().to_string()
                                            << " " << component_s() << " uuid=" << event.get_component() << ", "
                                            << "parent uuid=" << event.get_parent() << ", agent_id="
                                            << agent->get_gami_id());
    try {
        switch (event.get_notification()) {
            case Notification::Add: {
                try {
                    if (agent_framework::module::get_manager<Model>().entry_exists(event.get_component())) {
                        log_info("rest", ctx.indent << ctx.indent << "[" << char(ctx.mode) << "] "
                                                                << "Duplicated or late event. Ignoring.");
                        return true;
                    }
                    // add may throw
                    add(ctx, event.get_parent(), event.get_component(), true);
                    northbound_events = ctx.events;
                    return true;
                }
                catch (const psme::core::agent::AgentUnreachable&) {
                    // in case of connection error, added element plus all his descendants will be removed from
                    // local "database". Events collected in Context will not be returned in northbound_events
                    log_error("rest",
                              ctx.indent << "Exception while handling Agent Event. Removing non-complete data.");
                    do_remove(ctx, event.get_component());
                    return false;
                }
            }
            case Notification::Remove: {
                try {
                    auto uuid_to_remove = event.get_component();
                    do_remove(ctx, uuid_to_remove);
                    northbound_events = ctx.events;
                }
                catch (const agent_framework::exceptions::InvalidValue&) {
                    log_info("rest", "Got notification about removal"
                        << "of a component, but component is not present in the model.");
                }
                catch (const agent_framework::exceptions::NotFound&) {
                    log_info("rest", "Got notification about removal"
                        << "of a component, but component is not present in the model.");
                }
                return true;
            }
            case Notification::Update: {
                // update may throw
                if (!agent_framework::module::get_manager<Model>().entry_exists(event.get_component())) {
                    log_error("rest", "Update event for unknown entry "
                        << event.get_component() << " of type "
                        << event.get_type().to_string() << ". Ignoring.");
                    return true;
                }
                update(ctx, event.get_parent(), event.get_component());
                northbound_events = ctx.events;
                return true;
            }
            default:
                log_error("rest", "Unknown agent event received: " << event.get_notification().to_string());
                break;
        }
    }
    catch (const core::agent::AgentUnreachable& e) {
        log_error("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                 << "AgentEvent Handling failed due to agent connection error: "
                                                 << e.what());
    }
    catch (const json_rpc::JsonRpcException& e) {
        log_error("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                 << "AgentEvent Handling failed due to JsonRpcException: "
                                                 << e.what());
    }
    catch (...) {
        log_error("rest", ctx.indent << "Exception while handling Agent Event. State may be incomplete.");
        return false;
    }

    return false;
}


template<typename Request, typename Model, typename IdPolicy>
std::uint64_t GenericHandler<Request, Model, IdPolicy>::get_manager_epoch() {
    return agent_framework::module::get_manager<Model>().get_current_epoch();
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
::poll(JsonAgentSPtr agent,
       const std::string& parent_uuid, const Component parent_component,
       const std::string& uuid) {

    Context ctx;
    ctx.agent = agent.get();
    ctx.mode = Context::Mode::POLLING;
    ctx.stack.emplace(parent_component);

    try {
        log_info("rest", ctx.indent << "[" << char(ctx.mode) << "] " << "Polling started on "
                                                << component_s() << " uuid=" << uuid);
        try {
            add(ctx, parent_uuid, uuid, true /*recursively*/); // add may throw
        }
        catch (const json_rpc::JsonRpcException&) {}

        SubscriptionManager::get_instance()->notify(ctx.events);
    }
    catch (const core::agent::AgentUnreachable&) {
        log_error("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                 << "Polling failed due to agent error (unreachable).");
    }

    if (ctx.num_removed > 0) {
        log_info("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                << "#removed : " << ctx.num_removed);
    }
    if (ctx.num_added > 0) {
        log_info("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                << "#added : " << ctx.num_added);
    }
    if (ctx.num_updated > 0) {
        log_info("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                << "#updated: " << ctx.num_updated);
    }
    if (ctx.num_status_changed > 0) {
        log_info("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                << "#status changed: " << ctx.num_status_changed);
    }
    if (ctx.num_alerts > 0) {
        log_info("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                << "#alerts: " << ctx.num_alerts);
    }
}


template<typename Request, typename Model, typename IdPolicy>
std::uint64_t GenericHandler<Request, Model, IdPolicy>
::do_load(Context& ctx, const std::string& parent, const std::string& uuid, bool recursively) {

    assert(parent != uuid);

    log_info("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                            << "Loading [" << component_s() << " " << uuid << ", parent_uuid: "
                                            << parent << "]");

    return add(ctx, parent, uuid, recursively);
}


template<typename Request, typename Model, typename IdPolicy>
std::uint64_t GenericHandler<Request, Model, IdPolicy>
::load(JsonAgentSPtr agent, const std::string& parent, const Component parent_component,
       const std::string& uuid, bool recursively) {

    assert(parent != uuid);

    Context ctx;
    ctx.agent = agent.get();
    ctx.mode = Context::Mode::LOADING;
    ctx.stack.emplace(parent_component);

    auto rest_id = do_load(ctx, parent, uuid, recursively);
    SubscriptionManager::get_instance()->notify(ctx.events);

    return rest_id;
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
::load_collection(JsonAgentSPtr agent, const std::string& parent, const Component parent_component,
                  const CollectionType& collection_type, bool recursively) {
    Context ctx;
    ctx.agent = agent.get();
    ctx.mode = Context::Mode::LOADING;
    ctx.stack.emplace(parent_component);
    auto indent = ctx.indent;
    ctx.indent += "    ";

    log_info("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                            << "Loading collections of type [" << collection_type << "] for ["
                                            << component_s() << " " << parent << "]");

    Request request{parent};
    const Model entry = ctx.agent->execute<Model>(request);

    std::vector<Collection> collections{};
    std::for_each(entry.get_collections().get_array().begin(), entry.get_collections().get_array().end(),
                  [&collection_type, &collections](const Collection& element) {
                      if (element.get_type() == collection_type) {
                          collections.push_back(element);
                      }
                  }
    );

    if (collections.empty()) {
        log_error("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                 << "No collection of type \'" << collection_type
                                                 << "\' for [" << component_s() << " " << parent << "]");
    }

    if (recursively) {
        fetch_subcomponents(ctx, parent, collections);
    }
    else {
        // same as in fetch_siblings, this method maps component types (aka collection types)
        // to the epoch from before loading in order to know what we should remove
        std::map<CollectionType, uint64_t> epochs{};
        for (const auto& collection : collections) {
            log_info("rest", ctx.indent << "[" << char(ctx.mode)
                                                    << "] Collection [" << collection.get_name() << "]");
            auto handler = HandlerManager::get_instance()->get_handler(collection.get_type());
            if (epochs.find(collection.get_type()) == epochs.end()) {
                epochs[collection.get_type()] = handler->get_manager_epoch();
            }
            const auto siblings = handler->fetch_sibling_uuid_list(ctx, parent, collection.get_name());
            for (const auto& sibling: siblings) {
                handler->do_load(ctx, parent, sibling.get_subcomponent(), false);
            }
        }
        // removing the components we did not touch
        for (const auto& collection_epoch : epochs) {
            HandlerManager::get_instance()->get_handler(collection_epoch.first)
                ->remove_untouched(ctx, parent, collection_epoch.second);
        }
    }
    ctx.indent = indent;
    SubscriptionManager::get_instance()->notify(ctx.events);
}


template<typename Request, typename Model, typename IdPolicy>
std::uint64_t GenericHandler<Request, Model, IdPolicy>
::add(Context& ctx, const std::string& parent, const std::string& uuid, bool recursively) {
    UpdateStatus update_status{};
    const Model entry = fetch(ctx, parent, uuid, update_status);
    if (recursively) {
        auto indent = ctx.indent;
        ctx.indent += "    ";

        fetch_subcomponents(ctx, uuid, entry.get_collections());

        ctx.indent = indent;
    }
    return entry.get_id();
}


template<typename Request, typename Model, typename IdPolicy>
std::uint64_t GenericHandler<Request, Model, IdPolicy>
::update(Context& ctx, const std::string& parent, const std::string& uuid) {
    return do_load(ctx, parent, uuid, false);
}


template<typename Request, typename Model, typename IdPolicy>
typename GenericHandler<Request, Model, IdPolicy>::UpdateStatus GenericHandler<Request, Model, IdPolicy>
::add_to_model(Context& ctx, Model& entry) {

    auto status = agent_framework::module::get_manager<Model>().add_or_update_entry(entry);

    auto log_event = [this, &entry, &ctx] (const std::string& what_happened) {
        log_debug("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                     << what_happened << " [" << this->component_s() << " " << entry.get_uuid()
                                     << ", parent_uuid: " << entry.get_parent_uuid()
                                     << ", id: " << entry.get_id() << "]");
    };

    if (UpdateStatus::StatusChanged == status) {
        ctx.add_event(get_component(), eventing::EventType::StatusChange, entry.get_uuid());
        ctx.num_status_changed++;
        log_event("Status changed");
    }
    if (UpdateStatus::StatusChanged == status
            && entry.get_status().get_health() == agent_framework::model::enums::Health::Critical) {
        ctx.add_event(get_component(), eventing::EventType::Alert, entry.get_uuid());
        ctx.num_alerts++;
        log_event("Critical health alert");
    }
    if (UpdateStatus::StatusChanged == status || UpdateStatus::Updated == status) {
        ctx.add_event(get_component(), eventing::EventType::ResourceUpdated, entry.get_uuid());
        ctx.num_updated++;
        log_event("Updated");
    }
    if (UpdateStatus::Added == status) {
        ctx.add_event(get_component(), eventing::EventType::ResourceAdded, entry.get_uuid());
        ctx.num_added++;
        log_event("Added");
    }

    return status;
}


template<typename Request, typename Model, typename IdPolicy>
Model GenericHandler<Request, Model, IdPolicy>
::fetch(Context& ctx, const std::string& parent, const std::string& uuid, UpdateStatus& update_status) {

    auto entry = fetch_entry(ctx, parent, uuid);
    entry.set_id(get_id(uuid, parent));
    entry.set_agent_id(ctx.agent->get_gami_id());
    assert(!entry.get_agent_id().empty());

    update_status = add_to_model(ctx, entry);

    return entry;
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
::fetch_subcomponents(Context& ctx, const std::string& parent, const Array <Collection>& collections) {

    auto sub_handlers = ::psme::rest::model::handler::HandlerManager::get_instance()->get_handlers(collections);
    ctx.stack.emplace(get_component());

    // epochs will be used to find out which nodes were not read from agent during polling.
    // if something was not read, we assume that was deleted and should be removed from internal database
    // (as well as its descendants).
    // this map maps each CollectionType (and manager, they have 1-1 relation) to the epoch before
    // loading these components - a vector is not enough because of the possibility of having multiple
    // collections of the same type (with different names)
    std::map<Component, uint64_t> epochs{};

    for (auto sub_handler = sub_handlers.begin(); sub_handler != sub_handlers.end(); ++sub_handler) {
        try {
            // we only want the lowest epoch
            if (epochs.find((*sub_handler).second->get_component()) == epochs.end()) {
                epochs[(*sub_handler).second->get_component()] = (*sub_handler).second->get_manager_epoch();
            }
            (*sub_handler).second->fetch_siblings(ctx, parent, (*sub_handler).first);
            remember_sub_handler((*sub_handler).second);
        }
        catch (const psme::core::agent::AgentUnreachable&) {
            throw; // connection error should stop transaction (polling, event handling)
        }
            /*
             * other exceptions should not stop us from reading.
             * note, that error was already logged
             */
        catch (...) {}
    }

    // After all fetching we remove nodes that could not be found
    for (auto& sub_handler : sub_handlers) {
        sub_handler.second->remove_untouched(ctx, parent, epochs[sub_handler.second->get_component()]);
    }
    ctx.stack.pop();
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
::remove_untouched(Context& ctx, const std::string& parent_uuid, std::uint64_t epoch) {
    if (Context::Mode::POLLING == ctx.mode || Context::Mode::LOADING == ctx.mode) {
        auto not_updated = agent_framework::module::get_manager<Model>().get_keys(parent_uuid,
                                                                                  [&epoch, &ctx](const Model& entry) {
                                                                                      return (entry.get_agent_id() ==
                                                                                              ctx.agent->get_gami_id() &&
                                                                                              !entry.was_touched_after(
                                                                                                  epoch));
                                                                                  }
        );

        for (const auto& to_remove_uuid : not_updated) {
            std::string updated_by = Context::Mode::POLLING == ctx.mode ? "polling" : "loading";
            log_debug("rest", "[" << char(ctx.mode) << "] [" << component_s()
                                              << " = " << to_remove_uuid << "] not found by " << updated_by
                                              << ". Removing.");
            do_remove(ctx, to_remove_uuid);
            ctx.num_removed++;
        }
    }
}


template<typename Request, typename Model, typename IdPolicy>
Model GenericHandler<Request, Model, IdPolicy>
::fetch_entry(Context& ctx, const std::string& parent, const std::string& uuid) {

    Request request{uuid};
    log_debug("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                             << "Fetching [" << component_s() << " " << uuid << "]");
    try {
        auto element = ctx.agent->execute<Model>(request);
        element.set_parent_uuid(parent);
        element.set_uuid(uuid);
        element.set_parent_type(ctx.get_parent_component());
        return element;
    }
    catch (const json_rpc::JsonRpcException& e) {
        log_error("rest",
                  ctx.indent << "[" << char(ctx.mode) << "] "
                             << "RPC Error while fetching [" << component_s() << " - "
                             << uuid << "]: " << e.what());
        throw;
    }
    catch (const psme::core::agent::AgentUnreachable& e) {
        log_error("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                 << "RPC Error (AgentUnreachable) while fetching ["
                                                 << component_s() << " " << uuid << "] " << e.what());
        throw;
    }
    catch (const psme::rest::error::ServerException& e) {
        log_error("rest", ctx.indent << "[" << char(ctx.mode) << "] "
                                                 << "Server Error while fetching ["
                                                 << component_s() << " " << uuid << "] "
                                                 << e.get_error().as_string());
        throw;
    }
    catch (const std::exception& e) {
        log_error("rest", "Exception while fetching a " << component_s() << ": " << e.what());
        throw;
    }
    catch (...) {
        log_error("rest", "Other exception while fetching a " << component_s());
        throw;
    }
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
::fetch_siblings(Context& ctx, const std::string& parent_uuid,
                 const std::string& collection_name) {
    auto subcomponents = fetch_sibling_uuid_list(ctx, parent_uuid,
                                                 collection_name);
    for (const auto& subcomponent_entry : subcomponents) {
        try {
            add(ctx, parent_uuid, subcomponent_entry.get_subcomponent(), true);
        }
        catch (const psme::core::agent::AgentUnreachable&) {
            // no log. already handled
            throw;
        }
        catch (...) {} // ignore other errors, continue
    }
}


template<typename Request, typename Model, typename IdPolicy>
agent_framework::model::attribute::Array<agent_framework::model::attribute::SubcomponentEntry>
GenericHandler<Request, Model, IdPolicy>
::fetch_sibling_uuid_list(Context& ctx,
                          const std::string& parent_uuid,
                          const std::string& collection_name) {
    GetCollectionReq collection{parent_uuid, collection_name};

    log_debug("rest", ctx.indent
        << "[" << char(ctx.mode) << "] "
        << "Fetching list of all components of type [" << component_s()
        << "] from collection [" << collection_name
        << "] for parent " << parent_uuid);
    try {
        auto res = ctx.agent->execute < Array < SubcomponentEntry >> (collection);
        log_debug("rest", ctx.indent
            << "[" << char(ctx.mode) << "] "
            << "Got " << res.get_array().size());
        return res;
    }
    catch (const psme::core::agent::AgentUnreachable& e) {
        log_error("rest", ctx.indent
            << "[" << char(ctx.mode) << "] "
            << "Agent connection error while fetching list of all"
                "components of type [" << component_s()
            << "] from collection [" << collection_name
            << "] for parent " << parent_uuid
            << " : " << e.what());
        throw; // connection error - stop transaction
    }
    catch (const json_rpc::JsonRpcException& e) {
        log_error("rest", ctx.indent
            << "[" << char(ctx.mode) << "] "
            << "Agent exception while fetching list of"
                "all components of type [" << component_s()
            << "] from collection [" << collection_name
            << "] for parent " << parent_uuid
            << " : " << e.what());

        // this error may result incompatibility of rest and agent. we should not stop transaction in progress
        // be nice and return empty list
        return agent_framework::model::attribute::Array<agent_framework::model::attribute::SubcomponentEntry>();
    }
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>::remove(const std::string& uuid) {
    Context ctx;
    ctx.mode = Context::Mode::USER_ACTION;
    do_remove(ctx, uuid);
    SubscriptionManager::get_instance()->notify(ctx.events);
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>::do_remove(Context& ctx, const std::string& uuid) {
    for (auto component = m_sub_components.begin(); component != m_sub_components.end(); ++component) {
        auto handler = ::psme::rest::model::handler::HandlerManager::get_instance()->get_handler(*component);
        handler->remove_all(ctx, uuid);
    }
    remove_single(ctx, uuid);
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>::remove_all(Context& ctx, const std::string& parent) {

    auto siblings = agent_framework::module::get_manager<Model>().get_keys(parent);
    for (const auto& uuid : siblings) {
        do_remove(ctx, uuid);
    }
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>::remove_single(Context& ctx, const std::string& uuid) {
    ctx.add_event(get_component(), eventing::EventType::ResourceRemoved, uuid);

    auto parent_uuid = agent_framework::module::get_manager<Model>().get_entry(uuid).get_parent_uuid();
    log_info("rest", ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                            << "Removing [" << component_s() << " " << uuid
                                            << ", parent_uuid: " << parent_uuid
                                            << ", id: "
                                            << agent_framework::module::get_manager<Model>().uuid_to_rest_id(uuid)
                                            << "] from the model");

    agent_framework::module::get_manager<Model>().remove_entry(uuid);

    log_debug("db", "remove single " << parent_uuid << "." << uuid);
    m_id_policy.purge(uuid, parent_uuid); // we do not want to have same rest id when FRU is reinserted
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>::remove_agent_data(Context& ctx, const std::string& gami_id) {
    auto agent_predicate = [&gami_id](const Model& entry) { return entry.get_agent_id() == gami_id; };
    const auto uuids_to_remove = agent_framework::module::get_manager<Model>().get_keys(agent_predicate);

    for (const auto& uuid : uuids_to_remove) {
        log_debug("db", "remove agent data " << uuid);
        do_remove(ctx, uuid);
    }
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>::accept(ResourceVisitor& visitor, const std::string& uuid) {
    visitor.visited.clear();
    do_accept(visitor, uuid);
}


template<typename Request, typename Model, typename IdPolicy>
bool GenericHandler<Request, Model, IdPolicy>::do_accept(ResourceVisitor& visitor, const std::string& uuid) {
    if (visitor.visited.end() == visitor.visited.find(uuid)) {
        // limit the scope of entry reference so the lock is released
        // when the reference is not needed
        {
            auto ref = agent_framework::module::get_manager<Model>().get_entry_reference(uuid);
            if (!visitor.visit(ref.get_raw_ref(), get_component())) {
                return false; // break
            }
        }
        visitor.visited.insert(uuid);

        for (auto component = m_sub_components.begin(); component != m_sub_components.end(); ++component) {
            auto handler = ::psme::rest::model::handler::HandlerManager::get_instance()->get_handler(*component);
            if (!handler->do_accept_recursively(visitor, uuid, get_component())) {
                return false; // break
            }
        }
    }
    return true;
}


template<typename Request, typename Model, typename IdPolicy>
bool GenericHandler<Request, Model, IdPolicy>::do_accept_recursively(ResourceVisitor& visitor,
                                                                     const std::string& parent_uuid,
                                                                     const Component /*parent_component*/) {
    auto children = agent_framework::module::get_manager<Model>().get_keys(parent_uuid);
    for (const std::string& child_uuid : children) {
        if (!do_accept(visitor, child_uuid)) {
            return false; // break
        }
    }
    return true;
}


template<typename Request, typename Model, typename IdPolicy>
agent_framework::model::enums::Component
GenericHandler<Request, Model, IdPolicy>::find_component(const std::string& uuid) {
    auto registry = agent_framework::module::GenericManagerRegistry::get_instance();

    Component component = Component::None;
    registry->for_each_table([&uuid, &component](agent_framework::module::TableInterface* table) {
        if (table->entry_exists(uuid)) {
            component = table->get_component();
            return false; // break
        }
        return true; // continue for_each_table
    });
    return component;
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>::what_to_add_what_to_remove(
    agent_framework::model::attribute::Array<agent_framework::model::attribute::SubcomponentEntry> fetched,
    const std::string& parent_uuid, agent_framework::module::managers::ManyToManyManager& many2many,
    std::vector<std::string>& out_to_add,
    std::vector<std::string>& out_to_remove) {

    auto previous = many2many.get_children(parent_uuid);
    auto new_set = std::set<std::string>();
    for (const auto& f : fetched) {
        new_set.insert(f.get_subcomponent());
    }
    auto old_set = std::set<std::string>(previous.begin(), previous.end());

    out_to_add.resize(new_set.size());
    out_to_remove.resize(old_set.size());

    auto it_add = std::set_difference(new_set.begin(), new_set.end(),
                                      old_set.begin(), old_set.end(),
                                      out_to_add.begin());
    out_to_add.resize(static_cast<size_t>(it_add - out_to_add.begin()));

    auto it_rem = std::set_difference(old_set.begin(), old_set.end(),
                                      new_set.begin(), new_set.end(),
                                      out_to_remove.begin());
    out_to_remove.resize(static_cast<size_t>(it_rem - out_to_remove.begin()));
}


template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>::fetch_parent_children(
    psme::rest::model::handler::HandlerInterface::Context& ctx,
    const std::string& parent_uuid,
    const std::string& collection_name,
    agent_framework::module::managers::ManyToManyManager& many2many) {

    const auto parent_type = ctx.get_parent_component().to_string();
    const auto child_type = Model::get_component().to_string();
    log_debug("rest", ctx.indent
        << "[" << static_cast<char>(ctx.mode) << "] "
        << "Assigning " << collection_name << " to " << parent_type << " " << parent_uuid);

    const auto fetched = fetch_sibling_uuid_list(ctx, parent_uuid,
                                                 collection_name);
    std::vector<std::string> to_add, to_remove;

    what_to_add_what_to_remove(fetched,
                               parent_uuid,
                               many2many,
                               to_add,
                               to_remove);

    for (const auto& uuid : to_add) {
        log_debug("rest", ctx.indent
            << "[" << static_cast<char>(ctx.mode) << "] "
            << "Assigning " << child_type << " " << uuid << " to " << parent_type << " "
            << parent_uuid);
        many2many.add_entry(parent_uuid, uuid, ctx.agent->get_gami_id());
    }

    for (const auto& uuid : to_remove) {
        log_debug("rest", ctx.indent
            << "[" << static_cast<char>(ctx.mode) << "] "
            << "Disassociate " << child_type << " " << uuid << " from " << parent_type << " "
            << parent_uuid);
        many2many.remove_entry(parent_uuid, uuid);
    }
}

}
}
}
}
