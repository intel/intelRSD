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

from include.psme_xml_structure.managers.processor_manager import ProcessorManager as ProcessorManager_abstract
from include.libs.cpuinfo import cpuinfo
from include.common.globals import *

PROC_MODELS = ["E3", "E5", "E7", "X3", "X5", "X7", "I3", "I5", "I7", "Silver", "Gold", "Platinum", "Unknown"]
MHZ=1000000

class ProcessorManager(ProcessorManager_abstract):
    @classmethod
    def set_fields(cls, processor, data, context=None):
        cpu_info = cpuinfo.get_cpu_info()
        arch = cpu_info[LSHW_ARCH].lower()
        instrSet = arch
        if arch == "x86_64":
            arch = "x86"
            instrSet = "x86-64"

        processor.processorArchitecture = arch
        processor.instructionSet = instrSet
        processor.processorType = data[LSHW_DESCRIPTION]
        processor.socket = data[LSHW_SLOT]

        try:
            processor.cpuId.numericId = data[XML_AT_ID].split(":")[1]
        except IndexError:
            processor.cpuId.numericId = 0
        processor.manufacturer = str(data[XML_VENDOR])
        for model in PROC_MODELS:
                if model.upper() not in str(data[XML_PRODUCT]).upper():
                    continue
                else:
                    break

        # work-around for SDV AVN X3 model, which model name is not written in processor chip
        if "Unknown" == model: model = "X3"

        processor.model = model
        processor.modelName = data[XML_PRODUCT]
        capabilities = data[LSHW_CAPABILITIES][LSHW_CAPABILITY]
        if not isinstance(capabilities, list):
            capabilities = [capabilities]
        for instr in capabilities:
            processor.capability.append(instr[XML_AT_ID])
        settings = data[LSHW_CONFIGURATION][LSHW_SETTING]
        if not isinstance(settings, list):
            settings = [settings]
        for prop in settings:
            if prop[XML_AT_ID] == LSHW_CORES:
                processor.totalCores = int(prop[XML_AT_VALUE])
            elif prop[XML_AT_ID] == LSWH_ENABLED_CORES:
                enabled_cores = int(prop[XML_AT_VALUE])
            elif prop[XML_AT_ID] == LSHW_THREADS:
                processor.totalThreads = int(prop[XML_AT_VALUE])

        processor.enabledCores = enabled_cores
        processor.enabledThreads = int(enabled_cores * (processor.totalThreads/processor.totalCores))
        processor.maxSpeedMHz = int(data[LSHW_SIZE][XML_HASH_TEXT])/MHZ

        return processor

    @classmethod
    def split_data(cls, data):
        ret = []
        for cpu in data:
            try:
                if cpu[LSHW_DESCRIPTION] == LSHW_CPU_TYPE:
                    ret.append(cpu)
            except KeyError:
                pass

        return ret

    @classmethod
    def prepare_data(cls, data):
        ret = []
        for node in data:
            try:
                if node[XML_AT_ID].startswith(LSHW_CPU):
                    ret.append(node)
            except KeyError:
                pass

        return ret

