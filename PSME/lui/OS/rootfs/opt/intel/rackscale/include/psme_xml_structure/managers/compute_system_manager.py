"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
from abc import abstractmethod

from include.psme_xml_structure.managers.manager import Manager
from include.psme_xml_structure.managers.managers_list import ManagersList, ManagersTypes

from include.psme_xml_structure.model.compute_system import ComputeSystem
from include.psme_xml_structure.model.storage_subsystem import StorageSubsystem
from include.common.globals import *


class ComputeSystemManager(Manager):
    @classmethod
    def get_data(cls, data, context=None):
        raw_data = cls.prepare_data(data)
        compute_system = ComputeSystem()

        try:
            data = raw_data[XML_LIST][XML_NODE][XML_NODE]
            if not isinstance(data, list):
                data = [data]
            for data_part in data:
                if data_part[XML_AT_ID] == LSHW_CORE:
                    data_node = data_part[XML_NODE]
                    break
        except KeyError as e:
            return compute_system

        fru_blade = data_part

        processor_manager = ManagersList.get_manager(ManagersTypes.PROCESSOR_MANAGER)
        memory_manager = ManagersList.get_manager(ManagersTypes.MEMORY_MANAGER)
        storage_controller_manager = ManagersList.get_manager(ManagersTypes.STORAGE_CONTROLLER_MANAGER)
        network_interface_manager = ManagersList.get_manager(ManagersTypes.NETWORK_INTERFACE_MANAGER)
        fru_info_manager = ManagersList.get_manager(ManagersTypes.FRU_INFO_MANAGER)
        usb_device_manager = ManagersList.get_manager(ManagersTypes.USB_DEVICE_MANAGER)
        pci_device_manager = ManagersList.get_manager(ManagersTypes.PCI_DEVICE_MANAGER)

        compute_system = ComputeSystem()

        compute_system.processor = processor_manager.get_data(data_node, context=cls.my_context())
        compute_system.memory = memory_manager.get_data(data_node, context=cls.my_context())
        storage_subsystem = StorageSubsystem()
        storage_subsystem.storageController = storage_controller_manager.get_data(data_node, context=cls.my_context())
        compute_system.storageSubsystem = storage_subsystem
        compute_system.networkInterface = network_interface_manager.get_data(data_node, context=cls.my_context())
        compute_system.usbDevice = usb_device_manager.get_data(data_node, context=cls.my_context())
        compute_system.pciDevice = pci_device_manager.get_data(data_node, context=cls.my_context())

        compute_system = cls.set_fields(compute_system, data_node, context=context)
        compute_system.fruInfo = fru_info_manager.get_data(fru_blade, context=cls.my_context())

        return compute_system

    @classmethod
    @abstractmethod
    def set_fields(cls, compute_system, data, context=None):
        pass

    @staticmethod
    def my_context():
        return ManagersTypes.COMPUTE_SYSTEM_MANAGER
