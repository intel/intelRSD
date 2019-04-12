/*!
 * @brief General database interface
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation
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
 * @file database.cpp
 */

#include "database/file_database.hpp"

#include "generic/assertions.hpp"
#include "logger/logger.hpp"
#include "logger/logger_factory.hpp"

#include <cstdint>
#include <thread>

using namespace database;
using namespace generic;

const std::chrono::seconds Database::NEVER{INT64_MAX};

const std::string Database::EMPTY{""};

std::string Database::default_location = "/var/opt/psme";

void Database::set_default_location(const std::string& location) {
    default_location = location;
}

Database::SPtr Database::create(const std::string& name, bool with_policy, const std::string& location) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    /* check if database is already created */
    for (auto& db : databases) {
        if (db->get_name() == name) {
            log_error("db", "Database '" << name << "' already created");
            assert(FAIL("Database already created"));
            return db;
        }
    }

    /* proper database type is to be created here.. Currently only FileDatabase is handled */
    const std::string n = (name.substr(0, 1) == "*") ? "" : name;
    SPtr added{new FileDatabase(n, with_policy, location.empty() ? default_location : location)};

    databases.push_back(added);
    return added;
}

std::recursive_mutex Database::mutex{};

Database::Databases Database::databases{};

Database::~Database() { }

Database::Database(const std::string& _name) : name(_name) { }

const std::string& Database::get_name() const {
    return name;
}

void Database::remove() {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    Databases::iterator it;
    for (it = databases.begin(); it != databases.end(); it++) {
        if ((*it)->get_name() == get_name()) {
            databases.erase(it);
            return;
        }
    }
    log_error("db", "Database " << get_name() << " already removed");
}

unsigned Database::invalidate_all(std::chrono::seconds interval) {
    SPtr db{Database::create("*retention", true)};
    AlwaysMatchKey key{};
    /* invalidate valid entries, remove outdated. Don't touch non related ones */
    auto ret = db->cleanup(key, interval);
    db->remove();
    return ret;
}

unsigned Database::remove_outdated(std::chrono::seconds interval) {
    SPtr db{Database::create("*retention", true)};
    AlwaysMatchKey key{};
    /* remove outdated files, don't touch non related ones */
    unsigned ret = db->wipe_outdated(key, interval);
    db->remove();
    return ret;
}


Serializable::Serializable() { }

Serializable::~Serializable() { }

std::ostream& database::operator<<(std::ostream& stream, const Serializable& val) {
    stream << val.serialize();
    return stream;
}

String::String() : m_str("") { }

String::String(const std::string& str) : m_str(str) { }

String::String(std::string&& str) : m_str(std::move(str)) { }

String& String::operator=(const std::string& str) {
    m_str = str;
    return *this;
}

String& String::operator=(std::string&& str) {
    m_str = std::move(str);
    return *this;
}

std::ostream& database::operator<<(std::ostream& stream, const String& str) {
    stream << str.m_str;
    return stream;
}

std::string String::serialize() const {
    return m_str;
}

bool String::unserialize(const std::string& str) {
    m_str = str;
    return true;
}



std::string UuidKey::serialize() const {
    if (!parent.empty()) {
        return parent + "." + uuid;
    }
    return uuid;
}

bool UuidKey::unserialize(const std::string& str) {
    size_t pos = str.find('.');
    if (pos != std::string::npos) {
        parent = str.substr(0, pos);
        uuid = str.substr(pos + 1);
    }
    else {
        parent = "";
        uuid = str;
    }
    return true;
}

std::string IdValue::serialize() const {
    if (id == 0) {
        return "";
    }
    return std::to_string(id);
}

bool IdValue::unserialize(const std::string& str) {
    if (str.empty()) {
        return false;
    }

    /*! @todo util for stoul, etc.. with (range) checking */
    id = std::stoul(str);
    return true;
}

std::string ResourceLastKey::serialize() const {
    return resource + "." + parent + "." + ResourceLastKey::LAST;
}

bool ResourceLastKey::unserialize(const std::string& str) {
    size_t last = str.rfind(std::string(".") + ResourceLastKey::LAST);
    if (std::string::npos == last) {
        return false;
    }
    size_t uuid = str.rfind('.', last - 1);
    if (std::string::npos == uuid) {
        return false;
    }
    /* check if data related the parent */
    if (parent != str.substr(uuid + 1, last - uuid - 1)) {
        return false;
    }
    resource = str.substr(0, uuid);
    return true;
}

std::string AlwaysMatchKey::serialize() const {
    assert(FAIL("Intended to be used only to iterate over all data"));
    throw 0;
}

bool AlwaysMatchKey::unserialize(const std::string&) { return true; }

const std::string ResourceLastKey::LAST{"last"};
