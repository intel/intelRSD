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
 * @file module_manager.hpp
 * @brief Module Manager
 * */

#pragma once

#include "table_interface.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "agent-framework/generic/obj_reference.hpp"
#include "agent-framework/module/managers/generic_manager_registry.hpp"
#include "agent-framework/module/model/task.hpp"
#include "agent-framework/module/utils/utils.hpp"

#include <vector>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <string>
#include <functional>
#include <atomic>

/*! Psme namespace */
namespace agent_framework {
namespace module {


/*! Generic implementation manager */
template <typename T>
class GenericManager : public TableInterface {
public:
    using value_type = T;
    using ManagerDataVec = std::vector<T>;
    using KeysVec = std::vector<std::string>;
    using IdsVec = std::vector<std::uint64_t>;
    using Reference = generic::ObjReference<T, std::recursive_mutex>;
    using ReferenceVec = std::vector<Reference>;
    using Filter = std::function<bool(const T&)>;

    GenericManager() {
        GenericManagerRegistry::get_instance()->register_table(this);
    }

    virtual ~GenericManager();

    model::enums::Component get_component() override {
        return value_type::get_component();
    }

    template <typename U>
    void add_entry(U&& entry_r) {
        T entry = std::forward<U>(entry_r);
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        const auto it = find_entry(entry.get_uuid());
        if (m_manager_data.end() != it) {
            THROW(exceptions::InvalidUuid, "model",
                  "Object with this UUID already exists. UUID = '" + entry.get_uuid() + "'.");
        }
        entry.touch(++m_current_epoch);
        m_manager_data.push_back(std::move(entry));
    }

    template <typename U>
    UpdateStatus add_or_update_entry(U&& entry_r) {
        T entry = std::forward<U>(entry_r);
        UpdateStatus res = UpdateStatus::NoUpdate;
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        entry.touch(++m_current_epoch);

        auto it = find_entry(entry.get_uuid());
        if (m_manager_data.end() != it) {
            if (it->get_parent_uuid() != entry.get_parent_uuid()) {
                THROW(exceptions::InvalidUuid, "model",
                      std::string("Parent UUID cannot be updated. ") + T::get_collection_name().to_string()
                      + " with uuid " + entry.get_uuid() + "', parent changed from "
                      + it->get_parent_uuid() + " to " + entry.get_parent_uuid());
            }
            const auto& db_hash = model::utils::Hash::from_resource(*it);
            const auto& entry_hash = model::utils::Hash::from_resource(entry);

            if (db_hash.status != entry_hash.status) {
                res = UpdateStatus::StatusChanged;
            }
            else if (db_hash.resource_without_status != entry_hash.resource_without_status) {
                res = UpdateStatus::Updated;
            }

            (*it) = std::move(entry);
        }
        else {
            m_manager_data.push_back(std::move(entry));
            res = UpdateStatus::Added;
        }
        return res;
    }

    T get_entry(const std::string& uuid) const {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        const auto it = find_entry(uuid);
        if (m_manager_data.end() != it) {
            return *it;
        }
        THROW(exceptions::InvalidUuid, "model",
              std::string(T::get_collection_name().to_string()) + " [UUID = '" + uuid + "'] not found.");
    }


    ManagerDataVec get_entries(Filter filter = [](const T&) { return true; }) {
        ManagerDataVec ret{};
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        for (const auto& entry: m_manager_data) {
            if (filter(entry)) {
                ret.emplace_back(entry);
            }
        }
        return ret;
    }


    ManagerDataVec get_entries(const std::string& parent_uuid, Filter filter = [](const T&) { return true; }) {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        return get_entries([&parent_uuid, &filter](const T& entry) {
            return parent_uuid == entry.get_parent_uuid() && filter(entry);
        });
    }

    Reference get_entry_reference(const std::string& uuid) {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        auto it = std::find_if(m_manager_data.begin(),
                            m_manager_data.end(), [&uuid](const T& entry) {
                                return (entry.get_persistent_uuid() == uuid || entry.get_temporary_uuid() == uuid);
                            });
        if (m_manager_data.end() != it) {
            return Reference(*it, m_mutex);
        }
        THROW(exceptions::InvalidUuid, "model",
              std::string(T::get_collection_name().to_string()) + " [UUID = '" + uuid + "'] not found.");
    }

    using Hook = std::function<void(const T&)>;
    void remove_entry(const std::string& uuid, Hook pre_delete_hook = [](const T&) {}) {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        const auto it = find_entry(uuid);
        if (m_manager_data.cend() != it) {
            pre_delete_hook(*it);
            m_manager_data.erase(it);
        }
    }

    void remove_by_parent(const std::string& uuid) {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        auto n = remove_if([&uuid](const T& entry) { return entry.get_parent_uuid() == uuid; });
        if (n != 0) {
            log_info("model", "Removed " << n << " " << T::get_collection_name().to_string() << ", parent " << uuid);
        }
    }

    void clear_entries() {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        m_manager_data.clear();
    }

    KeysVec get_keys() const {
        KeysVec keys{};
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        for (const auto& entry : m_manager_data) {
            keys.emplace_back(entry.get_uuid());
        }
        return keys;
    }

    /*!
     * @brief Get keys for all objects that pass through filter
     *
     * @param filter A std::function, eg. a lambda, serving as a filter
     *
     * @return Vector of UUIDs
     * */
    KeysVec get_keys(Filter filter = [](const T&) { return true; }) {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        KeysVec keys{};
        for (const auto& entry : m_manager_data) {
            if (filter(entry)) {
                keys.emplace_back(entry.get_uuid());
            }
        }
        return keys;
    }

    KeysVec get_keys(const std::string& parent_uuid, Filter filter = [](const T&) { return true; }) {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        return get_keys([&parent_uuid, &filter](const T& entry) {
            return parent_uuid == entry.get_parent_uuid() && filter(entry);
        });
    }

    /*!
     * @brief get ids of all objects governed by a manager
     *
     * @return vector of ids
     */
    IdsVec get_ids() {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        IdsVec ids{};
        for (const auto& entry : m_manager_data) {
            ids.emplace_back(entry.get_id());
        }
        return ids;
    }

    IdsVec get_ids(const std::string& parent_uuid) {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        IdsVec ids{};
        for (const auto& entry : m_manager_data) {
            if (entry.get_parent_uuid() == parent_uuid) {
                ids.emplace_back(entry.get_id());
            }
        }
        return ids;
    }

    bool entry_exists(const std::string& uuid) override {
        return m_manager_data.cend() != find_entry(uuid);
    }

    std::size_t get_entry_count() const {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        return m_manager_data.size();
    }

    std::size_t get_entry_count(const std::string& parent_uuid) {
        return static_cast<unsigned long>(std::count_if(
                             m_manager_data.cbegin(),
                             m_manager_data.cend(),
                             [&parent_uuid](const T& entry) {
                                return entry.get_parent_uuid() == parent_uuid;
                             }));
    }

    /*!
     * @brief rest_id_to_uuid - find object by REST url id.
     *
     * @param id rest url id
     * @param parent_uuid [optional, see below] an object's parent
     *
     * parent_uuid is empty iff we're searching for an object with a unique
     * REST id (for example /Systems/2)
     * on the other hand, for object with non-unique ids, parent id is required
     *
     * @return found object's uuid
     */
    const std::string& rest_id_to_uuid(std::uint64_t id, const std::string& parent_uuid = {}) const {
        if (parent_uuid.empty()) {
            return find_uuid_by_id(id);
        }
        else {
            return find_uuid_by_id_and_parent(id, parent_uuid);
        }
    }

    /*!
     * @brief Accessor for epoch number maintained by this GenericManager
     * @return epoch number
     */
    std::uint64_t get_current_epoch() {
        return m_current_epoch;
    }

    /*!
     * @brief rest_uuid_to_id - find REST url id by object's uuid
     *
     * @param uuid object's uuid
     *
     * @return found object's id
     */
    uint64_t uuid_to_rest_id(const std::string& uuid) {
        return find_id_by_uuid(uuid);
    }

    /*!
     * @brief Removes all resources associated with given agent
     *
     * @param gami_id Id of json agent
     * */
    void clean_resources_for_agent(const std::string& gami_id) {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        auto n = remove_if([&gami_id](const T& entry) { return entry.get_agent_id() == gami_id; });
        if (n != 0) {
            log_info("model", "Removed " << n << " " << T::get_collection_name().to_string() << ", agent " << gami_id);
        }
    }

protected:
    mutable std::recursive_mutex m_mutex{};
    ManagerDataVec m_manager_data{};
    std::atomic<std::uint64_t> m_current_epoch {1};

private:

    typename ManagerDataVec::const_iterator find_entry(const std::string& uuid) const {
        auto it = std::find_if(m_manager_data.cbegin(),
                            m_manager_data.cend(),
                            [&uuid](const T& entry) {
                                  return ((entry.has_persistent_uuid() && entry.get_persistent_uuid() == uuid) || entry.get_temporary_uuid() == uuid);
                            });
        return it;
    }

    typename ManagerDataVec::iterator find_entry(const std::string& uuid) {
        auto it = std::find_if(m_manager_data.begin(),
                               m_manager_data.end(),
                               [&uuid](const T& entry) {
                                   return ((entry.has_persistent_uuid() && entry.get_persistent_uuid() == uuid) || entry.get_temporary_uuid() == uuid);
                               });
        return it;
    }

    /*!
     * @brief find_uuid_by_id helper function for rest_id_to_uuid
     *
     * @param id object's REST id
     *
     * @return object's uuid
     */
    const std::string& find_uuid_by_id(std::uint64_t id) const {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        for (const auto& entry : m_manager_data) {
            if (entry.get_id() == id) {
                return entry.get_uuid();
            }
        }

        const auto& message = std::string("Could not find ") +
                T::get_collection_name().to_string() + " with ID: " + std::to_string(id) + ".";

        log_warning("model", message);
        throw exceptions::NotFound(message);
    }

    /*!
     * @brief find_uuid_by_id helper function for rest_id_to_uuid
     *
     * @param id object's REST id
     * @param parent_uuid object's parent
     *
     * @return object's uuid
     */
    const std::string& find_uuid_by_id_and_parent(std::uint64_t id, const std::string& parent_uuid) const {
        std::lock_guard<std::recursive_mutex> lock{m_mutex};
        for (const auto& entry : m_manager_data) {
            if (entry.get_parent_uuid() == parent_uuid && entry.get_id() == id) {
                return entry.get_uuid();
            }
        }

        const auto& message = std::string("Could not find ") +
                T::get_collection_name().to_string() + " with ID: " + std::to_string(id) + ".";
        log_warning("model", message + " Parent UUID = " + parent_uuid + ".");
        throw exceptions::NotFound(message);

    }

    /*!
     * @brief find_id_by_uuid helper function for uuid_to_rest_id
     * @param uuid object's uuid
     * @return object's REST id
     */
    uint64_t find_id_by_uuid(const std::string& uuid) {
        for (const auto& entry : m_manager_data) {
            if (entry.get_persistent_uuid() == uuid || entry.get_temporary_uuid() == uuid) {
                return entry.get_id();
            }
        }

        THROW(exceptions::InvalidUuid, "model",
            "Entry not found in the manager for UUID = " + uuid + ".");
    }

    /*!
     * @brief removes entries for which predicate returns true
     * @param predicate predicate to select entries to remove
     * @return number of removed entities
     * */
    template<typename Predicate>
    typename ManagerDataVec::difference_type remove_if(Predicate predicate){
        auto last = m_manager_data.end();
        auto first = std::remove_if(m_manager_data.begin(), last, predicate);
        auto count_removed = std::distance(first, last);
        if (count_removed) {
            m_manager_data.erase(first, last);
        }
        return count_removed;
    }

};

template <typename T>
GenericManager<T>::~GenericManager() {}

template<>
template<typename U>
GenericManager<model::Task>::UpdateStatus GenericManager<model::Task>::add_or_update_entry(U&& entry_r) {
    model::Task entry = std::forward<U>(entry_r);
    UpdateStatus res = UpdateStatus::NoUpdate;
    std::lock_guard<std::recursive_mutex> lock{m_mutex};
    entry.touch(++m_current_epoch);

    auto it = find_entry(entry.get_uuid());
    if (m_manager_data.end() != it) {
        if (it->get_parent_uuid() != entry.get_parent_uuid()) {
            THROW(exceptions::InvalidUuid, "model",
                  "Parent UUID cannot be updated. Entry = '" + entry.get_uuid() + "', parent changed from "
                  + it->get_parent_uuid() + " to " + entry.get_parent_uuid());
        }
        const auto& db_hash = model::utils::Hash::from_resource(*it);
        const auto& entry_hash = model::utils::Hash::from_resource(entry);

        if (db_hash.status != entry_hash.status) {
            res = UpdateStatus::StatusChanged;
        }
        else if (db_hash.resource_without_status != entry_hash.resource_without_status) {
            res = UpdateStatus::Updated;
        }

        (*it) = std::forward<U>(entry);
        if (UpdateStatus::NoUpdate != res && entry.get_end_time().has_value()) {
            it->call_completion_notifiers();
        }
    }
    else {
        m_manager_data.emplace_back(std::move(entry));
        res = UpdateStatus::Added;
    }
    return res;
}

}
}
