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

#include "psme/rest/endpoints/utils.hpp"

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"

#include <stack>

namespace psme {
namespace rest {
namespace model {
namespace handler {

template<typename Request, typename Model, typename IdPolicy>
class GenericHandler;

/*!
* @brief Base class for all handlers.
*
* Base class for handlers that handle events (ADD, REMOVE, UPDATE) from agents.
* Along with handling events (handle(...) method) it is used for polling (poll(...) method).
* */
struct HandlerInterface {
public:
    using Component = agent_framework::model::enums::Component;
    using Collection = agent_framework::model::attribute::Collection;
    using CollectionType = agent_framework::model::enums::CollectionType;
    using JsonAgent = psme::core::agent::JsonAgent;
    using JsonAgentSPtr = psme::core::agent::JsonAgentSPtr;
    using EventData = agent_framework::model::attribute::EventData;
    using Notification = agent_framework::model::enums::Notification;
    using Event = psme::rest::eventing::Event;
    using EventVec = psme::rest::eventing::EventVec;
    using EventType = psme::rest::eventing::EventType;
    using ComponentStack = std::stack<Component>;
    template<typename T>
    using Array = agent_framework::model::attribute::Array<T>;

    /*!
     * @brief Data that holds state of operation executed on handler.
     * It is passed down during processing.
     * */
    struct Context {
        enum class Mode : char {
            EVENT = 'E', POLLING = 'P', LOADING = 'L', USER_ACTION = 'U', AGENT_DISAPPEARED = 'A'
        };

        Mode mode{};
        ComponentStack stack{};

        Component get_parent_component() {
            return stack.size() > 0 ? Component(stack.top()) : Component(Component::None);
        }

        std::string indent{};

        /*!
         * @brief json agent to talk to
         * */
        JsonAgent* agent{nullptr};

        EventVec events{};

        u_int32_t num_added{0};
        u_int32_t num_removed{0};
        u_int32_t num_updated{0};
        u_int32_t num_status_changed{0};
        u_int32_t num_alerts{0};

        void add_event(Component component, EventType event_type, const std::string& uuid) {
            std::string url{};
            try {
                url = psme::rest::endpoint::utils::get_component_url(component, uuid);
            }
            catch(const agent_framework::exceptions::NotFound& ex) {
                log_debug("rest", indent << "Exception while building event origin URL " << ex.what());
                return;
            }

            // URL with JSON pointer means a property, not a resource.
            // But events are for resources, so we transform the event to an update of the resource.
            // So for example a ResourceAdded with URL /redfish/v1/Chassis/1/Thermal#/Fans/0
            // will be sent as ResourceUpdated event for /redfish/v1/Chassis/1/Thermal.
            size_t position = url.find_first_of('#');
            if (position != std::string::npos) {
                events.emplace_back(Event(EventType::ResourceUpdated, url.substr(0, position)));
            }
            else {
                events.emplace_back(Event(event_type, url));
            }
        }
    };

    /*!
     * @brief Constructor
     */
    HandlerInterface() = default;

    virtual ~HandlerInterface();

    /*!
     * @brief removes a component and all its descendants from the model
     *
     * @param[in] uuid component's uuid
     * */
    virtual void remove(const std::string& uuid) = 0;

    /*!
     * @brief Remove from model all data pertaining to a given agent
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] gami_id GAMI ID of agent which data should be removed
     * */
    virtual void remove_agent_data(Context& ctx, const std::string& gami_id) = 0;

    /*!
     * @brief dispatches request to the right method: add(), remove() or update()
     *
     * 1. If add():
     * - it fetches out new resource from agent using parent_uuid and uuid given in request
     * - it recursively fetches (DFS) all subcomponents.
     * (Handler does not have knowledge about hierarchy of resources. I knows nothing about
     * the fact that System has Processors. It simply retrieves every known sub-collection
     * reported by resource)
     * 2. If remove():
     * - it removes the node from the tree as well as all its descendants
     * 3. If update():
     * - fetches out resource form agent and updates local model
     *
     * @param[in] agent JSON agent that is the originator of the event
     * @param[in] event Event data (tells us WHAT and WHY)
     * @param[in,out] northbound_events Redfish EventRecords collected while handling the event
     * @return Status of handle - true if success
     * */
    virtual bool handle(JsonAgentSPtr agent, const EventData& event, EventVec& northbound_events) = 0;

    /*!
     * @brief Similar to handle(...) but used to poll data asynchronously.
     *
     * @param[in] agent JSON agent to talk to during polling
     * @param[in] parent_uuid UUID of parent node
     * @param[in] parent_component parent component type
     * @param[in] uuid Start polling from this node
     * */
    virtual void poll(JsonAgentSPtr agent,
                      const std::string& parent_uuid, const Component parent_component,
                      const std::string& uuid) = 0;

    /*!
     * @brief can be used by client to load resource on-demand.
     *
     * @param[in] agent JSON agent to talk to while polling
     * @param[in] parent UUID of parent node
     * @param[in] parent_component parent component type
     * @param[in] uuid UUID of the node to reload
     * @param[in] recursively Do you want to retrieve descendants as well?
     *
     * Will be used by POST, PUT, PATCH endpoints to reload data from agent
     */
    virtual std::uint64_t load(JsonAgentSPtr agent,
                               const std::string& parent, const Component parent_component,
                               const std::string& uuid, bool recursively = false) = 0;

    /*!
     * @brief gets the current epoch from manager bound to this handler
     *
     * @return manager's current epoch
     */
    virtual std::uint64_t get_manager_epoch() = 0;

    /*!
     * @brief can be used by client to load resource's collection on-demand.
     *
     * Will be used by post,put,patch endpoints to reload data from agent
     *
     * @param[in] agent JSON agent to talk to while loading
     * @param[in] parent UUID of parent node
     * @param[in] parent_component parent component type
     * @param[in] collection_type Collection type we want to load
     * @param[in] recursively Do you want to retrieve descendants as well?
     * */
    virtual void load_collection(JsonAgentSPtr agent,
                                 const std::string& parent, const Component parent_component,
                                 const CollectionType& collection_type, bool recursively) = 0;

    /*!
     * @brief ResourceVisitor interface
     */
    class ResourceVisitor {
    public:
        virtual ~ResourceVisitor();
        /*!
         * @brief Callback to be called for each resource in the subtree pointed out by accept(...) method
         *
         * @param[in] resource object that represents single node of resource tree
         * @param[in] component kind of component that is represented by resource
         *
         * @return return false if process of traversing tree should be stopped immediately
         * */
        virtual bool visit(const agent_framework::model::Resource& resource, const agent_framework::model::enums::Component component) = 0;

    private:
        /*!
         * @brief set to visited resources - to break cycles
         */
        std::set<std::string> visited{};

        template<typename Request, typename Model, typename IdPolicy>
        friend class psme::rest::model::handler::GenericHandler;
    };

    /*!
     * @brief Visitor pattern implemented to make possible to visit the node and its descendants
     *
     * @param[in] visitor object that implements ResourceVisitor interface
     * @param[in] uuid uuid of the object to start with
     */
    virtual void accept(ResourceVisitor& visitor, const std::string& uuid) = 0;
protected:

    /*!
     * @brief Returns component for which this handler is targeted
     * @return component
     */
    virtual Component get_component() = 0;

    /*!
     * @brief Fetches from agent all components with given parent uuid
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent_uuid uuid of parent component
     * @param[in] collection_name name of the collection to be fetched
     */
    virtual void fetch_siblings(Context& ctx, const std::string& parent_uuid,
                                const std::string& collection_name) = 0;

    /*!
     * @brief fetches list of all sibling UUIDs for given parent in collection with
     * given name
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent_uuid uuid of parent component
     * @param[in] collection_name name of the collection
     *
     * @return list of UUIDs being siblings of parent in specific collection
     */
    virtual agent_framework::model::attribute::Array<agent_framework::model::attribute::SubcomponentEntry>
            fetch_sibling_uuid_list(Context& ctx, const std::string& parent_uuid,
                                    const std::string& collection_name) = 0;

    /*!
     * @brief removes all components that have not been touched after given epoch
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent_uuid uuid of parent component
     * @param[in] epoch epoch to remove before
     */
    virtual void remove_untouched(Context& ctx, const std::string& parent_uuid, std::uint64_t epoch) = 0;

    /*!
     * @brief updates a single component
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent uuid of parent component
     * @param[in] uuid uuid of updated component
     *
     * @return component's REST id
     */
    virtual std::uint64_t update(Context& ctx, const std::string& parent, const std::string& uuid) = 0;

    /*!
     * @brief removes all children of given parent (including descendants)
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent uuid of parent whose children should be removed
     */
    virtual void remove_all(Context& ctx, const std::string& parent) = 0;

    /*!
     * @brief Visitor pattern implemented to make possible to visit the node and its descendants
     *
     * @param[in] visitor object that implements ResourceVisitor interface
     * @param[in] uuid uuid of the object to start with
     * @return returns true if visitor should continue
     */
    virtual bool do_accept(ResourceVisitor& visitor, const std::string& uuid) = 0;

    /*!
     * @brief Visitor pattern implemented to make possible to visit the node and its descendants
     *
     * Because hierarchy is represented by objects of different types, it is not possible to
     * enclose stepping down the tree in single method. This helper method serves that purpose.
     *
     * @param[in] visitor object that implements ResourceVisitor interface
     * @param[in] parent_uuid uuid of the parent object to start with
     * @param[in] parent_component parent component
     * @return returns true if visitor should continue
     */
    virtual bool do_accept_recursively(ResourceVisitor& visitor,
                                       const std::string& parent_uuid,
                                       const Component parent_component) = 0;

    /*!
     * @brief can be used by client to load resource on-demand.
     *
     * Will be used by post, put, patch endpoints to reload data from agent
     *
     * @param[in] ctx State of the handler passed down when handling request
     * @param[in] parent UUID of parent node
     * @param[in] uuid UUID of the node to reload
     * @param[in] recursively Do you want to retrieve descendants as well?
     *
     * @return rest id of top-most loaded element
     * */
    virtual std::uint64_t do_load(Context& ctx, const std::string& parent, const std::string& uuid, bool recursively) = 0;

    template<typename Request, typename Model, typename IdPolicy>
    friend class psme::rest::model::handler::GenericHandler;
};


}
}
}
}
