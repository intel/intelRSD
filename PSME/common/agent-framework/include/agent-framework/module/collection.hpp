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
 * @file module/collection.hpp
 * @brief Collection specific data used in commands response
 * */

#ifndef AGENT_FRAMEWORK_MODULE_COLLECTION_HPP
#define AGENT_FRAMEWORK_MODULE_COLLECTION_HPP

#include <string>
#include <vector>

namespace Json {
class Value;
}

namespace agent_framework {
namespace generic {

/*! Class representing collection attribute
 *  for commands response */
class Collection {
public:
    /*! Collection entry class */
    class CollectionEntry {
    public:
        /*! Constructor name and type are mandatory fields */
        CollectionEntry(const std::string& name, const std::string& type,
                        const std::string& slot_mask = "")
            : m_name(name), m_type(type), m_slot_mask(slot_mask) {}

        /*!
         * @brief Set collection entry name
         * @param name Entry name
         */
        void set_name(const std::string& name);

        /*!
         * @brief Get collection entry name
         * @return Entry name
         */
        const std::string& get_name() const;

        /*!
         * @brief Set collection entry type
         * @param type Entry type
         */
        void set_type(const std::string& type);

        /*!
         * @brief Get collection entry type
         * @return Entry type
         */
        const std::string& get_type() const;

        /*!
         * @brief Set collection entry slot mask
         * @param slot_mask Entry slot mask
         */
        void set_slot_mask(const std::string& slot_mask);

        /*!
         * @brief Get collection entry slot mask
         * @return Entry slot mask
         */
        const std::string& get_slot_mask() const;
    private:
        std::string m_name{};
        std::string m_type{};
        std::string m_slot_mask{};
    };
    /*! Collection entry array */
    using collection_t = std::vector<CollectionEntry>;

    /*! Collection constructors */
    Collection() = default;
    Collection(const Collection&) = default;
    Collection& operator=(const Collection&) = default;

    /*! Collection virtual destructor */
     ~Collection();

    /*!
     * @brief Convert Collection to JSON object
     * @return JSON representation of Collection object
     * */
    Json::Value to_json() const;

    /*!
     * @brief Add entry to collection
     * @param entry Collection entry object
     */
    void add_collection_entry(const CollectionEntry& entry);

    /*!
     * @brief Set collection
     * @param collection Collection array
     */
    void set_collection(const collection_t& collection);

    /*!
     * @brief Get collection
     * @return Collection array
     */
    const collection_t& get_collection() const;

private:
    collection_t m_collection{};
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_COLLECTION_HPP */
