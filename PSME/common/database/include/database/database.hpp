/*!
 * @brief General database interface
 *
 * This database is very simple, "single" table (non entity-relationship)
 * database. It handles only key/value pairs. All keys are kept unique.
 *
 * database allows two schemas of operations: iterate over the entities and
 * single entity operations. These schemas exclude themselves (iteration is
 * done only during initialization, while modification afterwards "on event").
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file database.hpp
 */

#pragma once

#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>


namespace database {

/*! @brief Forward declaration for base class of keys and values kept in database */
class Serializable;

class Database {

    friend class DatabaseTester;

public:
    /*! @brief Interval to avoid some files */
    static const std::chrono::seconds NEVER;

    /*! @brief Empty string */
    static const std::string EMPTY;

    /*! @brief Default type for database pointer */
    using SPtr = std::shared_ptr<Database>;

    /*! @brief Validity type of the entry */
    enum class EntityValidity {
        ERROR,   //!< Cannot read/stat the file
        NOENTRY, //!< File has no appropriate flags set
        VALID,   //!< File has sticky bit set
        INVALID, //!< File has sticky bit cleared
        OUTDATED //!< File has sticky bit cleared and status was changed before interval
    };

    /*!
     * @brief Set default resource "location" for the database
     *
     * Databases are stored under given location or in default one
     * (if not given).
     *
     * @param location directory name where the data is to be stored.
     */
    static void set_default_location(const std::string& location);

    /*!
     * @brief database factory method
     *
     * Creates database. Appropriate type is created, based on location.
     * All databases are kept and tracked statically by the name.
     * (It is allowed to have only one database under the name created)
     * All databases with name starting with '*' are related to all data
     * within same path, this allows to iterate over all the databases
     * (in the location).
     *
     * @param name database name to be returned
     * @param with_policy if entries should be handled by retention policy
     * @param location to store the data or empty if default is to be used
     * @return database pointer for requested name.
     */
    static SPtr create(const std::string& name, bool with_policy = false, const std::string& location = EMPTY);


    /*! @brief Get name of the database */
    const std::string& get_name() const;

    /*!
     * @brief start database iteration.
     *
     * Iterating pattern over the data must be:
     * @verbatim
     * XxxKey key;
     * XxxValue value;
     * start();
     * while (next(key, value)) {
     *     ... do something with key/value pair ...
     * }
     * end();
     * @endverbatim
     */
    virtual bool start() = 0;


    /*!
     * @brief get next entry from the database
     * @param[out] key next key from the database
     * @param[out] value value for the key
     * @return true if key/value pair was read successfully
     */
    virtual bool next(Serializable& key, Serializable& value) = 0;


    /*! @brief end database iteration. */
    virtual void end() = 0;

    /*!
     * @brief get value for given key
     * @param key key to look for
     * @param[out] value where to be stored
     * @return true if entity exists
     */
    virtual bool get(const Serializable& key, Serializable& value) = 0;

    /*!
     * @brief store value under given key
     * @param key key to look for
     * @param[out] value what is to be stored
     * @return true if value was stored without any issue
     */
    virtual bool put(const Serializable& key, const Serializable& value) = 0;

    /*!
     * @brief remove value under given key
     * @param key entity to be removed identification
     * @return true if entry was removed
     */
    virtual bool remove(const Serializable& key) = 0;

    /*!
     * @brief Check entity validity
     * @param key Identifier of the entity to be marked.
     * @param interval in [s] to check if entity is outdated, if 0 is given no
     *                 outdating is checked
     * @return entity validity
     */
    virtual EntityValidity get_validity(const Serializable& key, std::chrono::seconds interval = NEVER) = 0;

    /*!
     * @brief Mark entity as invalid
     *
     * Outdated entities are wiped from the storage on wipe_invalid().
     *
     * @param key Identifier of the entity to be marked.
     * @return true if entity was made invalid
     */
    virtual bool invalidate(const Serializable& key) = 0;

    /*!
     * @brief Cleanup the database
     *
     * All valid entities are marked as invalid, all outdated entities are removed.
     *
     * @param key key to look for
     * @param interval in [s] when entity is outdated
     * @return number of removed or invalidated entries
     */
    virtual unsigned cleanup(Serializable& key, std::chrono::seconds interval = NEVER) = 0;

    /*!
     * @brief Wipe out all outdated entries
     *
     * Removes all invalid entries which are older than given interval.
     *
     * @param key key to look for
     * @param interval in [s] when entity is outdated
     * @return number of removed entries
     */
    virtual unsigned wipe_outdated(Serializable& key, std::chrono::seconds interval) = 0;

    /*!
     * @brief Drop the database
     *
     * Unconditionally removes all valid entries from the storage.
     *
     * @param key instance of key to "parse" found names
     * @return Number of removed entries
     */
    virtual unsigned drop(Serializable& key) = 0;

    /*!
     * @brief Mark all IDs as invalid and remove outdated ones
     *
     * Invalidate all valid IDs and remove all outdated ones. This should be
     * executed before IdPolicies are created and when they are destructed.
     * "Special" database (with empty name) is used to process all entries
     * (in all databases), so static context is assumed.
     *
     * @param interval Time [s] to identify outdated IDs
     * @return number of invalidated entries
     */
    static unsigned invalidate_all(std::chrono::seconds interval);


    /*!
     * @brief Remove all outdated IDs
     *
     * All invalid IDs which have not been changed in the last "interval" seconds.
     * "Special" database (with empty name) is used to process all entries (in all
     * databases), so static context is assumed.
     *
     * @param interval Time [s] to identify outdated IDs
     * @return number of removed entries
     */
    static unsigned remove_outdated(std::chrono::seconds interval);

    /*!
     * @brief Remove database from the pool
     *
     * It allows to "reuse" database name. Database is to be recreated.
     */
    void remove();

protected:
    /*!
     * @brief Database constructor.
     *
     * General constructor to build Database object. It constructs database with given name.
     *
     * @param name Database name.
     */
    Database(const std::string& name);

    /*! @brief Destructor */
    virtual ~Database();

private:

    /*! @brief Collection of all created databases */
    using Databases = std::vector<SPtr>;


    Database(const Database&) = delete;
    Database() = delete;

    /*! @brief (Unique) database name */
    std::string name{};

    /*! @brief Mutex to disallow access to database list from multiple threads. */
    static std::recursive_mutex mutex;

    /*! @brief All created databases. */
    static Databases databases;

    /*! @brief Default path to the databases. */
    static std::string default_location;
};

/*!
 * @brief Serializable object interface
 *
 * It allows keeping objects (either key or value) in the very simple database
 * without any knowledge about the objects (how to store them).
 */
class Serializable {
public:
    virtual ~Serializable();

    /*!
     * @brief String representation of value
     * @return unique string for unique object
     */
    virtual std::string serialize() const = 0;

    /*!
     * @brief Fill object with the data from the string
     * @param str string to be put in the object
     * @return true if value is ok and object is filled
     */
    virtual bool unserialize(const std::string& str) = 0;

protected:
    /*! @brief Default constructor. */
    Serializable();
};

std::ostream& operator<<(std::ostream& stream, const Serializable& str);

/*!
 * @brief Any string serializable
 *
 * Class to allow to use std::string as a key or value.
 * No aditional parsing/validation is done here, any string passed
 * (the key might be not valid thus)
 */
class String final : public Serializable {
    friend std::ostream& operator<<(std::ostream& stream, const String& str);

public:
    String();
    String(const std::string& str);
    String(std::string&& str);

    String& operator=(const std::string& str);
    String& operator=(std::string&& str);
    void set(const std::string& str) {
        m_str = str;
    }

    operator std::string() const {
        return m_str;
    }
    const std::string& get() {
        return m_str;
    }

private:
    std::string m_str;

    std::string serialize() const override;

    bool unserialize(const std::string& str) override;
};


std::ostream& operator<<(std::ostream& stream, const String& str);


/*!
 * @brief Simple key maching all entries in the database
 * @warning Cannot be used to handle requests, only to iterate over the data.
 */
class AlwaysMatchKey final : public Serializable {
    std::string serialize() const override __attribute__((noreturn));
    bool unserialize(const std::string& str) override;
};

/*! @brief Keep resource identification as pair of parent UUID and resource UUID. */
class UuidKey : public Serializable {
public:
    /*!
     * @brief Creates key without parent
     * @param _uuid Entity UUID
     */
    UuidKey(const std::string& _uuid) :
        uuid(_uuid),
        parent("") { }


    /*!
     * @brief Creates key for resource with parent
     * @param _uuid Entity UUID
     * @param _parent Parent UUID
     */
    UuidKey(const std::string& _uuid, const std::string& _parent) :
        uuid(_uuid),
        parent(_parent) { }


    /*! @brief Creates empty key */
    UuidKey() :
        uuid(""),
        parent("") { }


    /*! @brief Get stored UUID value */
    const std::string& get_uuid() const { return uuid; }


    /*! @brief Get stored parent UUID value */
    const std::string& get_parent() const { return parent; }


private:
    std::string uuid;
    std::string parent;


    std::string serialize() const override;


    bool unserialize(const std::string& str) override;
};



/*! @brief Representation if <<REST>> ID used by resources */
class IdValue : public Serializable {
public:
    /*! @brief Typedef for ID */
    using IdType =  std::uint64_t;

    /*!
     * @brief Creates exact ID
     * @param _id ID to be assigned
     */
    IdValue(IdType _id) : id(_id) { }

    /*! @brief Creates empty ID */
    IdValue() { }

    /*! @brief Get stored value */
    IdType get() const { return id; }

    /*!
     * @brief Set new value
     * @param _id ID to be assigned
     */
    void set(IdType _id) { id = _id; }

private:
    IdType id{0};

    virtual std::string serialize() const override;
    virtual bool unserialize(const std::string& str) override;
};

/*! @brief Key for resource last ID with filtering per parent */
class ResourceLastKey : public Serializable {
public:
    static const std::string LAST;

    /*! @brief Creates key to iterate over existing data */
    ResourceLastKey(const std::string& _parent):
        parent(_parent),
        resource("") { }

private:
    const std::string& parent;
    std::string resource;

    std::string serialize() const override;
    bool unserialize(const std::string& str) override;
};


} // @i{database}
