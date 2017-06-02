"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
from include.psme_xml_structure.managers.empty_manager import EmptyManager


class ManagersTypes:
    BMC_MANAGER = "bmc"
    CPU_ID_MANAGER = "cpu_id_manager"
    DRIVE_MANAGER = "drive_manager"
    FRU_INFO_MANAGER = "fru_info_manager"
    IPV4_ADDRESS_MANAGER = "ipv4_address manager"
    IPV6_ADDRESS_MANAGER = "ipv6_address manager"
    MEMORY_MANAGER = "memory_manager"
    NEIGHBOR_INFO_MANAGER = "neighbor_info_manager"
    NETWORK_INTERFACE_MANAGER = "network_interface_manager"
    NETWORK_SERVICE_MANAGER = "network_service_manager"
    PROCESSOR_MANAGER = "processor_manager"
    PSME_MANAGER = "psme_manager"
    COMPUTE_SYSTEM_MANAGER = "compute_system_manager"
    MANAGER_DRAWER_MANAGER = "manager_drawer_manager"
    MANAGER_BLADE_MANAGER = "manager_blade_manager"
    SERIAL_CONSOLE_MANAGER = "serial_console_manager"
    STATUS_MANAGER = "status_manager"
    STORAGE_CONTROLLER_MANAGER = "storage_controller_manager"
    USB_DEVICE_MANAGER = "usb_device_manager"
    PCI_DEVICE_MANAGER = "pci_device_manager"


class ManagersList:
    MANAGERS = {}
    @staticmethod
    def add_manager(manager_type, manager):
        ManagersList.MANAGERS[manager_type] = manager

    @staticmethod
    def get_manager(manager_type):
        try:
            return ManagersList.MANAGERS[manager_type]
        except KeyError:
            print "Not suitable manager was found, using empty one"
            return EmptyManager

    @staticmethod
    def clear_managers_list():
        ManagersList.MANAGERS = {}
