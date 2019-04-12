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
 * @file nvme_driver.hpp
 */

#pragma once



#include <string>
#include "json-wrapper/json-wrapper.hpp"
#include "spdk/model/namespace_data.hpp"
#include "spdk/model/version.hpp"
#include "spdk/model/controller_status.hpp"
#include "spdk/model/transport_id.hpp"
#include "spdk/model/controller_data.hpp"



namespace spdk {
namespace model {
namespace drivers {
/*!
 * @brief Implementation of Nvme Driver for Bdev model
 */
class NvmeDriver {
public:

    /*!
     * @brief Name for Nvme Driver
     */
    static const char DRIVER_NAME[];


    /*!
     * @brief Lists current info about Nvme Driver
     * @return info about Nvme Driver as a string
     */
    std::string to_string() const;


    /*!
     * @brief Get Transport ID
     * @return Transport ID
     */
    const TransportId& get_trid() const {
        return m_trid;
    }


    /*!
     * @brief Sets Transport ID
     * @param trid new Transport ID
     */
    void set_trid(const TransportId& trid) {
        m_trid = trid;
    }


    /*!
     * @brief Get PCI address
     * @return PCI address
     */
    const std::string& get_pci_address() const {
        return m_pci_address;
    }


    /*!
     * @brief Sets PCI address
     * @param pci_address new PCI address
     */
    void set_pci_address(const std::string& pci_address) {
        m_pci_address = pci_address;
    }


    /*!
     * @brief Get Namespace Data
     * @return Namespace Data
     */
    const NamespaceData& get_ns_data() const {
        return m_ns_data;
    }


    /*!
     * @brief Sets Namespace Data
     * @param ns_data new Namespace Data
     */
    void set_ns_data(const NamespaceData& ns_data) {
        m_ns_data = ns_data;
    }


    /*!
     * @brief Get NVME version
     * @return NVME version
     */
    const Version& get_version() const {
        return m_version;
    }


    /*!
     * @brief Sets NVME version
     * @param version new NVME version
     */
    void set_version(const Version& version) {
        m_version = version;
    }


    /*!
     * @brief Get Controller Data
     * @return Controller Data
     */
    const ControllerData& get_ctrlr_data() const {
        return m_ctrlr_data;
    }


    /*!
     * @brief Sets Controller Data
     * @param ctrlr_data new Controller Data
     */
    void set_ctrlr_data(const ControllerData& ctrlr_data) {
        m_ctrlr_data = ctrlr_data;
    }


    /*!
     * @brief Get Controller Status
     * @return Controller Status
     */
    const ControllerStatus& get_csts() const {
        return m_csts;
    }


    /*!
     * @brief Sets Controller Status
     * @param csts new Controller Status
     */
    void set_csts(const ControllerStatus& csts) {
        m_csts = csts;
    }


private:
    TransportId m_trid{};
    std::string m_pci_address{};
    NamespaceData m_ns_data{};
    Version m_version{};
    ControllerStatus m_csts{};
    ControllerData m_ctrlr_data{};

};


/*!
 * @brief converts from Json to NvmeDriver
 * @param json Json object
 * @param nvme_drive NvmeDriver object
 */
void from_json(const json::Json& json, NvmeDriver& nvme_driver);


/*!
 * @brief converts from NvmeDriver to Json
 * @param json Json object
 * @param nvme_drive NvmeDriver object
 */
void to_json(json::Json& json, const NvmeDriver& nvme_driver);
}
}
}

