/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file spdk_api.hpp
 */

#pragma once



#include "spdk/model/bdev.hpp"
#include "spdk/model/lvol_store.hpp"
#include "spdk/model/nvmf_subsystem.hpp"
#include "spdk/model/namespace.hpp"

#include "json-wrapper/json-wrapper.hpp"
#include "json-rpc/connectors/abstract_client_connector.hpp"
#include "json-rpc/handlers/json_rpc_request_invoker.hpp"

#include "json_optional.hpp"

#include <string>
#include <vector>
#include <mutex>



namespace spdk {
class Method {
public:
    Method() = delete;


    static constexpr const char GET_RPC_METHODS[] = "get_rpc_methods";
    static constexpr const char GET_BDEVS[] = "get_bdevs";
    static constexpr const char GET_LVOL_STORES[] = "get_lvol_stores";
    static constexpr const char GET_NVMF_SUBSYSTEMS[] = "get_nvmf_subsystems";
    static constexpr const char GET_RAID_BDEVS[] = "get_raid_bdevs";

    static constexpr const char CONSTRUCT_LVOL_STORE[] = "construct_lvol_store";
    static constexpr const char CONSTRUCT_LVOL_BDEV[] = "construct_lvol_bdev";
    static constexpr const char CONSTRUCT_NVMF_SUBSYSTEM[] = "construct_nvmf_subsystem";
    static constexpr const char NVMF_SUBSYSTEM_CREATE[] = "nvmf_subsystem_create";
    static constexpr const char CONSTRUCT_RAID_BDEV[] = "construct_raid_bdev";

    static constexpr const char NVMF_SUBSYSTEM_REMOVE_HOST[] = "nvmf_subsystem_remove_host";
    static constexpr const char NVMF_SUBSYSTEM_ADD_HOST[] = "nvmf_subsystem_add_host";
    static constexpr const char NVMF_SUBSYSTEM_ADD_LISTENER[] = "nvmf_subsystem_add_listener";
    static constexpr const char NVMF_SUBSYSTEM_ADD_NS[] = "nvmf_subsystem_add_ns";

    static constexpr const char DESTROY_LVOL_STORE[] = "destroy_lvol_store";
    static constexpr const char DESTROY_LVOL_BDEV[] = "destroy_lvol_bdev";
    static constexpr const char DELETE_NVMF_SUBSYSTEM[] = "delete_nvmf_subsystem";
    static constexpr const char DESTROY_RAID_BDEV[] = "destroy_raid_bdev";

    static constexpr const char RESIZE_LVOL_BDEV[] = "resize_lvol_bdev";

    static constexpr const char SNAPSHOT_LVOL_BDEV[] = "snapshot_lvol_bdev";

    static constexpr const char CLONE_LVOL_BDEV[] = "clone_lvol_bdev";

    static constexpr const char INFLATE_LVOL_BDEV[] = "inflate_lvol_bdev";
};

constexpr const char DEFAULT_SERIAL_NUMBER[] = "0000:00:01.0";

/*!
 * @brief Implementation of SPDK JSON-RPC API client
 */
class SpdkApi {
public:

    /*!
     * @brief Constructs SpdkApi based on the given connector
     * @param connector Client connector
     */
    explicit SpdkApi(const json_rpc::AbstractClientConnectorPtr connector,
                     const std::shared_ptr<json_rpc::JsonRpcRequestInvoker> invoker);


    /*!
     * @brief Get all RPC methods
     * @return vector of RPC methods
     */
    virtual std::vector<std::string> get_rpc_methods();


    /*!
     * @brief Get information about block devices
     * @param name Optional block device name
     * @return vector of Bdevs
     */
    virtual std::vector<spdk::model::Bdev> get_bdevs(const JsonOptional<std::string>& name = {});


    /*!
     * @brief Get vector of all logical volume stores
     * @return vector of LvolStore
     */
    virtual std::vector<spdk::model::LvolStore> get_lvol_stores();


    /*!
     * @brief Get vector of logical volume stores
     * @param uuid UUID of the logical volume store to retrieve information about
     * @return vector of LvolStore
     */
    virtual std::vector<spdk::model::LvolStore> get_lvol_stores_by_uuid(const std::string& uuid);


    /*!
     * @brief Get vector of logical volume stores
     * @param name Name of the logical volume store to retrieve information about
     * @return vector of LvolStore
     */
    virtual std::vector<spdk::model::LvolStore> get_lvol_stores_by_name(const std::string& lvs_name);


    /*!
     * @brief Get information about NVMe over Fabrics subsystems
     * @return vector of NvmfSubsystem
     */
    virtual std::vector<spdk::model::NvmfSubsystem> get_nvmf_subsystems();


    /*!
     * @brief Constructs logical volume store
     * @param bdev_name Bdev on which to construct logical volume store
     * @param lvs_name Name of the logical volume store to create
     * @param cluster_size Optional cluster size in bytes
     * @return UUID of the created logical volume store is returned
     */
    virtual std::string construct_lvol_store(const std::string& bdev_name,
                                             const std::string& lvs_name,
                                             const JsonOptional<std::uint64_t>& cluster_size = {});


    /*!
     * @brief Destroys logical volume store. Either uuid or lvs_name must be specified, but not both
     * @param uuid UUID of the logical volume store to destroy
     * @param lvs_name name of the logical volume store to destroy
     * @return true if LvolStore have been destroyed, false otherwise
     */
    virtual bool destroy_lvol_store(const std::string& uuid = std::string{},
                                    const std::string& lvs_name = std::string{});


    /*!
     * @brief Create a logical volume on a logical volume store with its UUID
     * @param lvol_name Name of logical volume to create
     * @param size Desired size of logical volume in bytes
     * @param uuid UUID of logical volume store to create logical volume on
     * @param thin_provision True to enable thin provisioning
     * @param clear_method default data clusters clear method
     * @return UUID of the created logical volume is returned
     */
    virtual std::string construct_lvol_bdev_by_store_uuid(const std::string& lvol_name,
                                                          const std::uint64_t size,
                                                          const std::string& uuid,
                                                          const JsonOptional<bool>& thin_provision = {},
                                                          const JsonOptional<std::string>& clear_method = {});


    /*!
     * @brief Create a logical volume on a logical volume store with its name
     * @param lvol_name Name of logical volume to create
     * @param size Desired size of logical volume in bytes
     * @param lvs_name Name of logical volume store to create logical volume on
     * @param thin_provision True to enable thin provisioning
     * @param clear_method default data clusters clear method
     * @return UUID of the created logical volume is returned
     */
    virtual std::string construct_lvol_bdev_by_store_name(const std::string& lvol_name,
                                                          const std::uint64_t size,
                                                          const std::string& lvs_name,
                                                          const JsonOptional<bool>& thin_provision = {},
                                                          const JsonOptional<std::string>& clear_method = {});


    /*!
     * @brief Destroy a logical volume
     * @param name UUID or alias of the logical volume to destroy
     * @return true if lvol have been destroyed, false otherwise
     */
    virtual bool destroy_lvol_bdev(const std::string& name);


    /*!
     * @brief Construct an NVMe over Fabrics target subsystem.
     * @param nqn Subsystem NQN
     * @param serial_number Serial number of virtual controller
     * @param max_namespaces Maximum number of namespaces that can be attached to the subsystem
     * @param allow_any_host Allow any host (true) or enforce allowed host whitelist (false). Default: false.
     * @return true if NVMe over Fabric subsystem has been created, false otherwise
     */
    virtual bool nvmf_subsystem_create(const std::string& nqn,
                                       const JsonOptional<std::string>& serial_number = {},
                                       const JsonOptional<std::uint64_t>& max_namespaces = {},
                                       const JsonOptional<bool>& allow_any_host = {});


    /*!
     * @brief Add a namespace to a subsystem.
     * @param subsystem_nqn Subsystem NQN
     * @param ns SPDK Namespace object.
     * @return The namespace ID is returned as the result.
     */
    virtual std::uint32_t nvmf_subsystem_add_ns(const std::string& subsystem_nqn, const model::Namespace& ns);


    /*!
     * @brief Remove host from NVMe over Fabrics allowed hosts.
     * @param subsystem_nqn Subsystem NQN
     * @param host_nqn Host NQN
     * @return true if Host have been removed, false otherwise
     */
    virtual bool nvmf_subsystem_remove_host(const std::string& subsystem_nqn, const std::string& host_nqn);


    /*!
     * @brief Remove host from NVMe over Fabrics allowed hosts.
     * @param subsystem_nqn Subsystem NQN
     * @param host SPDK host model
     * @return true if Host have been removed, false otherwise
     */
    virtual bool nvmf_subsystem_remove_host(const std::string& subsystem_nqn, const model::Host& host);


    /*!
     * @brief Add host to NVMe over Fabrics allowed hosts.
     * @param subsystem_nqn Subsystem NQN
     * @param host_nqn Host NQN
     * @return Returns true if host has been added, false otherwise
     */
    virtual bool nvmf_subsystem_add_host(const std::string& subsystem_nqn, const std::string& host_nqn);


    /*!
     * @brief Add host to NVMe over Fabrics allowed hosts.
     * @param subsystem_nqn Subsystem NQN
     * @param host SPDK host model
     * @return Returns true if host has been added, false otherwise
     */
    virtual bool nvmf_subsystem_add_host(const std::string& subsystem_nqn, const model::Host& host);


    /*!
     * @brief Add a new listen address to an NVMe-oF subsystem.
     * @param subsystem_nqn Subsystem NQN.
     * @param listen_address SPDK listen address object.
     * @return Returns true if address has been added, false otherwise.
     */
    virtual bool nvmf_subsystem_add_listener(const std::string& subsystem_nqn,
                                             const model::ListenAddress& listen_address);


    /*!
     * @brief Delete an existing NVMe-oF subsystem
     * @param nqn Subsystem NQN to delete
     * @return true if NVMe-oF subsystem have been destroyed, false otherwise
     */
    virtual bool delete_nvmf_subsystem(const std::string& nqn);


    /*!
     * @brief Construct a RAID bdev.
     * @param raid_bdev_name Name for a RAID bdev
     * @param strip_size Strip size in KB, supported values should be a power of 2 (starting from 8)
     * @param base_bdevs List of base bdevs names from which RAID bdev will be constructed
     * @param raid_level RAID level, in SPDK currently only level 0 is supported
     * @return true if RAID bdev have been created, false otherwise
     */
    virtual bool construct_raid_bdev(const std::string& raid_bdev_name,
                                     const std::uint64_t strip_size,
                                     const std::vector<std::string>& base_bdevs,
                                     const std::uint8_t raid_level = 0);


    /*!
    * @brief Resize a logical volume.
    * @param lvol_uuid UUID or alias of the logical volume to resize.
    * @param size Desired size of the logical volume in bytes
    * @return True if success, false otherwise.
    */
    virtual bool resize_lvol_bdev(const std::string& lvol_uuid, const std::uint64_t size);


    /*!
     * @brief Destroy a RAID bdev.
     * @return true if RAID bdev have been destroyed, false otherwise
     */
    virtual bool destroy_raid_bdev(const std::string& raid_bdev_name);


    /*!
     * @brief Get list of RAID bdevs names
     * @param category RAID bdev category requested, available categories: all/online/configuring/offline
     * @return vector of RAID bdevs names
     */
    virtual std::vector<std::string> get_raid_bdevs_names(const std::string& category = model::RaidBdevCategory::ALL);


    /*!
     * @brief Capture a snapshot of the current state of a logical volume
     * @param lvol_name UUID or alias of the logical volume to create a snapshot from
     * @param snapshot_name Name for the newly created snapshot
     * @return UUID of the created logical volume snapshot is returned
     */
    virtual std::string snapshot_lvol_bdev(const std::string& lvol_name, const std::string& snapshot_name);


    /*!
     * @brief Create clone logical volume based on a snapshot.
     * @param snapshot_name UUID or alias of the snapshot to clone
     * @param clone_name Name for the logical volume to create
     * @return UUID of the created logical volume clone is returned
     */
    virtual std::string clone_lvol_bdev(const std::string& snapshot_name, const std::string& clone_name);


    /*!
     * @brief Inflate a logical volume.
     * @param lvol_name UUID or alias of the logical volume to inflate
     * @return true if logical volume have been inflated, false otherwise
     */
    virtual bool inflate_lvol_bdev(const std::string& lvol_name);


protected:

    /*!
     * @brief Set ups JsonRpcRequestInvoker
     * @param method_name name of the calling method
     * @param parameters parameters needed to call the method
     * @return Returns JSON result from RPC invoker
     */
    const json::Json& send_request(const std::string& method_name, const json::Json& parameters = json::Json{});


    std::mutex m_mutex{};
    json_rpc::AbstractClientConnectorPtr m_connector{};
    std::shared_ptr<json_rpc::JsonRpcRequestInvoker> m_invoker{};

};
}
