/*!
 * @brief Declaration of utils for FPGAoF agent.
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
 * @file utils.hpp
 */
#pragma once



#include "agent-framework/module/common_components.hpp"



namespace agent {
namespace fpgaof {
namespace utils {

/*!
 * @brief Divide given input endpoints for target endpoints and initiator endpoints.
 * @param input_endpoints UUIDs of endpoints to be partitioned
 * @param target_endpoints UUIDs of target endpoints
 * @param initiator_endpoints UUIDs of initiator endpoints
 */
void partition_target_and_initiator_endpoints(const std::vector<Uuid>& input_endpoints,
                                              std::vector<Uuid>& target_endpoints,
                                              std::vector<Uuid>& initiator_endpoints);


/*!
 * @brief Sets processor status
 * @param processor_uuid uuid of the processor which status has to be set
 * @param status new value of processor status
 */
void set_processor_status(const std::string& processor_uuid,
                          const agent_framework::model::attribute::Status& status);


/*!
 * @brief Updates processor erased flag
 * @param processor reference to processor object which erased flag has to be updated
 * @param erased new value of erased flag
 */
void set_processor_erased(agent_framework::model::Processor& processor, bool erased);


/*!
 * @brief Updates processor erased flag in model
 * @param processor_uuid UUID of the Processor object which erased flag has to be updated
 * @param erased new value of erased flag
 * @warning This function updates Processor which exists in model. If processor is not in model exception is thrown.
 */
void set_processor_erased_in_model(const Uuid& processor_uuid, bool erased);


/*!
 * @brief Updates processor erased flag in model and database
 * @param processor_uuid uuid of the processor which erased flag has to be updated
 * @param erased new value of erased flag
 * @warning This function updates Processor which exists in model. If processor is not in model exception is thrown.
 */
void update_processor_erased(const std::string& processor_uuid, bool erased);


/*!
 * @brief Sets processor is in warning state flag
 * @param processor_uuid uuid of the processor to set 'is in warning state' flag in
 * @param is_in_warning_state new value of 'is in warning state' flag
 */
void set_processor_is_in_warning_state(const std::string& processor_uuid, bool is_in_warning_state);


/*!
 * @brief Sets processor is being erased flag
 * @param processor_uuid uuid of the processor to set 'is being erased' flag in
 * @param is_being_erased new value of 'is being erased' flag
 */
void set_processor_is_being_erased(const std::string& processor_uuid, bool is_being_erased);


/*!
 * @brief Synchronize discovered processors with processors database
 * @param system_uuid uuid of the System resource
 * @param processors collection of discovered Processor resources to be synchronized with database
 */
void
sync_processors_with_db(const Uuid& system_uuid, std::vector<agent_framework::model::Processor>& processors);


/*!
 * @brief Get ethernet interface UUID from IP address
 * @param ipv4_address IPv4 address of the interface
 * @return UUID of the interface with given IP address
 */
OptionalField<Uuid>
get_ethernet_interface_uuid_from_ip_address(const agent_framework::model::attribute::Ipv4Address& ipv4_address);


/*!
 * @brief Checks if RDMA is supported on interface exposing specified IP address
 * @param ipv4_address IP address exposed by interface to check
 * @return True if the interface supports RDMA, false otherwise
 */
bool is_rdma_supported_for_address(const std::string& ipv4_address);

}
}
}
