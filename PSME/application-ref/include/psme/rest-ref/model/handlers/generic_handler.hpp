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
 * */

#pragma once

#include "psme/rest-ref/model/handlers/handler_interface.hpp"
#include "psme/rest-ref/model/handlers/handler_manager.hpp"
#include "psme/rest-ref/model/handlers/id_policy.hpp"
#include "psme/rest-ref/eventing/event.hpp"
#include "psme/core/agent/agent_unreachable.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {

using agent_framework::eventing::EventData;

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
     * @brief - default constructor
     */
    GenericHandler() { }

    /* methods inheriting doxygen comments from base class: */

    virtual void remove(const std::string &uuid) override;

    virtual void remove_agent_data(const std::string & gami_id) override;

    virtual bool handle(JsonAgentSPtr agent, const EventData &event) override;

    virtual void poll(JsonAgentSPtr agent, const std::string& parent_uuid, const std::string& uuid) override;

    virtual std::uint64_t load(JsonAgentSPtr agent, const std::string &parent,
                       const std::string &uuid, bool recursively = false) override;

    virtual void load_collection(JsonAgentSPtr agent, const std::string &parent,
                                   const CollectionType& collection_type, bool recursively) override;

    virtual std::uint64_t get_manager_epoch() override;

    /* other methods: */

    /*!
     * @brief can be used by client to load resource on-demand.
     *
     * @param[in] parent - uuid of parent node
     * @param[in] uuid - uuid of the node to reload
     * @param[in] recursively - do you want to retrieve descendats as well?
     *
     * @return rest id of top-most loaded element
     *
     * Will be used by post,put,patch endponts to reload data from agent
     */
    std::uint64_t do_load(Context& ctx, const std::string &parent, const std::string &uuid, bool recursively);

protected:

    /* methods inheriting doxygen comments from base class: */

    virtual Component get_component() override {
        return Model::get_component();
    }

    virtual void fetch_siblings(Context& ctx, const std::string &parent_uuid, const std::string &collection_name) override;

    virtual agent_framework::model::attribute::Array<agent_framework::model::attribute::SubcomponentEntry>
            fetch_sibling_uuid_list(Context& ctx, const std::string &parent_uuid, const std::string &collection_name) override;

    virtual void remove_untouched(Context& ctx, const std::string &parent_uuid, std::uint64_t epoch) override;

    virtual std::uint64_t update(Context& ctx, const std::string &parent, const std::string &uuid) override;

    virtual void remove_all(const std::string &parent) override;

    /* other methods: */

    /*!
     * @brief Implements actions that will be taken after ADD event from agent
     *
     * @param[in] parent - uuid of parent node
     * @param[in] uuid - uuid of the node to reload
     * @param[in] recursively - do you want to retrieve descendats as well?
     */
    virtual std::uint64_t add(Context& ctx, const std::string &parent, const std::string &uuid, bool recursively);

    /*!
     * @brief Downloads recursively all subcollections
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent uuid of parent node
     * @param[in] collections Array of collections to download
     */
    void fetch_subcomponents(Context& ctx, const std::string &parent, const Array<Collection>& collections);

    /*!
     * @brief removes entity with given uuid
     *
     * @param[in] uuid uuid of entity to remove
     */
    void remove_single(const std::string uuid);

    /*!
     * @brief removes given uuid and all its descendants
     *
     * @param[in] uuid uuid of resource to remove
     */
    void do_remove(const std::string &uuid);

    /*!
     * @brief Guard to make sure that event is handled by correct handler
     *
     * @param[in] event event to be processed
     *
     * @return true is the handler is corrent addressee
     */
    bool is_event_for_me(const EventData &event) const {
        return event.get_type() == Model::get_component();
    }

    /*!
     * @brief Text version of component.
     *
     * @return component as text
     */
    const string component_s() const {
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
    std::uint64_t next_id(const std::string& uuid, const std::string& parent_uuid) {
        return m_id_policy.next_id(uuid, parent_uuid);
    }

    std::set<Component> m_sub_components{}; // each handler learns set of possible sub-coponents
    agent_framework::model::enums::CollectionName m_collection_name{Model::get_collection_name()};
    IdPolicy m_id_policy {};

    friend class HandlerTest;

private:

    Model fetch(Context& ctx, const std::string &parent, const std::string &uuid, UpdateStatus& update_status);

    Model fetch_entry(Context& ctx, const std::string &parent, const std::string &uuid);

    UpdateStatus add_to_model(Context& ctx, Model& entry, const std::string &uuid);
};

template<typename Request, typename Model, typename IdPolicy>
bool GenericHandler<Request, Model, IdPolicy>
    ::handle(JsonAgentSPtr agent, const EventData& event) {
    assert(is_event_for_me(event));

    Context ctx;
    ctx.agent = agent.get();
    ctx.mode = Context::Mode::EVENT ;

    log_info(GET_LOGGER("rest"), ctx.indent << ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                 << "Agent event received: " << event.get_notification().to_string()
                                 << " " << component_s() << " uuid=" << event.get_component() << ", "
                                 << "parent uuid=" << event.get_parent() << ", agent_id=" << agent->get_gami_id());
    try {
        using agent_framework::eventing::Notification;
        switch (event.get_notification()) {
            case Notification::Add: {
                try {
                    if (agent_framework::module::get_manager<Model>().entry_exists(event.get_component())) {
                        log_info(GET_LOGGER("rest"),ctx.indent << ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                 << "Duplicated or late event. Ignoring.");
                        return true;
                    }
                    // add may throw
                    add(ctx, event.get_parent(), event.get_component(), true);
                    SubscriptionManager::get_instance()->notify(ctx.events);
                    return true;
                }
                catch (const psme::core::agent::AgentUnreachable&) {
                    // in case of connection error, added element plus all his descendants will be removed from
                    // local "database"
                    do_remove(event.get_component());
                    log_error(GET_LOGGER("rest"), ctx.indent
                                                  << "Exception while handling Agent Event. Removing non-complete data" )
                    return false;
                }
            }
            case Notification::Remove: {
                try {
                    auto uuid_to_remove = event.get_component();
                    remove(uuid_to_remove);
                    SubscriptionManager::get_instance()->notify(ctx.events);
                }
                catch (::agent_framework::exceptions::InvalidParameters e) {
                    log_info(GET_LOGGER("rest"), "Got notification about removal"
                            "of a component, but component is not present in the model");
                }
                return true;
            }
            case Notification::Update: {
                // update may throw
                if (!agent_framework::module::get_manager<Model>().entry_exists(event.get_component())) {
                    log_error(GET_LOGGER("rest"), "Update event for unknown entry. Ignoring");
                    return true;
                }
                update(ctx, event.get_parent(), event.get_component());
                SubscriptionManager::get_instance()->notify(ctx.events);

                return true;
            }
            default:
                log_error(GET_LOGGER("rest"), "Unknown agent event received: " << event.get_notification().to_string());
                break;
        }
    }
    catch (const core::agent::AgentUnreachable& e) {
        log_error(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                      << "AgentEvent Handling failed due to agent connection error: "
                                      << e.what());
    }
    catch (const jsonrpc::JsonRpcException& e) {
        log_error(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                      << "AgentEvent Handling failed due to JsonRpcException: "
                                      << e.what());
    }
    catch (...) {
        log_error(GET_LOGGER("rest"), ctx.indent << "Exception while handling Agent Event. Data may not be complete" )
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
    ::poll(JsonAgentSPtr agent, const std::string& parent_uuid, const std::string& uuid) {

    Context ctx;
    ctx.agent = agent.get();
    ctx.mode = Context::Mode::POLLING;

    try {
        log_info(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] " <<  "Polling started on "
                 << component_s() << " uuid=" << uuid);
        try {
            add(ctx, parent_uuid, uuid, true /*recursively*/); // add may throw
        } catch (const jsonrpc::JsonRpcException&) {}

        SubscriptionManager::get_instance()->notify(ctx.events);
    }
    catch (const core::agent::AgentUnreachable&)  {
        log_error(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                      <<  "Polling failed due to agent error (unreachable).");
    }

    if (ctx.num_removed > 0) {
        log_info(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                     << "#removed : " << ctx.num_removed);
    }
    if (ctx.num_added > 0) {
        log_info(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                     << "#added : " << ctx.num_added);
    }
    if (ctx.num_updated > 0) {
        log_info(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                     << "#udpated: " << ctx.num_updated);
    }
}

template<typename Request, typename Model, typename IdPolicy>
std::uint64_t GenericHandler<Request, Model, IdPolicy>
    ::do_load(Context& ctx, const std::string &parent, const std::string &uuid, bool recursively) {

    assert(parent != uuid);

    log_info(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                 << "Loading [" << component_s() << " " << uuid << " parent_uuid=" << parent << "]");

    return add(ctx, parent, uuid, recursively);
}

template<typename Request, typename Model, typename IdPolicy>
std::uint64_t GenericHandler<Request, Model, IdPolicy>
    ::load(JsonAgentSPtr agent, const std::string &parent, const std::string &uuid, bool recursively) {

    assert(parent != uuid);

    Context ctx;
    ctx.agent = agent.get();
    ctx.mode = Context::Mode::LOADING;

    auto rest_id = do_load(ctx, parent, uuid, recursively);
    SubscriptionManager::get_instance()->notify(ctx.events);

    return rest_id;
}

template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
    ::load_collection(JsonAgentSPtr agent, const std::string &parent,
                   const CollectionType& collection_type, bool recursively) {
    Context ctx;
    ctx.agent = agent.get();
    ctx.mode = Context::Mode::LOADING;
    auto indent = ctx.indent;
    ctx.indent += "    ";

    log_info(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
             << "Loading collections of type [" << collection_type.to_string() << "] for ["
             << component_s() << " " << parent << " " << "]");

    Request request{parent};
    const Model entry = ctx.agent->execute<Model>(request);

    std::vector<Collection> collections{};
    std::for_each(entry.get_collections().get_array().begin(), entry.get_collections().get_array().end(),
        [&collection_type, &collections] (const Collection& element) {
            if (element.get_type() == collection_type) {
                collections.push_back(element);
            }
        }
    );

    if (collections.empty()) {
        log_error(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
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
            log_info(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode)
                     << "] Collection [" << collection.get_name() << "]");
            auto handler = HandlerManager::get_instance()->get_handler(collection.get_type());
            if (epochs.find(collection.get_type()) == epochs.end()) {
                epochs[collection.get_type()] = handler->get_manager_epoch();
            }
            const auto siblings = handler->fetch_sibling_uuid_list(ctx, parent, collection.get_name());
            for (const auto& sibling: siblings) {
                handler->update(ctx, parent, sibling.get_subcomponent());
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
    ::add(Context& ctx, const std::string &parent, const std::string &uuid, bool recursively) {

    UpdateStatus update_status;
    const Model entry = fetch(ctx, parent, uuid, update_status);
    if (recursively) {
        auto indent = ctx.indent;
        ctx.indent += "    ";
        bool my_skip = false;
        if (UpdateStatus::Added == update_status && !ctx.do_not_emit_from_descendats) {
            ctx.do_not_emit_from_descendats = true;
            my_skip = true;
        }

        fetch_subcomponents(ctx, uuid, entry.get_collections());

        if (my_skip) {
            ctx.do_not_emit_from_descendats = false;
        }
        ctx.indent = indent;
    }
    return entry.get_id();
}

template<typename Request, typename Model, typename IdPolicy>
std::uint64_t GenericHandler<Request, Model, IdPolicy>
    ::update(Context& ctx, const std::string &parent, const std::string &uuid) {

    return do_load(ctx, parent, uuid, false);
}

template<typename Request, typename Model, typename IdPolicy>
typename GenericHandler<Request, Model, IdPolicy>::UpdateStatus GenericHandler<Request, Model, IdPolicy>
    ::add_to_model(Context& ctx, Model& entry, const std::string &uuid) {

    auto status = agent_framework::module::get_manager<Model>().add_or_update_entry(entry);
    switch (status) {
        case UpdateStatus::Updated:
            ctx.add_event(get_component(), eventing::EventType::ResourceUpdated, uuid);
            ctx.num_updated++;
            log_debug(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                          << "Updated [" << component_s() << " " << uuid
                                          << ", parent_uuid=" << entry.get_parent_uuid()
                                          << ", id: " << entry.get_id() << "]");
            break;
        case UpdateStatus::Added:
            if (!ctx.do_not_emit_from_descendats) {
                ctx.add_event(get_component(), eventing::EventType::ResourceAdded, uuid);
            }
            ctx.num_added++;
            log_info(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                         << "Added [" << component_s() << " " << uuid
                                         << ", parent_uuid=" << entry.get_parent_uuid()
                                         << ", id: " << entry.get_id() << "]");
            break;
        case UpdateStatus::NoUpdate:
            break;
        default:
            assert(false);
    }
    return status;
}

template<typename Request, typename Model, typename IdPolicy>
Model GenericHandler<Request, Model, IdPolicy>
    ::fetch(Context& ctx, const std::string &parent, const std::string &uuid, UpdateStatus& update_status) {

    auto entry = fetch_entry(ctx, parent, uuid);
    entry.set_id(next_id(uuid, parent));
    entry.set_agent_id(ctx.agent->get_gami_id());
    assert(!entry.get_agent_id().empty());

    update_status = add_to_model(ctx, entry, uuid);

    return entry;
}

template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
    ::fetch_subcomponents(Context& ctx, const std::string &parent, const Array<Collection>& collections) {

    auto sub_handlers = ::psme::rest::model::handler::HandlerManager::get_instance()->get_handlers(collections);
    ctx.stack.emplace(get_component());

    // epochs will be used to find out which nodes were not read from agent during polling.
    // if somethig was not read, we assume that was deleted and should be removed from internal database
    // (as well as its descendants).
    // this map maps each CollectionType (and manager, they have 1-1 relation) to the epoch before
    // loading these components - a vector is not enough because of the possibility of having multiple
    // colections of the same type (with different names)
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
        /* other exceptions should not stop us from reading.
         * note, that error was already logged
         */
        catch (...) {}
    }

    // After all fetching we remove nodes that could not be found
    for (auto sub_handler : sub_handlers ) {
        sub_handler.second->remove_untouched(ctx, parent, epochs[sub_handler.second->get_component()]);
    }
    ctx.stack.pop();
}

template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
    ::remove_untouched(Context& ctx, const std::string& parent_uuid, std::uint64_t epoch) {
    if (Context::Mode::POLLING == ctx.mode || Context::Mode::LOADING == ctx.mode) {
        auto not_updated = agent_framework::module::get_manager<Model>().get_keys(parent_uuid,
            [&epoch, &ctx](const Model &entry) {
                return (entry.get_agent_id() == ctx.agent->get_gami_id() && !entry.was_touched_after(epoch));
            }
        );

        for (const auto &to_remove_uuid : not_updated) {
            std::string updated_by = Context::Mode::POLLING == ctx.mode ? "polling" : "loading";
            log_debug(GET_LOGGER("rest"), "[" << static_cast<char>(ctx.mode) << "] [" << component_s()
                                          << " = " << to_remove_uuid << "] not found by " << updated_by << ". Removing.");
            remove(to_remove_uuid);
            ctx.num_removed++;
        }
    }
}

template<typename Request, typename Model, typename IdPolicy>
Model GenericHandler<Request, Model, IdPolicy>
    ::fetch_entry(Context& ctx, const std::string &parent, const std::string &uuid) {

    Request request{uuid};
    log_debug(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                                  << "Fetching [" << component_s() << " " << uuid << "]");
    try {
        auto elem = ctx.agent->execute<Model>(request);
        elem.set_parent_uuid(parent);
        elem.set_uuid(uuid);
        return elem;
    } catch (const jsonrpc::JsonRpcException& e) {
        log_error(GET_LOGGER("rest"),
                  ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
                  << "RPC Error while fetching [" << component_s() << " - "
                  << uuid << "]: " << e.what());
        throw;
    }
    catch (const psme::core::agent::AgentUnreachable& e) {
        log_error(GET_LOGGER("rest"), ctx.indent << "[" << static_cast<char>(ctx.mode) << "] "
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

template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
    ::fetch_siblings(Context& ctx, const std::string &parent_uuid,
                 const std::string &collection_name) {
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

    log_debug(GET_LOGGER("rest"), ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Fetching list of all components of type [" << component_s()
                << "] from collection [" << collection_name
                << "] for parent " << parent_uuid);
    try {
        auto res = ctx.agent->execute<Array<SubcomponentEntry>>(collection);
        log_debug(GET_LOGGER("rest"), ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Got " << res.get_array().size());
        return res;
    }
    catch (const psme::core::agent::AgentUnreachable& e) {
        log_error(GET_LOGGER("rest"), ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
                << "Agent connection error while fetching list of all"
                   "components of type [" << component_s()
                << "] from collection [" << collection_name
                << "] for parent " << parent_uuid
                << " : " << e.what());
        throw; // connection error - stop transaction
    }
    catch (const jsonrpc::JsonRpcException& e) {
        log_error(GET_LOGGER("rest"), ctx.indent
                << "[" << static_cast<char>(ctx.mode) << "] "
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
void GenericHandler<Request, Model, IdPolicy>
    ::remove(const std::string &uuid) {
    auto url = psme::rest::endpoint::utils::get_component_url( get_component(), uuid);
    do_remove(uuid);
    auto upstream_event = Event( eventing::EventType::ResourceRemoved, url);
    SubscriptionManager::get_instance()->notify(upstream_event);
}

template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
    ::do_remove(const std::string &uuid) {

    for (auto component = m_sub_components.begin(); component != m_sub_components.end(); ++component) {
        auto handler = ::psme::rest::model::handler::HandlerManager::get_instance()->get_handler(*component);
        handler->remove_all(uuid);
    }
    remove_single(uuid);
}

template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>
    ::remove_all(const std::string &parent) {

    auto siblings = agent_framework::module::get_manager<Model>().get_keys(parent);
    for (const auto& uuid : siblings) {
        do_remove(uuid);
    }
}

template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>::remove_single(const std::string uuid) {

    log_info(GET_LOGGER("rest"), "Removing [" << component_s() << " " << uuid
         << "] from the model (id: " << agent_framework::module::get_manager<Model>().uuid_to_rest_id(uuid) << ")");
    agent_framework::module::get_manager<Model>().remove_entry(uuid);
    m_id_policy.remove(uuid); // we do not want to have same rest id when FRU is reinserted
}

template<typename Request, typename Model, typename IdPolicy>
void GenericHandler<Request, Model, IdPolicy>::remove_agent_data(const std::string& gami_id) {
    auto agent_predicate = [&gami_id](const Model& entry) { return entry.get_agent_id() == gami_id; };
    const auto uuids_to_remove = agent_framework::module::get_manager<Model>().get_keys(agent_predicate);

    for(const auto& uuid : uuids_to_remove) {
        auto parent_uuid = agent_framework::module::get_manager<Model>().get_entry_reference(uuid)->get_parent_uuid();
        m_id_policy.purge(uuid, parent_uuid);
        remove_single(uuid);
    }
}

}
}
}
}

