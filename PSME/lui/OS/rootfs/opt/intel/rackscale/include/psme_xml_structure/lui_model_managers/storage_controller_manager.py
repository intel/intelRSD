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
from include.psme_xml_structure.managers.storage_controller_manager import \
    StorageControllerManager as StorageControllerManager_abstract
from include.common.shell_command import ShellCommand
from include.common.exit_codes import EXIT_CODE_SUCCESS
from include.common.globals import *

STORAGE_SAS = "Serial Attached SCSI"
STORAGE_NVM = "Non-Volatile memory controller"
STORAGE_INTERFACES = ["PCIe", STORAGE_SAS, "SATA", STORAGE_NVM, "Unknown"]

class StorageControllerManager(StorageControllerManager_abstract):
    @classmethod
    def set_fields(cls, storage_controller, data, context=None):

        for interface in STORAGE_INTERFACES:
            if interface not in str(data[LSHW_STORAGE][LSHW_DESCRIPTION]):
                continue
            else:
                if interface == STORAGE_SAS:
                    interface = "SAS"
                elif interface == STORAGE_NVM:
                    interface = "NVMe"
                break
        storage_controller.interface = interface
        storage_controller.busInfo = data[LSHW_STORAGE][LSHW_BUSINFO]

        for drive in storage_controller.drive:
            drive.interface = storage_controller.interface

        return storage_controller

    @classmethod
    def prepare_data(cls, data):
        ret = []
        drives = cls.get_drives(data)
        for node in data:
            try:
                if node[XML_AT_ID].startswith(LSHW_PCI):
                    node = node[XML_NODE]
                    if not isinstance(node, list):
                        node = [node]
                    for pci_dev in node:
                        try:
                            if pci_dev[XML_AT_ID].startswith(LSHW_STORAGE):
                                ret.append({LSHW_STORAGE: pci_dev, LSHW_DRIVES: drives})
                            elif pci_dev[XML_AT_ID].startswith(LSHW_PCI):
                                pci_node = pci_dev[XML_NODE]
                                if isinstance(pci_node, dict):
                                    if pci_node[XML_AT_ID] == LSHW_STORAGE:
                                        ret.append({LSHW_STORAGE: pci_node, LSHW_DRIVES: drives})
                                elif isinstance(pci_node, dict):
                                    for dev in pci_node:
                                        if dev[XML_AT_ID] == LSHW_STORAGE:
                                            ret.append({LSHW_STORAGE: dev, LSHW_DRIVES: drives})
                        except (KeyError, TypeError):
                            # node is not containing information in proper format ->
                            # there are no information about network interfaces
                            pass
            except KeyError:
                # node is not containing information in proper format ->
                # there are no information about network interfaces
                pass

        return ret

    @classmethod
    def split_data(cls, data):
        return data


    @classmethod
    def get_drives(cls, data):
        drives = []
        for node in data:
            try:
                if node[XML_AT_ID].startswith(LSHW_SCSI):
                    # find pci bus where disk is connected
                    cmd = "udevadm info -q all -n " +\
                          str(node[XML_NODE][LSHW_LOGICALNAME]) +\
                          " | grep 'DEVPATH' | cut -d/  -f 4 "
                    pci_bus_command = ShellCommand(cmd, [])
                    pci_bus_command.execute()
                    if pci_bus_command.get_exitcode() == EXIT_CODE_SUCCESS:
                        node[LSHW_PCI_STORAGE_CNTRL] = pci_bus_command.get_stdout()[:-1]
                    else:
                        node[LSHW_PCI_STORAGE_CNTRL] = None
                    drives.append(node)
                elif node[XML_AT_ID].startswith(LSHW_PCI):
                    pci_list = node[XML_NODE]
                    if not isinstance(pci_list , list):
                        pci_list = [pci_list]
                    for pci in pci_list:
                        if pci[XML_AT_ID].startswith(LSHW_PCI):
                            if isinstance(pci[XML_NODE], dict):
                                if pci[XML_NODE][XML_AT_ID] == LSHW_STORAGE:
                                    resources = pci[XML_NODE][XML_NODE]
                                    if not isinstance(resources , list):
                                        resources = [resources]
                                    for disk in resources:
                                        if disk[XML_AT_ID].startswith(LSHW_DISK):
                                            disk[LSHW_PCI_STORAGE_CNTRL] = pci[XML_NODE][LSHW_BUSINFO]
                                            drives.append(disk)
                        elif pci[XML_AT_ID] == LSHW_STORAGE:
                            resources = pci[XML_NODE]
                            for disk in resources:
                                if disk[XML_AT_ID].startswith(LSHW_DISK):
                                    disk[LSHW_PCI_STORAGE_CNTRL] = pci[LSHW_BUSINFO]
                                    drives.append(disk)
            except (KeyError, TypeError):
                    pass
        return drives
