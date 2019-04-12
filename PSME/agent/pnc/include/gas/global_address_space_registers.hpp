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
 * @file global_address_space_registers.hpp
 * @brief Global Address Space Registers
 * */

#pragma once



#include "mrpc/command.hpp"
#include "top/top_level_registers.hpp"
#include "partition/partition_configuration.hpp"
#include "csr/configuration_space_register.hpp"
#include "access_interface_factory.hpp"

#include <memory>
#include <mutex>

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace gas {

/*! Global Address Space Registers */
class GlobalAddressSpaceRegisters {

/*! Required delay for polling mrpc commands status */
    static constexpr std::uint16_t MRPC_DELAY_MS = 10;

    static std::mutex m_mrpc_mutex;
    static std::mutex m_top_mutex;
    static std::mutex m_partition_mutex;
    static std::mutex m_csr_mutex;
public:
    /*! Default constructor */
    GlobalAddressSpaceRegisters() {}


    /*! Constructor initializing interface */
    GlobalAddressSpaceRegisters(AccessInterface* iface) : m_iface(iface) {}


    /*! Copy constructor */
    GlobalAddressSpaceRegisters(const GlobalAddressSpaceRegisters&) = default;


    /*! Assignment operator */
    GlobalAddressSpaceRegisters& operator=(const GlobalAddressSpaceRegisters&) = default;


    virtual ~GlobalAddressSpaceRegisters();


    /*!
     * @brief Initializes GAS with an access interface
     * @param[in] iface Access Interface to be used
     */
    void init(AccessInterface* iface) { m_iface = iface; }


    /*!
     * @brief Returns AccessInterface used by GAS
     * @return AccessInterface
     */
    AccessInterface* get_interface() const {
        return m_iface;
    }


    /*!
     * @brief Gets default GAS instance using AccessInterfaceFactory
     * @param[in] path Path to the memory file
     * @return Constructed GAS object
     */
    static GlobalAddressSpaceRegisters get_default(const std::string& path);


    /*!
     * @brief Executes MRPC command
     * @param cmd MRPC Command
     * @return MRPC Command status
     * */
    mrpc::CommandStatus execute_cmd(mrpc::Command& cmd) const;


    /*! Update TopLevelRegisters */
    void read_top();


    /*! Read configuration of the specific partition */
    void read_partition(std::uint8_t partition_id);


    /*! Read Configuration Space of the specific function */
    void read_csr(std::uint8_t instance_id);


    /*! TopLevelRegisters */
    top::TopLevelRegisters top{};

    /*! Partition Configuration */
    partition::PartitionConfiguration partition{};

    /*! Configuration Space Register */
    csr::ConfigurationSpaceRegister csr{};

private:

    AccessInterface* m_iface{nullptr};


    template<typename T>
    mrpc::CommandStatus read_register(T& reg) const;
};

}
}
}
