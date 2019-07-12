/*!
 * @brief Implementation of FpgaDiscoveryDatabase class.
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file fpga_database.cpp
 */

#include "tools/fpga_discovery_database.hpp"

using namespace agent::fpga_discovery::tools;
using namespace database;
using namespace std;

const string FpgaDiscoveryDatabase::ZONES = "zones";
const string FpgaDiscoveryDatabase::ENDPOINTS = "endpoints";
const string FpgaDiscoveryDatabase::CHASSIS_ASSET_TAG = "assetTag";
const string FpgaDiscoveryDatabase::ROLE = "role";
const string FpgaDiscoveryDatabase::IPV4 = "ipv4";
const string FpgaDiscoveryDatabase::PORT = "port";
const string FpgaDiscoveryDatabase::UUID = "uuid";
const string FpgaDiscoveryDatabase::TRANSPORT_PROTOCOL = "transportProtocol";

FpgaDiscoveryDatabase::FpgaDiscoveryDatabase(const Uuid& uuid, const string& name) : m_db{PersistentAttributes::get_instance(), uuid, name} {}

FpgaDiscoveryDatabase::~FpgaDiscoveryDatabase() {
    m_db.dump();
}

void FpgaDiscoveryDatabase::put(const string& key, const string& value) {
    m_db.put(key, value);
}

string FpgaDiscoveryDatabase::get(const string& key) const {
    return m_db.get(key);
}

void FpgaDiscoveryDatabase::del(const string& key) {
    m_db.del(key);
}

void FpgaDiscoveryDatabase::append(const string& bucket, const string& value) {
    m_db.append(bucket, value);
}

vector<string> FpgaDiscoveryDatabase::get_multiple_values(const string& bucket) const {
    return m_db.get_multiple_values(bucket);
}

void FpgaDiscoveryDatabase::remove() {
    m_db.remove();
}
