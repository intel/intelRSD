/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file lvol_store.cpp
 */
#include "json-wrapper/json-wrapper.hpp"
#include "spdk/model/lvol_store.hpp"



using namespace spdk;

constexpr const char model::LvolStore::UUID[];
constexpr const char model::LvolStore::BASE_BDEV[];
constexpr const char model::LvolStore::FREE_CLUSTERS[];
constexpr const char model::LvolStore::CLUSTER_SIZE[];
constexpr const char model::LvolStore::TOTAL_DATA_CLUSTERS[];
constexpr const char model::LvolStore::LVOL_STORE_BLOCK_SIZE[];
constexpr const char model::LvolStore::NAME[];


void model::from_json(const json::Json& json, spdk::model::LvolStore& lvol_store) {

    lvol_store.set_uuid(json.at(model::LvolStore::UUID).get<std::string>());
    lvol_store.set_base_bdev(json.at(model::LvolStore::BASE_BDEV).get<std::string>());
    lvol_store.set_free_cluster(json.at(model::LvolStore::FREE_CLUSTERS).get<std::uint64_t>());
    lvol_store.set_cluster_size(json.at(model::LvolStore::CLUSTER_SIZE).get<std::uint64_t>());
    lvol_store.set_total_data_clusters(json.at(model::LvolStore::TOTAL_DATA_CLUSTERS).get<std::uint64_t>());
    lvol_store.set_block_size(json.at(model::LvolStore::LVOL_STORE_BLOCK_SIZE).get<std::uint32_t>());
    lvol_store.set_name(json.at(model::LvolStore::NAME).get<std::string>());
}


void model::to_json(json::Json& json, const spdk::model::LvolStore& lvol_store) {

    json = json::Json{
        {model::LvolStore::UUID,                  lvol_store.get_uuid()},
        {model::LvolStore::BASE_BDEV,             lvol_store.get_base_bdev()},
        {model::LvolStore::FREE_CLUSTERS,         lvol_store.get_free_clusters()},
        {model::LvolStore::CLUSTER_SIZE,          lvol_store.get_cluster_size()},
        {model::LvolStore::TOTAL_DATA_CLUSTERS,   lvol_store.get_total_data_clusters()},
        {model::LvolStore::LVOL_STORE_BLOCK_SIZE, lvol_store.get_block_size()},
        {model::LvolStore::NAME,                  lvol_store.get_name()}
    };
}


std::string model::LvolStore::to_string() const {

    std::stringstream ss{};
    ss << "=========LVOL Store=========" << std::endl;
    ss << "UUID: " << get_uuid() << std::endl;
    ss << "Base bdev: " << get_base_bdev() << std::endl;
    ss << "Free clusters: " << get_free_clusters() << std::endl;
    ss << "Cluster size: " << get_cluster_size() << std::endl;
    ss << "Total data clusters: " << get_total_data_clusters() << std::endl;
    ss << "Block size: " << get_block_size() << std::endl;
    ss << "Name: " << get_name() << std::endl;
    ss << "============================" << std::endl;
    return ss.str();
}




