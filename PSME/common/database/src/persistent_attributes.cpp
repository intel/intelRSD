/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file config/persistent_attributes.cpp
 */

#include "database/persistent_attributes.hpp"

using namespace database;

namespace {

/*! @brief Keep resource identification as pair of parent UUID and resource UUID. */
class UuidAttrKey final : public database::Serializable {
public:
    /*!
     * @brief Empty key, to be used during iterations
     */
    UuidAttrKey();


    /*!
     * @brief Key for attribute for given asset
     * @param _attr Attribute name
     * @param _uuid Asset UUID
     */
    UuidAttrKey(const std::string& _uuid, const std::string& _attr);


    /*! @brief Get stored UUID value */
    const std::string& get_uuid() const { return uuid; }


    /*! @brief Attribute name */
    const std::string& get_attr() const { return attr; }


private:
    std::string uuid{};
    std::string attr{};


    std::string serialize() const override;


    bool unserialize(const std::string& str) override;
};


UuidAttrKey::UuidAttrKey() {}


UuidAttrKey::UuidAttrKey(const std::string& _uuid, const std::string& _attr) :
    attr(_attr), uuid(_uuid) {}


std::string UuidAttrKey::serialize() const {
    return uuid + "." + attr;
}


bool UuidAttrKey::unserialize(const std::string& str) {
    size_t dot = str.rfind('.');
    if (dot == std::string::npos) {
        return false;
    }
    uuid = str.substr(0, dot);
    attr = str.substr(dot + 1);
    return true;
}


class StringValue final : public database::Serializable {
public:
    StringValue();


    StringValue(const std::string& _value);


    std::string get_value() const {
        return value;
    }


private:
    std::string value{};


    std::string serialize() const override;


    bool unserialize(const std::string& str) override;
};


StringValue::StringValue() {}


StringValue::StringValue(const std::string& _value) :
    value(_value) {}


std::string StringValue::serialize() const { return value; }


bool StringValue::unserialize(const std::string& str) {
    value = str;
    return true;
}

}


PersistentAttributes& PersistentAttributes::get_instance() {
    static PersistentAttributes instance{};
    return instance;
}

PersistentAttributes::PersistentAttributes() : db(database::Database::create("attr")) {
    /* read all the data from persistence */
    db->start();
    ::UuidAttrKey uuid_attr{};
    ::StringValue value{};
    while (db->next(uuid_attr, value)) {
        settings[UuidAttrPair{uuid_attr.get_uuid(), uuid_attr.get_attr()}] = value.get_value();
    }
    db->end();
}

PersistentAttributes::~PersistentAttributes() {
    db->remove();
    db.reset();
}

bool PersistentAttributes::get_value(const std::string& uuid, const std::string& attr,
                                                    std::string& ret_value) {
    std::lock_guard<std::mutex> lock{mutex};

    const UuidAttrValueMap::const_iterator it = settings.find({uuid, attr});
    if (it == settings.end()) {
        return false;
    } else {
        ret_value = it->second;
        return true;
    }
}

void PersistentAttributes::remove_value(const std::string& uuid, const std::string& attr) {
    std::lock_guard<std::mutex> lock{mutex};

    db->remove(::UuidAttrKey{uuid, attr});
    settings.erase(UuidAttrPair{uuid, attr});
}


void PersistentAttributes::set_value(const std::string& uuid, const std::string& attr, const std::string& value) {
    std::lock_guard<std::mutex> lock{mutex};

    db->put(::UuidAttrKey{uuid, attr}, ::StringValue{value});
    settings[UuidAttrPair{uuid, attr}] = value;
}

PersistentAttributes::UuidAttrVector PersistentAttributes::get_persistent_settings() {
    std::lock_guard<std::mutex> lock{mutex};

    UuidAttrVector ret{};
    UuidAttrValueMap::const_iterator it;
    for (it = settings.begin(); it != settings.end(); it++) {
        ret.push_back(it->first);
    }
    return ret;
}
