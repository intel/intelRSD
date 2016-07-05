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

from include.psme_xml_structure.managers.dimm_manager import DimmManager as DimmManager_abstract
from include.libs.dmidecode import dmidecode
from include.common.globals import *

MEMORY_DATA_SECTION = 0

class DimmManager(DimmManager_abstract):
    @classmethod
    def set_fields(cls, dimm, data, context=None):

        dimm.deviceLocator = str(data[LSHW_SLOT])
        if dimm.deviceLocator == "DIMM A":
            dimm.deviceLocator = "A0"
        if dimm.deviceLocator == "DIMM B":
            dimm.deviceLocator = "A1"
        dimm.dataWidthBits = int(data[LSHW_WIDTH][XML_HASH_TEXT])
        dimm_details = dmidecode.get_memory_info(dimm.fruInfo.serialNumber)
        if DMI_TOTAL_WIDTH in dimm_details:
            dimm.busWidthBits = dimm_details[DMI_TOTAL_WIDTH].split(' ')[MEMORY_DATA_SECTION]
        if DMI_RANK in dimm_details:
            dimm.rankCount = dimm_details[DMI_RANK]
        if DMI_CNF_CLOCK_SPEED in dimm_details:
            dimm.operatingSpeedMHz = dimm_details[DMI_CNF_CLOCK_SPEED].split(' ')[MEMORY_DATA_SECTION]
        if DMI_MAX_VOLTAGE in dimm_details:
            dimm.maximumVoltageVolt = dimm_details[DMI_MAX_VOLTAGE].split(' ')[MEMORY_DATA_SECTION]
        if DMI_MIN_VOLTAGE in dimm_details:
            dimm.minimumVoltageVolt = dimm_details[DMI_MIN_VOLTAGE].split(' ')[MEMORY_DATA_SECTION]
        if DMI_VOLTAGE in dimm_details:
            dimm.voltageVolt = dimm_details[DMI_VOLTAGE].split(' ')[MEMORY_DATA_SECTION]

        return dimm

    @classmethod
    def split_data(cls, data):
        ret = []
        for node in data:
            if XML_NODE in node.keys():
                mem_node = node[XML_NODE]
                if not isinstance(mem_node, list):
                    mem_node = [mem_node]
                for dimm in mem_node:
                    if "[empty]" not in dimm[LSHW_DESCRIPTION]:
                        ret.append(dimm)

        return ret

    @classmethod
    def prepare_data(cls, data):
        ret = []
        for node in data:
            if str(node[XML_AT_ID]).startswith(LSHW_MEMORY):
                ret.append(node)

        return ret

