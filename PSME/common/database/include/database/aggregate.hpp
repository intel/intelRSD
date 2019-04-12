/*!
 * @brief A simple aggregate type database for PersistentAttributes.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file aggregate.hpp
 */

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <unordered_map>

namespace database {

class PersistentAttributes;

/*!
 * @brief A class to aggregate PersistentAttributes.
 *
 * The PersistentAttributes class was designed to store data as strings. Each single parameter would therefore be stored
 * as a separate file, which leads to an explosion of files. The Aggregate class is a builder for attributes/parameters
 * which are dependent and stores them in a single file with a well defined format.
 *
 * There are two types of parameters that can be added to an Aggregate instance. Single parameters are just simple
 * key-value pairs, where both the string and value is a string. Multiple parameters are held in buckets and each
 * Aggregate instance can have an arbitrary number of unique buckets identified by a string name. Each bucket can hold
 * an arbitrary number of key-value pairs, where the key is auto-generated from the bucket name and a sequence number
 * (0-based). For a bucket "foo", the subsequent keys will be "foo_0", "foo_1", "foo_2"...
 *
 * Each key-value pair is separated with a customizable character, by default ';'. Each key is separated from its value
 * by '=' and "MULTIPLE" starts a new multivalue. Both "=" and "MULTIPLE" are reserved and cannot be used as part of a
 * key. This is serialized to: key0=single_value;...;MULTIPLE=bucket_name;bucket_name_0=value0;bucket_name_1=value1;...
 * Each aggregate can have more than one "MULTIPLE=..." defined.
 *
 * A version number is appended at the beginning of the string representation for compatibility verification. The
 * version string has the form of MAJOR.MINOR, where both MAJOR and MINOR are integers and MAJOR >= 1 and MINOR >= 0.
 */
template<typename T>
class Aggregate final {
public:
    // key - value pairs
    using SinglesContainer = std::unordered_map<std::string, std::string>;

    // named buckets of key-value pairs, keys are automatically generated
    using MultiplesContainer = std::unordered_map<std::string, SinglesContainer>;

    // a vector of aggregates
    using AggregateContainer = std::vector<Aggregate>;

    /*!
     * @brief Aggregate constructor.
     *
     * @param pa A reference to the PersistentAttributes instance.
     * @param uuid The uuid of the resource for PersistentAttributes.
     * @param name The name for the attributes aggregate.
     * @param separator The separator for values in the aggregate string representation (31 = ASCII unit separator).
     */
    Aggregate(T& pa, const std::string& uuid, const std::string& name, char separator = 31) :
        m_sep{separator}, m_pa{pa}, m_name{name}, m_uuid{uuid} {
        std::string value;
        m_pa.get_value(uuid, name, value);
        parse(value);
    }

    /*!
     * @brief Defaulted copy constructor.
     */
    Aggregate(const Aggregate& rhs) = default;

    /*!
     * @brief Defaulted assignment operator.
     */
    Aggregate& operator=(const Aggregate& rhs) = default;

    /*!
     * @brief Defaulted move assignment operator.
     */
    Aggregate(Aggregate&& rhs) = default;

    /*!
     * @brief Defaulted move constructor.
     */
    Aggregate& operator=(Aggregate&& rhs) = default;

    /*!
     * @brief Defaulted copy constructor.
     */
    ~Aggregate() = default;

    /*!
     * @brief Add a value to the aggregate.
     *
     * @param key The name of the value to be added.
     * @param value The value.
     */
    void put(const std::string& key, const std::string& value) {
        check_key(key);
        m_singles.emplace(key, value);
    }

    /*!
     * @brief Put multiple key_value pairs in the aggregate.
     *
     * @param init_list An initializer list of values to be added.
     */
    void put(std::initializer_list<SinglesContainer::value_type> init_list) {
        for (const auto& val : init_list) {
            check_key(val.first);
        }

        m_singles.insert(init_list);
    }

    /*!
     * @brief Get a value from the aggregate.
     *
     * @param key The value to be returned.
     * @return A copy of the value.
     */
    std::string get(const std::string& key) const {
        return find(key)->second;
    }

    /*!
     * @brief Return a collection of values from a given bucket.
     *
     * @param bucket The name of the bucket.
     * @return A collection of values stored in the bucket.
     */
    std::vector<std::string> get_multiple_values(const std::string& bucket) const {
        std::vector<std::string> values;
        if (m_multiples.count(bucket)) {
            for (const auto& pair : m_multiples.at(bucket)) {
                values.push_back(pair.second);
            }
        }

        return values;
    }

    /*!
     * @brief Return all aggregates, where the name is equal to a PersistentAttribute name.
     *
     * @param pa A reference to the PersistentAttributes instance.
     * @param name The name for the attributes aggregate.
     *
     * @return A container of aggregates with matching names.
     */
    static AggregateContainer get_aggregates(T& pa, const std::string& name) {
        AggregateContainer container;
        for (const auto& record : pa.get_persistent_settings()) {
            if (record.second == name) {
                container.emplace_back(pa, record.first, record.second);
            }
        }

        return container;
    }

    /*!
     * @brief Remove a value from the aggregate.
     *
     * @param key The name of the value to be removed.
     */
    SinglesContainer::size_type del(const std::string& key) {
        auto removed = m_singles.erase(key);

        for (auto& singles : m_multiples) {
            try {
                // the key is generated automatically, the key parameter is in fact the value in multiples
                auto multiples_key = get_key_by_value(singles.first, key);
                removed += singles.second.erase(multiples_key);
            } catch (const std::out_of_range&) {
            }
        }

        return removed;
    }

    /*!
     * @brief Update a specific value in the aggregate.
     *
     * @param key The name of the value to be updated.
     * @param value The new value.
     */
    void update(const std::string& key, const std::string& value) {
        find(key)->second = value;
    }

    /*!
     * @brief Append a new sequenced value in the aggregate.
     *
     * The value will be appended to the specified bucket and the key/name will be automatically generated as
     * $bucket_#, where bucket is the first method parameter and # is the next index in the container. 0-based.
     *
     * @param bucket The bucket to insert the value into.
     * @param value The value to be added.
     */
    void append(const std::string& bucket, const std::string& value) {
        check_key(bucket);
        std::stringstream key;
        auto index = m_multiples[bucket].size();
        key << bucket << "_" << index;
        // If the key already exists then increment the index and try again
        while (m_multiples[bucket].find(key.str()) != m_multiples[bucket].end()) {
            index++;
            key.str(std::string()); // Resetting the key
            key.clear();
            key << bucket << "_" << index;
        }
        m_multiples[bucket].emplace(key.str(), value);
    }

    /*!
     * @brief Dump the aggregate contents into the PersistentAttributes instance.
     */
    void dump() const {
        std::stringstream stringified;
        stringified << MAJOR << "." << MINOR << m_sep;

        dump_singles(stringified, m_singles);
        for (const auto& entry : m_multiples) {
            stringified << MULT_MARK << KV_SEP << entry.first << m_sep;
            dump_singles(stringified, entry.second);
        }

        m_pa.set_value(m_uuid, m_name, stringified.str());
    }

    /*!
     * @brief Get a const reference to all singles of the aggregate.
     *
     * @return Reference to the container of singles.
     */
    const SinglesContainer& get_singles() const {
        return m_singles;
    }

    /*!
     * @brief Get a const reference to all multiples of the aggregate.
     *
     * Multiples is a named container(map) of singles.
     *
     * @return Reference to the container of multiples.
     */
    const MultiplesContainer& get_multiples() const {
        return m_multiples;
    }

    /*!
     * @brief Get aggregate's PersistentAttribute name.
     *
     * @return The name of the PersistentAttribute.
     */
    const std::string& get_name() const {
        return m_name;
    }

    /*!
     * @brief Get aggregate's PersistentAttribute uuid.
     *
     * @return The uuid of the PersistentAttribute.
     */
    const std::string& get_uuid() const {
        return m_uuid;
    }

    /*!
     * @brief Remove all attributes from database
     */
    void remove() {
        m_pa.remove_value(m_uuid, m_name);
    }

private:
    using position_extractor = std::function<std::string::size_type(const std::string&)>;

    /*!
     * @brief Search a given bucket for a specific value and return the name of the value.
     *
     * Note: O(n) complexity.
     *
     * @param bucket The name of the bucket.
     * @param value The value to find.
     * @return The name of the found value.
     */
    std::string get_key_by_value(const std::string& bucket, const std::string& value) const {
        for (const auto& pair : m_multiples.at(bucket)) {
            if (pair.second == value) {
                return pair.first;
            }
        }

        throw std::out_of_range(std::string{"database::Aggregate: value not found: "} += value);
    }

    /*!
     * @brief Read contents form the PersistentAttributes.
     *
     * @param value String representation to be parsed.
     */
    void parse(const std::string& value) {
        if (value.empty())
            return;

        auto extract_pos = [this](const std::string& item) -> std::string::size_type {
            auto pos = item.find(KV_SEP);
            if(std::string::npos == pos) {
                throw std::runtime_error("database::Aggregate: Malformed KV pair");
            }

            return pos;
        };

        std::stringstream stream{value};
        std::string item;

        // check version
        if (std::getline(stream, item, m_sep)) {
            check_version(item);
        }

        while (std::getline(stream, item, m_sep)) {
            if (item.find(MULT_MARK) != std::string::npos) {
                parse_multiple(stream, item.substr(extract_pos(item) + 1, item.size()), extract_pos);
            } else {
                auto pos = extract_pos(item);
                // push single KV pair
                m_singles.emplace(item.substr(0, pos), item.substr(pos + 1, item.size()));
            }
        }
    }

    /*!
     * @brief Parse a multi-value entry.
     *
     * @param stream The stream to parse.
     * @param bucket The name of the bucket being parsed.
     * @param extract_pos Delimiter extraction function.
     */
    void parse_multiple(std::stringstream& stream, const std::string& bucket, position_extractor extract_pos) {
        std::string item;

        while (std::getline(stream, item, m_sep)) {
            auto pos = extract_pos(item);
            // new multiple found
            if (item.find(MULT_MARK) != std::string::npos) {
                parse_multiple(stream, item.substr(pos + 1, item.size()), extract_pos);
            } else {
                // push single KV pair into the appropriate bucket
                m_multiples[bucket].emplace(item.substr(0, pos), item.substr(pos + 1, item.size()));
            }
        }
    }

    /*!
     * @brief Const find member function.
     *
     * @param key The key to find.
     * @return Const iterator to the found element, end(container) otherwise.
     */
    SinglesContainer::const_iterator find(const std::string& key) const {
        auto ret = m_singles.find(key);

        if (m_singles.end() != ret) {
            return ret;
        } else {
            // not found in singles, search multiples
            for (const auto& singles : m_multiples) {
                try {
                    // the key is generated automatically, the key parameter is in fact the value in multiples
                    auto multiples_key = get_key_by_value(singles.first, key);
                    ret = singles.second.find(multiples_key);
                    if (singles.second.end() != ret) {
                        return ret;
                    }
                } catch (const std::out_of_range&) {
                }
            }
        }

        throw std::out_of_range(std::string{"database::Aggregate: Key not found: "} += key);
    }

    /*!
     * @brief Non-const find member function.
     *
     * @param key The key to find.
     * @return Iterator to the found element, end(container) otherwise.
     */
    SinglesContainer::iterator find(const std::string& key) {
        auto ret = m_singles.find(key);

        if (m_singles.end() != ret) {
            return ret;
        } else {
            // not found in singles, search multiples
            for (auto& singles : m_multiples) {
                try {
                    // the key is generated automatically, the key parameter is in fact the value in multiples
                    auto multiples_key = get_key_by_value(singles.first, key);
                    ret = singles.second.find(multiples_key);
                    if (singles.second.end() != ret) {
                        return ret;
                    }
                } catch (const std::out_of_range&) {
                }
            }
        }

        throw std::out_of_range(std::string{"database::Aggregate: Key not found: "} += key);
    }

    /*!
     * @brief Dump the contents of a singles container into a stream.
     *
     * @param stream The stream to dump to.
     * @param singles The container to dump.
     */
    void dump_singles(std::stringstream& stream, const SinglesContainer& singles) const {
        for (const auto& entry : singles) {
            stream << entry.first << KV_SEP << entry.second << m_sep;
        }
    }

    /*!
     * @brief Check if the version is supported.
     *
     * @param version The version string to check.
     */
    void check_version(const std::string& version) const {
        int major{-1};
        char c;
        int minor{-1};

        std::istringstream ss{version};

        if ( !(ss >> major)  ||  !(ss >> c)  ||  (c != '.')  ||  !(ss >> minor) ) {
            throw std::runtime_error("database::Aggregate: Could not read version string");
        }

        if (major != MAJOR || minor != MINOR) {
            throw std::runtime_error("database::Aggregate: Unsupported version");
        }
    }

    void check_key(const std::string& key) const {
        if (key.find(MULT_MARK) != std::string::npos || key.find(KV_SEP) != std::string::npos) {
            throw std::runtime_error(std::string{"database::Aggregate: Forbidden key used: "} += key);
        }
    }

    static constexpr int MAJOR = 1;
    static constexpr int MINOR = 0;
    static constexpr char KV_SEP = '=';
    static const std::string MULT_MARK;

    MultiplesContainer m_multiples{};
    SinglesContainer m_singles{};
    const char m_sep;
    T& m_pa;
    std::string m_name;
    std::string m_uuid;
};

template<typename T>
const std::string Aggregate<T>::MULT_MARK = "MULTIPLE";

}  // namespace database
