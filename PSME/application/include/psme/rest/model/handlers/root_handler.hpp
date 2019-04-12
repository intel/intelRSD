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
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"

/*!
 * @brief Fake PortMember resource to satisfy GenericHandler and make it compilable
 */
namespace agent_framework {
namespace model {

/*!
 * @brief Stub required to instantiate RootHandler
 */
class FakeRoot : public Resource {
public:
    FakeRoot() : Resource("fakeuuid") {}


    FakeRoot(const FakeRoot&) = default;


    FakeRoot& operator=(const FakeRoot&) = default;


    virtual ~FakeRoot();


    static enums::Component get_component() {
        return enums::Component::Root;
    }


    static FakeRoot from_json(const json::Json&) {
        assert(false);
        return FakeRoot();
    }


    static enums::CollectionName get_collection_name() {
        return enums::CollectionName::None;
    }

    json::Json to_json() const {
        return json::Json{};
    }
};

namespace requests {

/*!
 * @brief Stub required to instantiate RootHandler
 */
class FakeRootInfo {
public:
    FakeRootInfo(const FakeRootInfo&) : m_uuid{} {}


    virtual ~FakeRootInfo();


    explicit FakeRootInfo(const std::string& uuid) : m_uuid(uuid) {}


    /*!
     * @brief returns command
     */
    static std::string get_command() {
        assert(false);
        return "Unexpected method called";
    }


    /*!
     * @brief conversion to json
     */
    json::Json to_json() const {
        assert(false);
        return json::Json{};
    }


    /*!
     * @brief conversion from  son
     */
    static FakeRootInfo from_json(const json::Json&) {
        assert(false);
        return FakeRootInfo("none");
    }


    /*!
     * @brief returns component uuid
     */
    const std::string& get_uuid() const {
        assert(false);
        return m_uuid;
    }


    std::string m_uuid;
};
}
}
}

namespace psme {
namespace rest {
namespace model {
namespace handler {

/*!
 * @brief Handler for artificial ROOT component introduced to simplify generic code
 */
class RootHandler : public GenericHandler<agent_framework::model::requests::FakeRootInfo,
    agent_framework::model::FakeRoot,
    IdPolicy<agent_framework::model::enums::Component::Root, NumberingZone::SHARED>> {
public:
    ~RootHandler();
protected:
    /*!
     * @brief Implements actions that will be taken after ADD event from agent
     * @param ctx State of the handler passed down when handling request
     * @param parent UUID of parent node
     * @param uuid UUID of the node to reload
     * @param recursively Do you want to retrieve descendants as well?
     */
    std::uint64_t add(Context& ctx, const std::string& parent, const std::string& uuid, bool recursively) override {
        (void) parent;
        (void) recursively;
        using Collection = agent_framework::model::attribute::Collection;
        Array<Collection> collections;
        collections.add_entry({ Collection(
                agent_framework::model::enums::CollectionName::Managers,
                agent_framework::model::enums::CollectionType::Managers)
        });
        collections.add_entry({ Collection(
                agent_framework::model::enums::CollectionName::Tasks,
                agent_framework::model::enums::CollectionType::Tasks)
        });
        collections.add_entry({ Collection(
            agent_framework::model::enums::CollectionName::MetricDefinitions,
            agent_framework::model::enums::CollectionType::MetricDefinitions)
        });
        collections.add_entry({ Collection(
            agent_framework::model::enums::CollectionName::Metrics,
            agent_framework::model::enums::CollectionType::Metrics)
        });
        auto indent = ctx.indent;
        ctx.indent += "    ";
        fetch_subcomponents(ctx, uuid, collections);
        ctx.indent = indent;
        return 0;
    }

    /*!
     * @brief removes a component and all its descendants from the model
     *
     * */
    void remove(const std::string&) override {
        throw std::runtime_error("Logic error - should not be executed");
    }

    /*!
     * @brief Handles events (Add, Update, Remove) from agent. It starts chain of actions
     * of retrieval data from agent, removal data from local database.
     *
     * @return status of handle - true if success
     *
     * Because single handler represents one type of resource, handlers will call another
     * handlers to build the whole tree of components
     * */
    bool handle(JsonAgentSPtr, const EventData&, EventVec&) override {
        throw std::runtime_error("Logic error - should not be executed");
    }

    /*!
     * @brief can be used by client to load resource on-demand.
     *
     * Will be used by post,put,patch endponts to reload data from agent
     */
    std::uint64_t load(JsonAgentSPtr, const std::string&, const Component, const std::string&, bool) override  {
        throw std::runtime_error("Logic error - should not be executed");
    }

    /*!
     * @brief Returns component for which this handler is targeted
     */
    Component get_component() override  {
        return Component::None;
    }

    /*!
     * @brief Fetches from agent all components with given parent uuid
     */
    void fetch_siblings(Context&, const std::string&, const std::string&) override  {
        throw std::runtime_error("Logic error - should not be executed");
    }

    /*!
     * @brief removes all childrens of given parent (including descendants)
     */
    void remove_all(Context&, const std::string&) override  {
        throw std::runtime_error("Logic error - should not be executed");
    }
};


}
}
}
}



