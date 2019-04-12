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
 *
 * @file many_to_many_manager.hpp
 *
 * @brief ManyToManyManager responsible for handling pairs of UUIDs.
 *
 * Used whenever it is needed to store Collections of links between
 * two model objects with UUIDs.
 * */

#pragma once



#include "agent-framework/module/managers/generic_manager.hpp"

#include <mutex>
#include <string>
#include <set>
#include <vector>
#include <tuple>
#include <algorithm>
#include <functional>


/*! Psme namespace */
namespace agent_framework {
namespace module {
namespace managers {

/*! Many-to-many relationships manager */
class ManyToManyManager {
public:
    using Predicate = std::function<bool(const Uuid&, const Uuid&, const std::string&)>;


    ManyToManyManager() = default;


    virtual ~ManyToManyManager();


    /*!
     * @brief Adds a pair of the UUIDs, does nothing if pair is already present.
     * Also adds the information about agent the entry is coming from.
     *
     * Note: give this function two parameters if and only if it is being called from an agent.
     * If the function is being called from the application, it must have the gami_id parameter!
     *
     * @param[in] parent UUID of the parent
     * @param[in] child UUID of the child
     * @param[in] gami_id UUID of the agent
     */
    void add_entry(const Uuid& parent, const Uuid& child, const std::string& gami_id = std::string{}) {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_manager_data.insert(IdPair(parent, child, gami_id));
    }


    /*!
     * @brief Removes specific pair from the manager
     *
     * @param[in] parent UUID of the parent
     * @param[in] child UUID of the child
     */
    void remove_entry(const Uuid& parent, const Uuid& child) {
        std::lock_guard<std::mutex> lock{m_mutex};
        auto it = std::find_if(m_manager_data.begin(), m_manager_data.end(),
                               [&parent, &child](const IdPair& entry) {
                                   return ((parent == std::get<0>(entry)) && (child == std::get<1>(entry)));
                               });
        if (it != m_manager_data.end()) {
            m_manager_data.erase(it);
        }
    }


    /*!
     * @brief Removes all the occurrences of a given parent from the manager.
     *
     * @param[in] removed_parent UUID of the parent
     */
    void remove_parent(const Uuid& removed_parent) {
        remove_if([&removed_parent](const Uuid& parent, const Uuid&, const std::string&) {
            return removed_parent == parent;
        });
    }


    /*!
     * @brief Removes all the occurrences of a given child from the manager
     *
     * @param[in] removed_child UUID of the child
     */
    void remove_child(const Uuid& removed_child) {
        remove_if([&removed_child](const Uuid&, const Uuid& child, const std::string&) {
            return removed_child == child;
        });
    }


    void remove_if(Predicate predicate) {
        std::lock_guard<std::mutex> lock{m_mutex};
        do_remove_if(predicate);
    }


    /*!
     * @brief Checks if entry is present in the manager
     *
     * @param[in] parent UUID of the parent
     * @param[in] child UUID of the child
     *
     * @return True if UUID pair entry is found in the manager
     */
    bool entry_exists(const Uuid& parent, const Uuid& child) const {
        std::lock_guard<std::mutex> lock{m_mutex};
        return std::any_of(m_manager_data.begin(), m_manager_data.end(),
                           [&parent, &child](const IdPair& entry) {
                               return (parent == std::get<0>(entry)) && (child == std::get<1>(entry));
                           });
    }


    /*!
     * @brief Checks if given parent exists
     *
     * @param[in] parent UUID of the parent
     *
     * @return True if UUID parent is present in the manager
     */
    bool parent_exists(const Uuid& parent) const {
        std::lock_guard<std::mutex> lock{m_mutex};
        for (const auto& entry : m_manager_data) {
            if (parent == std::get<0>(entry)) {
                return true;
            }
        }
        return false;
    }


    /*!
     * @brief Checks if given child exists
     *
     * @param[in] child UUID of the child
     *
     * @return True if UUID child is present in the manager
     */
    bool child_exists(const Uuid& child) const {
        std::lock_guard<std::mutex> lock{m_mutex};
        for (const auto& entry : m_manager_data) {
            if (child == std::get<1>(entry)) {
                return true;
            }
        }
        return false;
    }


    /*!
     * @brief Removes all entries from the manager
     */
    void clear_entries() {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_manager_data.clear();
    }


    /*!
     * @brief Returns a vector of children UUIDs for a specific parent
     *
     * @param[in] parent UUID of the parent
     *
     * @return Vector of all children UUIDs for the provided parent uuid
     */
    std::vector<Uuid> get_children(const Uuid& parent) const {
        std::lock_guard<std::mutex> lock{m_mutex};
        std::vector<Uuid> children{};
        for (const auto& entry : m_manager_data) {
            if (parent == std::get<0>(entry)) {
                children.push_back(std::get<1>(entry));
            }
        }
        return children;
    }


    /*!
     * @brief Returns a vector of parents UUIDs for a specific child
     *
     * @param[in] child UUID of the child
     *
     * @return Vector of all parents UUIDs for the provided child uuid
     */
    std::vector<Uuid> get_parents(const Uuid& child) const {
        std::lock_guard<std::mutex> lock{m_mutex};
        std::vector<Uuid> parents{};
        for (const auto& entry : m_manager_data) {
            if (child == std::get<1>(entry)) {
                parents.push_back(std::get<0>(entry));
            }
        }
        return parents;
    }


    /*!
     * @brief Returns all unique parents UUIDs present in the manager
     *
     * @return Vector of unique parents UUIDs
     */
    std::vector<Uuid> get_all_unique_parents() const {
        std::lock_guard<std::mutex> lock{m_mutex};
        std::set<Uuid> parents{};
        for (const auto& entry : m_manager_data) {
            parents.insert(std::get<0>(entry));
        }
        return {parents.begin(), parents.end()};
    }


    /*!
     * @brief Removes resources belonging to a given agent
     *
     * @param[in] gami_id Agent GAMI ID
     * */
    void clean_resources_for_agent(const std::string& gami_id) {
        std::lock_guard<std::mutex> lock{m_mutex};
        do_remove_if([&gami_id](const Uuid&, const Uuid&, const std::string& removed_gami_id) {
            return removed_gami_id == gami_id;
        });
    }


    /*!
     * @brief Removes resources not belonging to any agent
     * */
    void clean_resources_with_no_agent() {
        std::lock_guard<std::mutex> lock{m_mutex};
        do_remove_if([](const Uuid&, const Uuid&, const std::string& removed_gami_id) {
            return removed_gami_id == std::string{};
        });
    }


    /*!
     * @brief Update parent in the manager entries table by changing all occurrences of parent_uuid to parent_new_uuid.
     *
     * @param[in] parent_uuid Parent UUID to be replaced
     * @param[in] parent_new_uuid New parent UUID
     * */
    void update_parent(const Uuid& parent_uuid, const Uuid& parent_new_uuid) {
        std::lock_guard<std::mutex> lock{m_mutex};
        update_entry<0>(parent_uuid, parent_new_uuid);
    }


    /*!
     * @brief Update child in the manager entries table by changing all occurrences of child_uuid to child_new_uuid.
     *
     * @param[in] child_uuid Child UUID to be replaced
     * @param[in] child_new_uuid New child UUID
     * */
    void update_child(const Uuid& child_uuid, const Uuid& child_new_uuid) {
        std::lock_guard<std::mutex> lock{m_mutex};
        update_entry<1>(child_uuid, child_new_uuid);
    }


    /*!
     * @brief Update agent in the manager entries table by changing all occurrences of agent_uuid to agent_new_uuid.
     *
     * @param[in] agent_uuid Agent UUID to be replaced
     * @param[in] agent_new_uuid New agent UUID
     * */
    void update_agent(const std::string& agent_uuid, const std::string& agent_new_uuid) {
        std::lock_guard<std::mutex> lock{m_mutex};
        update_entry<2>(agent_uuid, agent_new_uuid);
    }


protected:

    template<unsigned P>
    void update_entry(const std::string& old_id, const std::string& new_id) {
        for (IdPairCollection::iterator it = m_manager_data.begin(); it != m_manager_data.end();) {
            if (old_id == std::get<P>(*it)) {
                IdPair updated_entry = *it;
                it = m_manager_data.erase(it);
                std::get<P>(updated_entry) = new_id;
                m_manager_data.insert(updated_entry);
            }
            else {
                it++;
            }
        }
    }


    void do_remove_if(Predicate predicate) {
        for (auto it = m_manager_data.begin(); it != m_manager_data.end();) {
            if (predicate(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it))) {
                it = m_manager_data.erase(it);
            }
            else {
                ++it;
            }
        }
    }


    using IdPair = std::tuple<Uuid, Uuid, std::string>;
    using IdPairCollection = std::set<IdPair>;

    mutable std::mutex m_mutex{};
    IdPairCollection m_manager_data{};
};

}
}
}
