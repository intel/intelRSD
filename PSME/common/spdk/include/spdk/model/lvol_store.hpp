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
 * @file lvol_store.hpp
 */

#pragma once



#include "json-wrapper/json-wrapper.hpp"



namespace spdk {
namespace model {

/*!
 * @brief Implementation of LvolStore model based on SPDK
 */

class LvolStore {
public:

    static constexpr const char UUID[] = "uuid";
    static constexpr const char BASE_BDEV[] = "base_bdev";
    static constexpr const char FREE_CLUSTERS[] = "free_clusters";
    static constexpr const char CLUSTER_SIZE[] = "cluster_size";
    static constexpr const char TOTAL_DATA_CLUSTERS[] = "total_data_clusters";
    static constexpr const char LVOL_STORE_BLOCK_SIZE[] = "block_size";
    static constexpr const char NAME[] = "name";


    /*!
     * @brief Sets LvolStores UUID
     * @param uuid unique LvosStore id
     */
    void set_uuid(const std::string& uuid) {
        m_uuid = uuid;
    }


    /*!
     * @brief Get a LvolStore's UUID.
     * @return LvolStore's UUID.
     */
    const std::string& get_uuid() const {
        return m_uuid;
    }


    /*!
     * @brief Sets base Bdev
     * @param base_bdev name of a base Bdev
     */
    void set_base_bdev(const std::string& base_bdev) {
        m_base_bdev = base_bdev;
    }


    /*!
     * @brief Gets base Bdev
     * @return base Bdev
     */
    const std::string& get_base_bdev() const {
        return m_base_bdev;
    }


    /*!
     * @brief Sets number of LvolStore's free clusters
     * @param free_clusters number of LvolStore's free clusters
     */
    void set_free_cluster(uint64_t free_clusters) {
        m_free_clusters = free_clusters;
    }


    /*!
     * @brief Get LvolStore's free clusters
     * @return LvolStore's free clusters
     */
    uint64_t get_free_clusters() const {
        return m_free_clusters;
    }


    /*!
     * @brief Get LvolStore's cluster size
     * @return LvolStore's cluster size
     */
    uint64_t get_cluster_size() const {
        return m_cluster_size;
    }


    /*!
     * @brief Sets LvolStore's cluster size
     * @param cluster_size size of a cluster
     */
    void set_cluster_size(uint64_t cluster_size) {
        m_cluster_size = cluster_size;
    }


    /*!
     * @brief Get LvolStores total data cluster
     * @return LvolStores total data cluster
     */
    uint64_t get_total_data_clusters() const {
        return m_total_data_clusters;
    }


    /*!
     * @brief Sets LvolStore's total data clusters
     * @param total_data_clusters number of data clusters
     */
    void set_total_data_clusters(std::uint64_t total_data_clusters) {
        m_total_data_clusters = total_data_clusters;
    }


    /*!
     * @brief Get LvolStores block size
     * @return LvolStores block size
     */
    uint32_t get_block_size() const {
        return m_block_size;
    }


    /*!
     * @brief Sets LvolStore's block size
     * @param block_size size of a LvolStore's block
     */
    void set_block_size(uint32_t block_size) {
        m_block_size = block_size;
    }


    /*!
     * @brief Get LvolStores name
     * @return LvolStores name
     */
    const std::string& get_name() const {
        return m_name;
    }


    /*!
     * @brief Get LvolStores name
     * @param LvolStore's name
     */
    void set_name(const std::string& name) {
        m_name = name;
    }


    /*!
     * @brief Lists current info about LvolStore
     * @return info about LvolStore as a string
     */
    std::string to_string() const;


private:
    std::string m_uuid{};
    std::string m_base_bdev{};
    std::uint64_t m_free_clusters{};
    std::uint64_t m_cluster_size{};
    std::uint64_t m_total_data_clusters{};
    std::uint32_t m_block_size{};
    std::string m_name{};

};


/*!
 * @brief converts from JSON to LvolStore
 * @param json Json object
 * @param lvol_store LvolStore object
 */
void from_json(const json::Json& json, LvolStore& lvol_store);


/*!
 * @brief converts from LvolStore to Json
 * @param json Json object
 * @param lvol_store LvolStore object
 */
void to_json(json::Json& json, const LvolStore& lvol_store);

}
}

