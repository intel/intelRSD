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

from include.psme_xml_structure.managers.network_interface_manager import \
    NetworkInterfaceManager as NetworkInterfaceManager_abstract
from include.libs.iplink import iplink
from include.libs.ethtool import ethtool
from include.common.globals import *


class NetworkInterfaceManager(NetworkInterfaceManager_abstract):
    @classmethod
    def set_fields(cls, network_interface, data, context=None):
        network_interface.macAddress = str(data[XML_SERIAL])
        network_interface.factoryMacAddress = network_interface.macAddress
        interface_logicalname = str(data[NET_LOGICALNAME])
        network_interface.frameSize = iplink.get_nic_mtu(interface_logicalname)
        settings = data[LSHW_CONFIGURATION][LSHW_SETTING]
        if not isinstance(settings, list):
            settings = [settings]
        for prop in settings:
            if prop[XML_AT_ID] == NET_AUTONEGOTIATION:
                if prop[XML_AT_VALUE] == LSHW_ON:
                    value = XML_TRUE
                else:
                    value = XML_FALSE
                network_interface.autoSense = value
            elif prop[XML_AT_ID] == NET_DUPLEX:
                if prop[XML_AT_VALUE] == NET_FULL:
                    value = XML_TRUE
                else:
                    value = XML_FALSE
                network_interface.fullDuplex = value
            elif prop[XML_AT_ID] == NET_SPEED:
                if NET_GBITS in prop[XML_AT_VALUE]:
                    network_interface.speedMbps = int(prop[XML_AT_VALUE].split(NET_GBITS)[0])*1000
                elif NET_MBITS in prop[XML_AT_VALUE]:
                    network_interface.speedMbps = int(prop[XML_AT_VALUE].split(NET_MBITS)[0])

        if network_interface.speedMbps == "":
            ethtool_info = ethtool.get_ethtool_info(interface_logicalname)
            if ethtool_info[NET_SPEED] != -1:
                network_interface.speedMbps = ethtool_info[NET_SPEED]

        return network_interface

    @classmethod
    def split_data(cls, data):
        ret = []
        for node in data:
            try:
                if node[XML_AT_ID].startswith(LSHW_PCI):
                    node = node[XML_NODE]
                    if not isinstance(node, list):
                        node = [node]
                    for pci_dev in node:
                        try:
                            if pci_dev[XML_AT_ID].startswith(LSHW_PCI):
                                if isinstance(pci_dev[XML_NODE], dict):
                                    if pci_dev[XML_NODE][XML_AT_ID] == LSHW_NETWORK:
                                        ret.append(pci_dev[XML_NODE])
                                elif isinstance(pci_dev[XML_NODE], list):
                                    for dev in pci_dev[XML_NODE]:
                                        if isinstance(dev[XML_NODE], dict):
                                            if dev[XML_NODE][XML_AT_ID] == LSHW_NETWORK:
                                                ret.append(dev[XML_NODE])
                            elif pci_dev[XML_AT_ID].startswith(LSHW_NETWORK):
                                ret.append(pci_dev)
                        except KeyError:
                            # node is not containing information in proper format ->
                            # there are no information about network interfaces
                            pass
            except KeyError:
                # node is not containing information in proper format ->
                # there are no information about network interfaces
                pass

        return ret

