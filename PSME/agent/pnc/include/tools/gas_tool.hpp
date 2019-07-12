/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * @file gas_tool.hpp
 *
 * @brief GasTool class used for gas operations - bind/unbind operation
 * various mrpc calls etc.
 * */

#pragma once



#include "agent-framework/module/utils/optional_field.hpp"
#include "agent-framework/module/utils/uuid.hpp"
#include "gas/global_address_space_registers.hpp"
#include "gas/mrpc/port_binding_info.hpp"
#include "gas/mrpc/partition_binding_info.hpp"
#include "gas/mrpc/bind_port.hpp"
#include "gas/mrpc/unbind_port.hpp"
#include "gas/mrpc/link_status_retrieve.hpp"



namespace agent {
namespace pnc {
namespace tools {

class ModelTool;
using ModelToolPtr = std::shared_ptr<ModelTool>;

/*! GasTool class */
class GasTool {
public:

    /*! Default constructor */
    GasTool() {}


    /*! Enable copy */
    GasTool(const GasTool&) {}


    GasTool& operator=(const GasTool&) { return *this; }


    /*! Default destructor */
    virtual ~GasTool();


    /*!
     * @brief Reads configuration space registers for an upstream port in the partition
     * @param[in] partition_id Id of the partition
     * @param[in,out] gas GAS registers to be updated
     * @return True if successful
     * */
    virtual bool read_csr_for_usp_port(std::uint8_t partition_id, gas::GlobalAddressSpaceRegisters& gas) const;


    /*!
     * @brief Performs get port binding info command
     * @param[in] gas Global Address Space Registers reference
     * @param[in] port_id physical id of the queried port
     * @return Port binding info result
     * */
    virtual gas::mrpc::PortBindingInfo get_port_binding_info(const gas::GlobalAddressSpaceRegisters& gas,
                                                             uint8_t port_id) const;


    /*!
     * @brief Performs get port binding info command
     * @param[in] gas Global Address Space Registers reference
     * @return Port binding info result
     * */
    virtual gas::mrpc::PortBindingInfo get_all_port_binding_info(const gas::GlobalAddressSpaceRegisters& gas) const {
        return get_port_binding_info(gas, 0xFF);
    }


    /*!
     * @brief Get partition binding info
     *
     * @param gas Global Address Space Registers reference
     * @param partition_id id of the partition/zone on the pcie switch
     * @return Partition binding info
     * */
    virtual gas::mrpc::PartitionBindingInfo get_partition_binding_info(
        const gas::GlobalAddressSpaceRegisters& gas, uint8_t partition_id) const;


    /*!
     * @brief Retrieves physical port id based on the logical bridge number
     * @param[in] gas Global Address Space Registers reference
     * @param[in] bridge_num Bridge num
     * @return Physical port Id
     * */
    virtual std::uint8_t bridge_num_to_phys_port_id(const gas::GlobalAddressSpaceRegisters& gas,
                                                    const std::uint8_t bridge_num) const;


    /*!
     * @brief Gets link status for a given port_uuid
     * @param[in] gas GAS registers instance
     * @param[in] phys_port_id Physical port id of the port
     * */
    virtual gas::mrpc::LinkStatusRetrieve get_link_status(const gas::GlobalAddressSpaceRegisters& gas,
                                                          std::uint8_t phys_port_id) const;


    /*!
     * @brief Gets logical bridge number (as on switch) from read PortBindingInfo
     * @param[in] pbi Result of the PortBindingInfo called on a queried port
     * @return Logical bridge of the port
     * */
    virtual uint8_t get_logical_bridge_for_port(const gas::mrpc::PortBindingInfo& pbi) const;


    /*!
     * @brief Unbind device driver from the management host
     * @param[in] switch_bridge_path Path to the Pcie Switch bridge bus-device-function representing the logical bridge
     * @param[in] bridge_id Id of parent sub-bridge of the device to have driver unbound
     * */
    virtual void unbind_management_host_driver(const std::string& switch_bridge_path, uint8_t bridge_id) const;


    /*!
     * @brief Unbinds device from the zone
     * @param[in] gas Instance of GAS registers
     * @param[in] partition_id Id of the zone from which the device is to be unbound
     * @param[in] bridge_id Id the bridge of the device to be unbound
     * */
    virtual void unbind_from_partition(const gas::GlobalAddressSpaceRegisters& gas, uint8_t partition_id,
                                       uint8_t bridge_id) const;


    /*!
     * @brief Binds physical port to a partition using a specified logical bridge
     * @param[in] gas Instance of GAS registers
     * @param[in] phys_port_id Physical Id of the port to be bound
     * @param[in] partition_id Id of the zone
     * @param[in] bridge_id Id of the logical bridge to be used
     * */
    virtual void bind_to_partition(const gas::GlobalAddressSpaceRegisters& gas, uint8_t phys_port_id,
                                   uint8_t partition_id, uint8_t bridge_id) const;


    /*!
     * @brief Gets id of a non used bridge
     * @param[in] pbi Read PartitionBindingInfo of the queried zone
     * @return Id of an empty bridge
     * */
    virtual uint8_t get_available_bridge_id(const gas::mrpc::PartitionBindingInfo& pbi) const;


    /*!
     * @brief Gets number of free bridges in the zone
     * @param[in] pbi Read PartitionBindingInfo of the queried zone
     * @return Number of available bridges
     * */
    virtual uint8_t get_available_bridges_count(const gas::mrpc::PartitionBindingInfo& pbi) const;


    /*!
     * @brief Gets presence bitmask for devices on ports
     * @param[in] gas GAS registers instance
     * @return Read bitmask
     * */
    virtual uint64_t get_presence_bitmask(const gas::GlobalAddressSpaceRegisters& gas) const;


    /*!
     * @brief Binds endpoint to the zone
     * @param[in] gas GAS registers instance
     * @param[in] zone_uuid Uuid of the zone
     * @param[in] endpoint_uuid Uuid of the endpoint to be bound
     * */
    virtual void bind_endpoint_to_zone(const gas::GlobalAddressSpaceRegisters& gas, const std::string& zone_uuid,
                                       const std::string& endpoint_uuid) const;


    /*!
    * @brief Unbinds endpoint to the zone
    * @param[in] gas GAS registers instance
    * @param[in] zone_uuid Uuid of the zone
    * @param[in] endpoint_uuid Uuid of the endpoint to be unbound
     * */
    virtual void unbind_endpoint_from_zone(const gas::GlobalAddressSpaceRegisters& gas, const std::string& zone_uuid,
                                           const std::string& endpoint_uuid) const;


    /*!
     * @brief Binds device to the managament partition
     * @param[in] mtool Model tool used to perform some operations
     * @param[in] gas GAS registers instance
     * @param[in] device_uuid Uuid of the device to be bound
     * */
    virtual void bind_device_to_mgmt_partition(ModelToolPtr mtool, gas::GlobalAddressSpaceRegisters& gas,
                                               const Uuid& device_uuid) const;


    /*!
     * @brief Unbinds device from the managament partition
     * @param[in] mtool Model tool used to perform some operations
     * @param[in] gas GAS registers instance
     * @param[in] device_uuid Uuid of the device to be bound
     * */
    virtual void unbind_device_from_mgmt_partition(ModelToolPtr mtool, gas::GlobalAddressSpaceRegisters& gas,
                                                   const Uuid& device_uuid) const;


    /*!
     * @brief Checks and logs whether a port was successfully unbound from a partition
     * @param[in] gas GAS registers instance
     * @param[in] phys_port_id physical id of the queried port
     * */
    virtual void check_port_unbinding_result(const gas::GlobalAddressSpaceRegisters& gas, uint8_t phys_port_id) const;


    /*!
     * @brief Checks and logs whether a port was successfully bound to a partition
     * @param[in] gas GAS registers instance
     * @param[in] partition_id Id of the zone
     * @param[in] bridge_id Id of the logical bridge to be used
     * */
    virtual void check_port_binding_result(const gas::GlobalAddressSpaceRegisters& gas, uint8_t partition_id,
                                           uint8_t bridge_id) const;

    /*!
     * @brief Gets Partition ID for specified zone. Zone has to contain one Initiator Endpoint to return Partition ID.
     * @param gas[in] GAS registers instance
     * @param zone_uuid[in] UUID of Zone to get Partition ID for
     * @return Partition ID associated with Zone if successful, empty OptionalField if fails (eg. if there is no Initiator Endpoint in the Zone).
     */
    virtual OptionalField<std::uint8_t> get_partition_id_by_zone(const agent::pnc::gas::GlobalAddressSpaceRegisters& gas, const Uuid& zone_uuid) const;


    /*!
     * @brief Gets Global Address Space Registers
     * @param switch_uuid Uuid of the PCI switch
     * @return Global Address Space Registers object for the PCI switch
     */
    virtual gas::GlobalAddressSpaceRegisters get_gas(const Uuid& switch_uuid);
};

using GasToolPtr = std::shared_ptr<GasTool>;

}
}
}
