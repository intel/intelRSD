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

from include.psme_xml_structure.managers.cpu_id_manager import CpuIdManager as CpuIdManager_abstract
from include.psme_xml_structure.managers.managers_list import ManagersTypes
from include.libs.cpuinfo import cpuinfo
from include.common.globals import *

class CpuIdManager(CpuIdManager_abstract):
    @classmethod
    def set_fields(cls, cpu_id, data, context=None):
        if context == ManagersTypes.PROCESSOR_MANAGER:
            cpu_info = cpuinfo.get_cpu_info()
            cpu_id.vendorId = cpu_info[CPUID_VENDOR_ID]
            cpu_id.numericId = 0
            cpu_id.family = cpu_info[CPUID_FAMILY]
            cpu_id.model = cpu_info[CPUID_MODEL]
            cpu_id.step = cpu_info[CPUID_STEPPING]
            cpu_id.microcodeInfo = cpu_info[CPUID_MICROCODE]

        return cpu_id

