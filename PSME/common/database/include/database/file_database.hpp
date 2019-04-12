/*!
 * @brief Database implementation
 *
 * Database entries are stored in the form of files, file name is "transcoded"
 * from the key, value is stored in the content.
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
 * @file file_database.hpp
 */

#pragma once

#include <functional>

#include "database/database.hpp"

namespace database {

/*!
 * @brief "Default" database to store key/value pairs as separate files in filesystem.
 */
class FileDatabase : public Database {

    friend class DatabaseTester;

public:
    /*!
     * @brief Create "plain file" database
     * @param name Name of the database
     * @param with_policy if retention policy attributes should be set for all files
     * @param location directory where to create all database files
     */
    FileDatabase(const std::string& name, bool with_policy, const std::string& location);

    virtual ~FileDatabase();

    bool start() override;
    bool next(Serializable& key, Serializable& value) override;
    void end() override;

    bool get(const Serializable& key, Serializable& value) override;
    bool put(const Serializable& key, const Serializable& value) override;
    bool remove(const Serializable& key) override;

    EntityValidity get_validity(const Serializable& key, std::chrono::seconds interval = NEVER) override;
    bool invalidate(const Serializable& key) override;

    unsigned cleanup(Serializable& key, std::chrono::seconds interval = NEVER) override;
    unsigned wipe_outdated(Serializable& key, std::chrono::seconds interval) override;
    unsigned drop(Serializable& key) override;

private:
    FileDatabase& operator=(const FileDatabase&) = delete;
    FileDatabase(const FileDatabase&) = delete;
    FileDatabase() = delete;

    /*!
     * @brief Database entries handled by retention policy
     */
    bool m_with_policy{};

    /*! Mutex to disallow iterating from multiple threads. */
    static std::recursive_mutex mutex;

    /*! @brief Function to be called on each found name */
    using ForeachFunction = std::function<bool(const std::string& stripped_name)>;

    /*!
     * @brief Iterate over all entries
     * @param key to iterate over
     * @param function to be executed on each matching name
     * @return number of processed entries
     */
    unsigned foreach(Serializable& key, ForeachFunction function);

    /*! @brief directory where files from DB are kept */
    const std::string path;

    /*! @brief Current state of iterating process */
    enum class IteratingState {
        NOT_STARTED, //!< Iterating not in progress
        STARTED, //!< Iterating was just started, but no next() was called
        ITERATE, //!< Iterating
        NO_MORE_DATA //!< No more data was found
    };
    IteratingState iterating_state{IteratingState::NOT_STARTED};

    /*! @brief List of found names to be used during iterating */
    using IteratedFiles = std::vector<std::string>;
    IteratedFiles iterated_files{};
    /*! Iterator to current name. to avoid iterated_files.remove() */
    IteratedFiles::iterator current_file{};

    /*!
     * @brief Return full file name for given (serialized) key
     * @param stripped_name Deserialized key
     * @return full name of the file
     */
    std::string full_name(const std::string& stripped_name) const;

    /*!
     * @brief Strip db name and extension from file name taken from the directory
     * @param d_name file name to be stripped. It should be same type as in the
     *               dirent structure (char[256]), but it is implementation
     *               dependant and no typedef is available!
     * @return stripped name
     */
    std::string strip_name(const std::string& d_name) const;

    /*!
     * @brief Read file with given name from default PATH directory
     * @param file_name Full name of the file to be read
     * @param[out] data content read from the file
     * @warning Files longer than VALUE_LEN are not handled
     */
    static bool read_file(const std::string& file_name, std::string& data);

    /*!
     * @brief Save data into file with requested name
     *
     * Sticky bit is set for saved file as well (this is, file stand
     * "unconditionally" valid).
     *
     * @param file_name Full name of the file where data is to be stored.
     * @param data data to be stored in the file.
     * @param with_policy if "policy" attributes are to be set
     * @return true if file saved without any issue
     */
    static bool save_file(const std::string& file_name, const std::string& data, bool with_policy);

    /*!
     * @brief Remove file from directory
     * @param file_name Full name of the file to be removed
     * @return true if file removed
     */
    static bool remove_file(const std::string& file_name);

    /*!
     * @brief Check file state ("validity")
     *
     * Several states of the file are checked: attributes (to see if file is "valid" and is to be handled
     * by "retention policy" databases) and last file modification time (to see if "invalid" file was not
     * updated for specified interval).
     *
     * @param file_name Full name of the file to be checked
     * @param interval when file stands outdated
     * @param with_policy if "policy" attributes are to be checked
     * @return state of the file
     */
    static EntityValidity file_validity(const std::string& file_name, std::chrono::seconds interval, bool with_policy);

    /*!
     * @brief Mark file as invalid
     *
     * Invalid file after specified interval is assumed to be outdated (and
     * removed from the disk).
     *
     * @param file_name Full name of the file to be marked as invalid
     * @return true if file is valid
     */
    static bool invalidate_file(const std::string& file_name);

    /*!
     * @brief Select directory for storing the files
     *
     * Given path (if exists) or /tmp/database_xxxxxx (created one)
     * directory is reported.
     */
    static std::string check_directory(const std::string& dir);

    /*! @brief Mapping of given locations and real paths */
    using Locations = std::map<std::string, std::string>;
    /*! @brief All checked paths used by the databases */
    static Locations locations;
};

} // @i{database}
