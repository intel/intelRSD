/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"



namespace agent_framework {
namespace command {

using Keys = std::vector<std::string>;
using Collections = model::attribute::Array<model::attribute::Collection>;
using Response = model::attribute::Array<model::attribute::SubcomponentEntry>;

/*! @brief For templates we need pure 'enum' types, not our enum 'wrappers' */
using CollectionType = model::enums::CollectionType::CollectionType_enum;

/*! @brief This describes if a link is just a link or an ownership relation */
enum class LinkType {
    Weak, Strong
};

/*!
 * @brief Helper class used for templates.
 *
 * It Link<[Weak|Strong], CollectionType, ChildType> means that we should expect
 * a weak/strong collection of type CollectionType that stores resources of type ChildType.
 * Two first parameters are enum values, the last one is a type.
 *
 * example:
 *     Link<LinkType::Strong, CollectionType::Systems, System>
 * */
template<LinkType LINK_TYPE, CollectionType COLLECTION_TYPE, typename CHILD>
struct Link {
};


/*! @brief Helper function, takes collection by string and finds it on the list of entity collections */
inline model::attribute::Collection find_collection(const Collections& collections, const std::string& name) {
    for (const auto& collection : collections) {
        if (name == collection.get_name()) {
            return collection;
        }
    }

    model::attribute::Collection c{};
    c.set_type(model::enums::CollectionType::None);
    return c;
}


/*! @brief Adds list of children to the response */
inline void add_children_to_response(const Keys& keys, Response& response) {
    for (const auto& key : keys) {
        model::attribute::SubcomponentEntry s{};
        s.set_subcomponent(key);
        response.add_entry(s);
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
template<typename... LINKS>
class ResourceProcessor {
public:
    /*!
     * @brief Finds all children that are in a collection under a resource and adds them to the response
     * */
    static bool process_collection(const model::attribute::Collection&, const std::string&, Response&) {
        // generic template - should never be called
        assert(false);
        return false;
    }
};

/*!
 * @brief Specialization for strong collections.
 *
 * We access GenericManager<CHILD> via get_manager<CHILD> method.
 * */
template<typename PARENT, CollectionType COLLECTION_TYPE, typename CHILD, typename... LINKS>
class ResourceProcessor<PARENT, Link<LinkType::Strong, COLLECTION_TYPE, CHILD>, LINKS...> {
public:

    /*!
     * @brief Finds all children that are in a collection under a resource and adds them to the response
     *
     * @param[in] collection Queried collection
     * @param[in] uuid Queried parent resource uuid
     * @param[out] response Response class to be filled with found children
     *
     * @return True if children were found, False if no matching collection was found
     * */
    static bool process_collection(const model::attribute::Collection& collection,
                                   const std::string& uuid,
                                   Response& response) {
        if (COLLECTION_TYPE == collection.get_type()) {
            add_children_to_response(module::get_manager<CHILD>().get_keys(uuid), response);
            return true;
        }
        // carry on to the next Link<> on the template list
        return ResourceProcessor<PARENT, LINKS...>::process_collection(collection, uuid, response);
    }
};

/*!
 * @brief Specialization for Weak collections.
 *
 * We access specific ManyToManyManager via get_m2m_manager<PARENT, CHILD> method.
 * */
template<typename PARENT, CollectionType COLLECTION_TYPE, typename CHILD, typename... LINKS>
class ResourceProcessor<PARENT, Link<LinkType::Weak, COLLECTION_TYPE, CHILD>, LINKS...> {
public:
    static bool process_collection(const model::attribute::Collection& collection,
                                   const std::string& uuid,
                                   Response& response) {
        if (COLLECTION_TYPE == collection.get_type()) {
            add_children_to_response(module::get_m2m_manager<PARENT, CHILD>().get_children(uuid), response);
            return true;
        }
        // carry on to the next Link<> on the template list
        return ResourceProcessor<PARENT, LINKS...>::process_collection(collection, uuid, response);
    }
};

/*! If we run out of Links in the template parameter list LINKS - just return false, as collection was not found */
template<typename PARENT>
class ResourceProcessor<PARENT> {
public:
    static bool process_collection(const model::attribute::Collection&, const std::string&, Response&) {
        return false;
    }
};


/*!
 * @brief Process collections for a resource of a given typename.
 *
 * @tparam RESOURCE is a parent type
 * @tparam COLLECTIONS is a list of Link classes defining what collections * are to be checked.
 *
 * @param[in] uuid Uuid of the parent resource
 * @param[in] collection_name Name of the collection to look for
 * @param[out] response Response to be filled with found children
 *
 * @return True if collection was found and processed, false otherwise
 * */
template<typename RESOURCE, typename... COLLECTIONS>
bool process_resource_collections(const std::string& uuid, const std::string& collection_name, Response& response) {
    if (module::get_manager<RESOURCE>().entry_exists(uuid)) {
        auto resource = module::get_manager<RESOURCE>().get_entry(uuid);
        auto collection = find_collection(resource.get_collections(), collection_name);

        if (!ResourceProcessor<RESOURCE, COLLECTIONS...>::process_collection(collection, uuid, response)) {
            THROW(agent_framework::exceptions::InvalidCollection, "gami",
                  "Invalid collection name: \'" + collection_name + "\'");
        }
        return true;
    }
    return false;
}

}
}
