/*!
 * @brief General database interface
 *
 * @header{License}
 * @copyright Copyright (c) 2015-2017 Intel Corporation
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
 * @header{Filesystem}
 * @file database.cpp
 */

#include "psme/rest/model/handlers/file_database.hpp"

#include <logger/logger.hpp>
#include <logger/logger_factory.hpp>

#include <cassert>
#include <cstdint>
#include <thread>

namespace psme {
namespace rest {
namespace model {
namespace handler {

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

/*! FAIL condition for assert. Raise assertion when given condition is met. */
constexpr bool FAIL(const char*, bool when = true) { return !when; }

const std::chrono::seconds Database::NEVER{INT64_MAX};

std::recursive_mutex Database::mutex{};

Database::Databases Database::databases{};

Database::~Database() { }

Database::Database(const std::string& _name) : name(_name) { }

const std::string& Database::get_name() const { return name; }

Database::SPtr Database::create(const std::string& name) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    /* check if database is already created */
    for (auto& db : databases) {
        if (db->get_name() == name) {
            log_error(GET_LOGGER("db"), "Database '" << name << "' already created");
            assert (FAIL("Database already created"));
            return db;
        }
    }
    /* create new one, everything started with '*' means "all the data" */
    const std::string n = (name.substr(0, 1) == "*") ? "" : name;
    SPtr added{new FileDatabase(n)};
    databases.push_back(added);
    return added;
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
    log_error(GET_LOGGER("db"), "Database " << get_name() << " already removed");
}

unsigned Database::invalidate_all(std::chrono::seconds interval) {
    SPtr db{Database::create("*retention")};
    AlwaysMatchKey key{};
    auto ret = db->cleanup(key, interval);
    db->remove();
    return ret;
}

unsigned Database::remove_outdated(std::chrono::seconds interval) {
    SPtr db{Database::create("*retention")};
    AlwaysMatchKey key{};
    unsigned ret = db->wipe_outdated(key, interval);
    db->remove();
    return ret;
}

Serializable::Serializable() { }

Serializable::~Serializable() { }

std::string AlwaysMatchKey::serialize() const {
    assert (FAIL("Intended to be used only to iterate over all data"));
    throw 0;
}

bool AlwaysMatchKey::unserialize(const std::string&) { return true; }

const std::string ResourceLastKey::LAST{"last"};


} // @i{handler}
} // @i{model}
} // @i{rest}
} // @i{psme}
