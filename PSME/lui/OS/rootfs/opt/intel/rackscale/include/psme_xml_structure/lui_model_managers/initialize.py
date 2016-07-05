"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section DESCRIPTION
"""

from include.psme_xml_structure.managers.managers_list import ManagersList, ManagersTypes

from include.psme_xml_structure.lui_model_managers.bmc_manager import BmcManager
from include.psme_xml_structure.lui_model_managers.cpu_id_manager import CpuIdManager
from include.psme_xml_structure.lui_model_managers.drive_manager import DriveManager
from include.psme_xml_structure.lui_model_managers.fru_info_manager import FruInfoManager
from include.psme_xml_structure.lui_model_managers.ipv4_address_manager import Ipv4AddressManager
from include.psme_xml_structure.lui_model_managers.ipv6_address_manager import Ipv6AddressManager
from include.psme_xml_structure.lui_model_managers.dimm_manager import DimmManager
from include.psme_xml_structure.lui_model_managers.neighbor_info_manager import NeighborInfoManager
from include.psme_xml_structure.lui_model_managers.network_service_manager import NetworkServiceManager
from include.psme_xml_structure.lui_model_managers.network_interface_manager import NetworkInterfaceManager
from include.psme_xml_structure.lui_model_managers.processor_manager import ProcessorManager
from include.psme_xml_structure.lui_model_managers.psme_manager import PsmeManager
from include.psme_xml_structure.lui_model_managers.compute_system_manager import ComputeSystemManager
from include.psme_xml_structure.lui_model_managers.manager_drawer_manager import ManagerDrawerManager
from include.psme_xml_structure.lui_model_managers.manager_blade_manager import ManagerBladeManager
from include.psme_xml_structure.lui_model_managers.serial_console_manager import SerialConsoleManager
from include.psme_xml_structure.lui_model_managers.status_manager import StatusManager
from include.psme_xml_structure.lui_model_managers.storage_controller_manager import StorageControllerManager
from include.psme_xml_structure.lui_model_managers.usb_device_manager import UsbDeviceManager
from include.psme_xml_structure.lui_model_managers.pci_device_manager import PciDeviceManager


ManagersMap = {
    ManagersTypes.BMC_MANAGER: BmcManager,
    ManagersTypes.CPU_ID_MANAGER: CpuIdManager,
    ManagersTypes.DRIVE_MANAGER: DriveManager,
    ManagersTypes.FRU_INFO_MANAGER: FruInfoManager,
    ManagersTypes.IPV4_ADDRESS_MANAGER: Ipv4AddressManager,
    ManagersTypes.IPV6_ADDRESS_MANAGER: Ipv6AddressManager,
    ManagersTypes.DIMM_MANAGER: DimmManager,
    ManagersTypes.NEIGHBOR_INFO_MANAGER: NeighborInfoManager,
    ManagersTypes.NETWORK_INTERFACE_MANAGER: NetworkInterfaceManager,
    ManagersTypes.NETWORK_SERVICE_MANAGER: NetworkServiceManager,
    ManagersTypes.PROCESSOR_MANAGER: ProcessorManager,
    ManagersTypes.PSME_MANAGER: PsmeManager,
    ManagersTypes.COMPUTE_SYSTEM_MANAGER: ComputeSystemManager,
    ManagersTypes.MANAGER_DRAWER_MANAGER: ManagerDrawerManager,
    ManagersTypes.MANAGER_BLADE_MANAGER: ManagerBladeManager,
    ManagersTypes.SERIAL_CONSOLE_MANAGER: SerialConsoleManager,
    ManagersTypes.STATUS_MANAGER: StatusManager,
    ManagersTypes.STORAGE_CONTROLLER_MANAGER: StorageControllerManager,
    ManagersTypes.USB_DEVICE_MANAGER: UsbDeviceManager,
    ManagersTypes.PCI_DEVICE_MANAGER: PciDeviceManager
}


def initialize():
    ManagersList.clear_managers_list()
    for manager_type, manager in ManagersMap.iteritems():
        ManagersList.add_manager(manager_type=manager_type, manager=manager)
