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
 * @file spdk_converter.hpp
 */
#pragma once



#include "agent-framework/module/storage_components.hpp"

#include "spdk/model/listen_address.hpp"
#include "spdk/model/host.hpp"
#include "spdk/model/namespace.hpp"



namespace agent {
namespace spdk {
namespace utils {
class SpdkConverter {

public:
    static const constexpr char RSD_SERIAL_NUMBER[] = "RSD:0000:00:01.0";


    /*!
     * @brief Build alias from LVS name and LVOL name.
     * @param lvs_name Name of Logical Volume Store
     * @param lvol_bdev Name of Logical Volume
     * @return Volume's alias in format "LvsName/LvolName"
     */
    static std::string make_alias(const std::string& lvs_name, const std::string& lvol_bdev);


    /*!
     * @brief Converts Volume to NVMe over Fabrics Namespace
     * @param volume Volume
     * @return Namespace
     */
    static ::spdk::model::Namespace convert_volume_to_namespace(const agent_framework::model::Volume& volume);


    /*!
     * @brief Converts Endpoint to NVMe over Fabrics Host
     * @param endpoint Endpoint
     * @return Host
     */
    static ::spdk::model::Host convert_initiator_to_host(const agent_framework::model::Endpoint& endpoint);


    /*!
     * @brief Converts IpTransportDetail to NVMe over Fabrics ListenAddress
     * @param ip_transport_detail IpTransportDetail
     * @return ListenAddress
     */
    static ::spdk::model::ListenAddress convert_to_listen_address(
        const agent_framework::model::attribute::IpTransportDetail& ip_transport_detail);

};
}
}
}


