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
 * @file command/stubs/get_collection.cpp
 * @brief GetCollection stub implementation
 * */

#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/pnc_commands.hpp"
#include "agent-framework/logger_ext.hpp"



using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

namespace {

using KeysVec = std::vector<std::string>;
using Collections = attribute::Array<attribute::Collection>;
using Response = GetCollection::Response;

/*! For templates we need pure 'enum' types, not our enum 'wrappers' */
using CollectionType = enums::CollectionType::CollectionType_enum;

/*! This describes if a link is just a link or an ownership relation */
enum class LinkType { Weak, Strong };

/*!
 * Helper class used for templates it Link<[Weak|Strong], CollectionType, ChildType> means that we shoudl expect
 * a weak/strong collection of type CollectionType that stores resources of type ChildType C
 * example:
 *     Link<LinkType::Strong, CollectionType::Systems, System>
 * Two first parameters are enum values, the last one is a type.
 * */
template <LinkType LINK_TYPE, CollectionType COLLECTION_TYPE, typename CHILD> struct Link {};

/*! Helper function, takes collection by string and finds it on the list of entity collections */
Collection find_collection(const Collections& collections, const std::string& name) {
    for (const auto& collection : collections) {
        if (name == collection.get_name()) {
            return collection;
        }
    }
    Collection c;
    c.set_type(enums::CollectionType::None);
    return c;
}

/*! Adds list of children to the response */
void add_children_to_response(const KeysVec& keys, Response& rsp) {
    for (const auto& key : keys) {
        SubcomponentEntry s;
        s.set_subcomponent(key);
        rsp.add_entry(s);
    }
}

/*!
 * For generic handling of collections we need a method that is a template of two parameters:
 * -> Type of the parent resource,
 * -> List of Links defining collections and children found under these collections,
 * Since there are two possible Collection types - strong and weak, then we need to partially specialize the template,
 * therefore we have to create a template object, not a template function. However single static function is enough
 * for it. Example usage - if we have a class Parent with a WeakChild linked by a weak collection and a StrongChild
 * linked by a strong collection, and the collections are named WeakChildren and StrongChildren, the call
 * should look like:
 *     ResourceProcessor<Parent,
 *         Link<Weak, WeakChildren, WeakChild>,
 *         Link<Strong, StrongChildren, StrongChild>
 *     >::process_collection(arguments...)
 * */
template <typename... LINKS>
class ResourceProcessor {
public:
    /*!
     * @brief Finds all children that are in a collection under a resource and adds them to the response
     * */
    static bool process_collection(const Collection&, const std::string&, Response&) {
        // generic template - should never be called
        assert(false);
        return false;
    }
};

/*! Specialization for strong collections */
template <typename PARENT, CollectionType COLLECTION_TYPE, typename CHILD, typename... LINKS>
class ResourceProcessor<PARENT, Link<LinkType::Strong, COLLECTION_TYPE, CHILD>, LINKS...> {
public:
    /*! We access GenericManager<CHILD> via get_manager<CHILD> method */

    /*!
     * @brief Finds all children that are in a collection under a resource and adds them to the response
     * @param[in] collection Queried collection
     * @param[in] uuid Queried parent resource uuid
     * @param[out] rsp Response class to be filled with found children
     * @return True if children were found, False if no matching collection was found
     * */
    static bool process_collection(const Collection& collection, const std::string& uuid, Response& rsp) {
        if (COLLECTION_TYPE == collection.get_type()) {
            add_children_to_response(get_manager<CHILD>().get_keys(uuid), rsp);
            return true;
        }
        // carry on to the next Link<> on the template list
        return ResourceProcessor<PARENT, LINKS...>::process_collection(collection, uuid, rsp);
    }
};

/*! Specialization for Weak collections */
template <typename PARENT, CollectionType COLLECTION_TYPE, typename CHILD, typename... LINKS>
class ResourceProcessor<PARENT, Link<LinkType::Weak, COLLECTION_TYPE, CHILD>, LINKS...> {
public:
    /*! We access specific ManyToManynManager via get_m2m_manager<PARENT, CHILD> method */
    static bool process_collection(const Collection& collection, const std::string& uuid, Response& rsp) {
        if (COLLECTION_TYPE == collection.get_type()) {
            add_children_to_response(get_m2m_manager<PARENT, CHILD>().get_children(uuid), rsp);
            return true;
        }
        // carry on to the next Link<> on the template list
        return ResourceProcessor<PARENT, LINKS...>::process_collection(collection, uuid, rsp);
    }
};

/*! If we run out of Links in the template parameter list LINKS - just return false, as collection was not found */
template <typename PARENT>
class ResourceProcessor<PARENT> {
public:
    static bool process_collection(const Collection&, const std::string&, Response&) {
        return false;
    }
};

/*!
 * @brief Process collections for a resource of a given typename
 * Template parameters: RESOURCE is a parent type, COLLECTIONS is a list of Link classes defining what collections
 * are to be checked.
 * @param[in] uuid Uuid of the parent resource
 * @param[in] collection_name Name of the collection to look for
 * @param[out] response Response to be filled with found children
 * @return True if collection was found and processed, false otherwise
 * */
template <typename RESOURCE, typename... COLLECTIONS>
bool process_resource_collections(const std::string& uuid, const std::string& collection_name,
        GetCollection::Response& response) {
    if (get_manager<RESOURCE>().entry_exists(uuid)) {
        auto resource = get_manager<RESOURCE>().get_entry(uuid);
        auto collection = find_collection(resource.get_collections(), collection_name);

        if (!ResourceProcessor<RESOURCE, COLLECTIONS...>::process_collection(collection, uuid, response)) {
            THROW(agent_framework::exceptions::InvalidCollection, "pnc-gami",
                  "Invalid collection name: \'" + collection_name + "\'");
        }
        return true;
    }
    return false;
}

void get_collection(const GetCollection::Request& request, GetCollection::Response& response) {
    const auto& uuid = request.get_uuid();
    const auto& name = request.get_name();

    if (process_resource_collections<Manager,
            Link<LinkType::Strong, enums::CollectionType::Chassis, Chassis>,
            Link<LinkType::Strong, enums::CollectionType::Systems, System>,
            Link<LinkType::Strong, enums::CollectionType::PCIeDevices, PcieDevice>,
            Link<LinkType::Strong, enums::CollectionType::Fabrics, Fabric>
            >(uuid, name, response)) {}
    else if (process_resource_collections<PcieDevice,
                Link<LinkType::Strong, enums::CollectionType::PCIeFunctions, PcieFunction>
                >(uuid, name, response)) {}
    else if (process_resource_collections<Chassis,
                Link<LinkType::Strong, enums::CollectionType::Drives, Drive>
                >(uuid, name, response)) {}
    else if (process_resource_collections<Drive,
                Link<LinkType::Weak, enums::CollectionType::PCIeFunctions, PcieFunction>
                >(uuid, name, response)) {}
    else if (process_resource_collections<System,
                Link<LinkType::Strong, enums::CollectionType::StorageSubsystems, StorageSubsystem>
                >(uuid, name, response)) {}
    else if (process_resource_collections<StorageSubsystem,
                Link<LinkType::Weak, enums::CollectionType::Drives, Drive>
                >(uuid, name, response)) {}
    else if (process_resource_collections<Fabric,
                Link<LinkType::Strong, enums::CollectionType::Endpoints, Endpoint>,
                Link<LinkType::Strong, enums::CollectionType::Switches, Switch>,
                Link<LinkType::Strong, enums::CollectionType::Zones, Zone>
                >(uuid, name, response)) {}
    else if (process_resource_collections<Switch,
                Link<LinkType::Strong, enums::CollectionType::Ports, Port>
                >(uuid, name, response)) {}
    else if (process_resource_collections<Zone,
                Link<LinkType::Weak, enums::CollectionType::Endpoints, Endpoint>
                >(uuid, name, response)) {}
    else if (process_resource_collections<Endpoint,
                Link<LinkType::Weak, enums::CollectionType::Ports, Port>
                >(uuid, name, response)) {}
    else {
        THROW(::agent_framework::exceptions::InvalidUuid, "pnc-gami",
            "Component not found, invalid uuid: " + uuid);
    }

    log_debug(GET_LOGGER("pnc-gami"), "Getting collection of " << request.get_name() << " successful.");
}
}

REGISTER_COMMAND(GetCollection, ::get_collection);
