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
#ifndef PSME_ID_POLICY_HPP
#define PSME_ID_POLICY_HPP


#include "agent-framework/generic/singleton.hpp"
#include <atomic>
#include <cstdint>
#include <assert.h>
#include <map>

namespace psme {
namespace rest {
namespace model {
namespace handler {

namespace internal {

/*!
 * @brief According to redfish spec, different components ARE chassis.
 * Because they all should be identified by short IDs, unique in the
 * entire family of chassis we need a mechanism to assign numbers
 * that is common for ALL chassis-like components. That's why singleton
 *  is used here.
 * */
class ChassisIds : public agent_framework::generic::Singleton<ChassisIds> {
public:
    virtual ~ChassisIds();
    std::uint64_t next_id() {
        return m_id++;
    }
    void reset() {
        m_id = 1;
    }
private:
    std::atomic<std::uint64_t> m_id {1};
};
}

/*!
 * @brief IdMemoizer is used as a base class for other policies.
 * Its purpose is to ensure that fetching component multiple times
 * does not change rest id
 * */
class IdMemoizer {
public:
    bool get_memoized_id(const std::string& uuid, std::uint64_t& id) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        auto it = m_memoized_ids.find(uuid);
        if (it != m_memoized_ids.end()) {
            id = it->second;
            return true;
        }
        else {
            return false;
        }
    }
    void memoize_id(const std::string& uuid, std::uint64_t id) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        assert(m_memoized_ids.find(uuid) == m_memoized_ids.end());
        m_memoized_ids[uuid] = id;
    }
    void remove(const std::string& uuid) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        auto it = m_memoized_ids.find(uuid);
        if (it != m_memoized_ids.end()) {
            m_memoized_ids.erase(it);
        }
    }
    void reset() {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        m_memoized_ids.clear();
    }
protected:
    std::map<std::string, std::uint64_t> m_memoized_ids {};
    std::recursive_mutex m_mutex{};
};

/*!
 * @brief an IdPolicy is used to assign REST ids to model objects discovered
 * from agents. The following class is a virtual base class for all IdPolicies.
 */
class IdPolicy : public IdMemoizer {
public:
    virtual ~IdPolicy();
    /*!
     * @brief generate na id, which is unique either globally among objects of
     * the same type or for objects with the same parent, and which is to be
     * assigned to newly discovered object.
     *
     * @param parent_uuid the uuid of an object's parent
     *
     * @return the generated ids
     */
    std::uint64_t next_id(const std::string& uuid, const std::string& parent_uuid) {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        std::uint64_t id;
        if (get_memoized_id(uuid, id)) {
            return id;
        }
        id = do_next_id(parent_uuid);
        memoize_id(uuid, id);
        return id;
    }

    /*!
     * @brief Purge IdPolicy object
     *
     * By default, the function removes the UUID from its databse. It can be
     * overriden in derived classes to gain custom behaviour with comparison
     * to the remove() method.
     *
     * param[in] uuid UUID to be purged
     * */
    virtual void purge(const std::string& uuid, const std::string&) {
        remove(uuid);
    }
protected:
    virtual std::uint64_t do_next_id(const std::string& parent_uuid) = 0;
    virtual void reset() {
        IdMemoizer::reset();
    }
    friend class HandlerTest;
};

/*!
 * @brief LocalIdPolicy is used to generate ids for objects whose id is
 * unigue with respect to a parent, eg. /Systems/yyy/StorageController/xxx
 * It contains a map tracking parent uuids (equivalent of yyy) and the last
 * id assigned (xxx)
 */
class LocalIdPolicy final : public IdPolicy {
public:
    ~LocalIdPolicy();

    /*!
     * @brief Purge LocalIdPolicy object
     *
     * It removes the parent_uuid from the map of known parents, so that the
     * next id given to a resource for a given parent will start from beginning.
     * */
    virtual void purge(const std::string& uuid, const std::string& parent_uuid) override {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        IdPolicy::purge(uuid, parent_uuid);
        auto it = m_ids.find(parent_uuid);
        if(m_ids.end() != it) {
            m_ids.erase(it);
        }
    }
protected:
    /*!
     * @brief generate ids according to policy (see class documentation above)
     *
     * @param parent_uuid the uuid of an object's parent
     *
     * @return the generated ids
     */
    std::uint64_t do_next_id(const std::string& parent_uuid) override {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        auto it = m_ids.find(parent_uuid);
        auto id = (it == m_ids.end()) ? 1ul : it->second + 1;
        m_ids[parent_uuid] = id;
        return id;
    }
private:
    // We use a map, m_ids, connecting the parent object uuid (std::string) and
    // the last id (std::uint64_t) that has been assigned to an object of the
    // handled class with this parent
    using IdMap = std::map<std::string, std::uint64_t>;
    void reset() override {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        IdPolicy::reset();
        m_ids.clear();
    }
    IdMap m_ids {};
    friend class HandlerTest;
};

/*!
 * @brief GlobalId policy is used to generate ids for top level objects with
 * global ids, eg. /Systems/xxx or /Switches/xxx
 */
class GlobalIdPolicy final : public IdPolicy {
public:
    ~GlobalIdPolicy();
protected:
    /*!
     * @brief generate ids according to policy (see class documentation above)
     *
     * @return the generated ids
     */
    std::uint64_t do_next_id(const std::string&) override {
        return m_id++;
    }
    void reset() override {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        IdPolicy::reset();
        m_id = 1;
    }
    std::atomic<std::uint64_t>m_id {1};
    friend class HandlerTest;
};

/*!
 * @brief ChassisIdPolicy is used to generate REST ids for Chassis objects. It
 * requires a singleton ChassisIds, because many classes of objects are mapped
 * to Chassis endpoints
 */
class ChassisIdPolicy final : public IdPolicy {
public:
    ~ChassisIdPolicy();
protected:
    /*!
     * @brief generate ids according to policy (see class documentation above)
     *
     * @return the generated ids
     */
    std::uint64_t do_next_id(const std::string&) override {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        return internal::ChassisIds::get_instance()->next_id();
    }
    void reset() override {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        IdPolicy::reset();
        internal::ChassisIds::get_instance()->reset();
    }
    friend class TestHandler;
};

}
}
}
}
#endif //PSME_ID_POLICY_HPP
