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
 * @file nvme_driver.cpp
 */


#include "json-wrapper/json-wrapper.hpp"
#include "spdk/model/drivers/nvme_driver.hpp"



using namespace spdk;

const char model::drivers::NvmeDriver::DRIVER_NAME[] = "nvme";

namespace {

constexpr const char TRID[] = "trid";
constexpr const char NS_DATA[] = "ns_data";
constexpr const char PCI_ADDRESS[] = "pci_address";
constexpr const char VERSION[] = "vs";
constexpr const char CTRLR_DATA[] = "ctrlr_data";
constexpr const char CSTS[] = "csts";

}


void model::drivers::to_json(json::Json& json, const NvmeDriver& nvme_driver) {
    json = json::Json{
        {::TRID,        nvme_driver.get_trid()},
        {::NS_DATA,     nvme_driver.get_ns_data()},
        {::PCI_ADDRESS, nvme_driver.get_pci_address()},
        {::VERSION,     nvme_driver.get_version()},
        {::CTRLR_DATA,  nvme_driver.get_ctrlr_data()},
        {::CSTS,        nvme_driver.get_csts()}
    };
}


void model::drivers::from_json(const json::Json& json, NvmeDriver& nvme_driver) {

    nvme_driver.set_trid(json.at(::TRID).get<TransportId>());
    nvme_driver.set_ns_data(json.at(::NS_DATA).get<NamespaceData>());
    nvme_driver.set_pci_address(json.at(::PCI_ADDRESS).get<std::string>());
    nvme_driver.set_version(json.at(::VERSION).get<Version>());
    nvme_driver.set_ctrlr_data(json.at(::CTRLR_DATA).get<ControllerData>());
    nvme_driver.set_csts(json.at(::CSTS).get<ControllerStatus>());

}


std::string model::drivers::NvmeDriver::to_string() const {

    std::stringstream ss{};

    ss << std::endl << " Transport ID: " << std::endl << get_csts().to_string() << std::endl;
    ss << std::endl << " Namespace Data: " << std::endl << get_ns_data().to_string() << std::endl;
    ss << std::endl << " PCI address: " << get_pci_address() << std::endl;
    ss << std::endl << " NVMe Version: " << std::endl << get_version().to_string() << std::endl;
    ss << std::endl << " Controller Data: " << std::endl << get_ctrlr_data().to_string() << std::endl;
    ss << std::endl << " Controller Status: " << std::endl << get_csts().to_string() << std::endl;

    return ss.str();
}





