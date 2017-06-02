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

from include.psme_xml_structure.managers.memory_manager import MemoryManager as MemoryManager_abstract
from include.libs.dmidecode import dmidecode
from include.common.globals import *

MEMORY_DATA_SECTION = 0

class MemoryManager(MemoryManager_abstract):
    @classmethod
    def set_fields(cls, memory, data, context=None):

        memory.deviceLocator = str(data[LSHW_SLOT])
        if memory.deviceLocator == "DIMM A":
            memory.deviceLocator = "A0"
        if memory.deviceLocator == "DIMM B":
            memory.deviceLocator = "A1"
        memory.dataWidthBits = int(data[LSHW_WIDTH][XML_HASH_TEXT])
        memory_details = dmidecode.get_memory_info(memory.fruInfo.serialNumber)
        if DMI_TOTAL_WIDTH in memory_details:
            memory.busWidthBits = memory_details[DMI_TOTAL_WIDTH].split(' ')[MEMORY_DATA_SECTION]
        if DMI_RANK in memory_details:
            memory.rankCount = memory_details[DMI_RANK]
        if DMI_CNF_CLOCK_SPEED in memory_details:
            memory.operatingSpeedMHz = memory_details[DMI_CNF_CLOCK_SPEED].split(' ')[MEMORY_DATA_SECTION]
        if DMI_MAX_VOLTAGE in memory_details:
            memory.maximumVoltageVolt = memory_details[DMI_MAX_VOLTAGE].split(' ')[MEMORY_DATA_SECTION]
        if DMI_MIN_VOLTAGE in memory_details:
            memory.minimumVoltageVolt = memory_details[DMI_MIN_VOLTAGE].split(' ')[MEMORY_DATA_SECTION]
        if DMI_VOLTAGE in memory_details:
            memory.voltageVolt = memory_details[DMI_VOLTAGE].split(' ')[MEMORY_DATA_SECTION]

        return memory

    @classmethod
    def split_data(cls, data):
        ret = []
        for node in data:
            if XML_NODE in node.keys():
                mem_node = node[XML_NODE]
                if not isinstance(mem_node, list):
                    mem_node = [mem_node]
                for memory in mem_node:
                    if "[empty]" not in memory[LSHW_DESCRIPTION]:
                        ret.append(memory)

        return ret

    @classmethod
    def prepare_data(cls, data):
        ret = []
        for node in data:
            if str(node[XML_AT_ID]).startswith(LSHW_MEMORY):
                ret.append(node)

        return ret

