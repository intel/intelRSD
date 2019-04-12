/*!
 * @brief Implementation of NvmeDatabase class.
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
 * @file nvme_database.cpp
 */

#include "tools/nvme_database.hpp"

using namespace agent::nvme::tools;
using namespace database;
using namespace std;

const string NvmeDatabase::ZONES = "zones";
const string NvmeDatabase::ENDPOINTS = "endpoints";
const string NvmeDatabase::VOLUME_ERASED = "erased";
const string NvmeDatabase::CHASSIS_ASSET_TAG = "assetTag";
const string NvmeDatabase::ROLE = "role";
const string NvmeDatabase::IPV4 = "ipv4";
const string NvmeDatabase::RDMA_PORT = "rdma_port";
const string NvmeDatabase::NQN = "nqn";

NvmeDatabase::NvmeDatabase(const Uuid& uuid, const string& name) : m_db{PersistentAttributes::get_instance(), uuid, name} {}

NvmeDatabase::~NvmeDatabase() {
    m_db.dump();
}

void NvmeDatabase::put(const string& key, const string& value) {
    m_db.put(key, value);
}

string NvmeDatabase::get(const string& key) const {
    return m_db.get(key);
}

void NvmeDatabase::del(const string& key) {
    m_db.del(key);
}

void NvmeDatabase::append(const string& bucket, const string& value) {
    m_db.append(bucket, value);
}

vector<string> NvmeDatabase::get_multiple_values(const string& bucket) const {
    return m_db.get_multiple_values(bucket);
}

void NvmeDatabase::remove() {
    m_db.remove();
}
