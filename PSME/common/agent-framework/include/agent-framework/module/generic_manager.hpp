/*!
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
 *
 * @file module_manager.hpp
 * @brief Module Manager
 * */

#ifndef AGENT_FRAMEWORK_MODULE_MANAGERS_GENERIC_MANAGER_HPP
#define AGENT_FRAMEWORK_MODULE_MANAGERS_GENERIC_MANAGER_HPP

#include "agent-framework/exceptions/exception.hpp"
#include <vector>
#include <algorithm>

/*! Psme namespace */
namespace agent_framework {
namespace generic {

template <typename T>
class GenericManager {
public:
    using value_type = T;
    using ManagerDataVec = std::vector<T>;
    using KeysVec = std::vector<std::string>;

    virtual ~GenericManager();

    template <typename U>
    void add_entry(U&& entry) {
        m_manager_data.emplace_back(std::forward<U>(entry));
    }

    const T& get_entry(const std::string& uuid) const {
        for (const auto& elem : m_manager_data) {
            if (elem.get_uuid() == uuid) {
                return elem;
            }
        }

        THROW(::agent_framework::exceptions::InvalidParameters, "rpc",
          "Manager entry not found, invalid uuid " + uuid);
    }

    void remove_entry(const std::string& uuid) {
        const auto it = find_entry(uuid);
        if (m_manager_data.cend() != it) {
            m_manager_data.erase(it);
        }
    }

    void clear_entries() {
        m_manager_data.clear();
    }

    KeysVec get_keys() const {
        KeysVec keys{};
        for (const auto& entry : m_manager_data) {
            keys.emplace_back(entry.get_uuid());
        }
        return keys;
    }

    std::size_t get_entry_count() const {
        return m_manager_data.size();
    }

    bool find_elem(const std::string& uuid) const {
        for (const auto& elem : m_manager_data) {
            if (elem.get_uuid() == uuid) {
                return true;
            }
        }
        return false;
    }

private:
    typename ManagerDataVec::const_iterator
    find_entry(const std::string& uuid) const {
        return std::find_if(m_manager_data.cbegin(),
                            m_manager_data.cend(),
                            [&uuid](const T& entry) {
                                  return entry.get_uuid() == uuid;
                            });
    }

    typename ManagerDataVec::iterator find_entry(const std::string& uuid) {
        return std::find_if(m_manager_data.begin(),
                            m_manager_data.end(),
                            [&uuid](const T& entry) {
                                  return entry.get_uuid() == uuid;
                            });
    }

private:
    ManagerDataVec m_manager_data{};
};

template <typename T>
GenericManager<T>::~GenericManager() {}

}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODULE_MANAGER_HPP */

