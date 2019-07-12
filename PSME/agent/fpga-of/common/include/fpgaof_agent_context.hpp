/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file fpgaof_agent_context.hpp
 * @brief Declaraction of PSME FPGA-oF agent context
 */

#pragma once



#include "sysfs/abstract_sysfs_interface.hpp"
#include "interface-reader/base_interface_reader.hpp"
#include "loader/fpgaof_loader.hpp"
#include "opaepp/opae-proxy/opae_proxy_context.hpp"
#include "opaepp/opae-proxy/opae_proxy_device_reader.hpp"



namespace agent {
namespace fpgaof {

/*!
 * Contains all interfaces that are required by the FPGA-oF agent discovery.
 * All interfaces will be used from multiple threads so they have to be thread safe.
 * If this is not true for an interface, thread-safe proxy of the interface has to be provided.
 */
class FpgaofAgentContext final {
public:
    using SPtr = std::shared_ptr<FpgaofAgentContext>;


    /*!
     * @brief Default constructor.
     */
    FpgaofAgentContext();

    /*!
     * Interface to sysfs to read system GUID
     */
    std::shared_ptr<sysfs::AbstractSysfsInterface> sysfs_interface{};

    /*!
     * Running FPGA-oF configuration.
     */
    std::shared_ptr<loader::FpgaofConfiguration> configuration{};

    /*!
     * Opaepp wrapper context of opae proxy
     */
    std::shared_ptr<opaepp::OpaeProxyContext> opae_proxy_context{};

    /*!
     * Opaepp opae proxy device reader for FPGAoF discovery
     */
    std::shared_ptr<opaepp::OpaeProxyDeviceReader> opae_proxy_device_reader{};
};

/*!
 * Declare context class used in the following declarations.
 * Change this declaration to change the context class used in commands.
 * */
using AgentContext = FpgaofAgentContext;

}
}
