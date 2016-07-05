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

from include.psme_xml_structure.managers.fru_info_manager import FruInfoManager as FruInfoManager_abstract
from include.psme_xml_structure.managers.managers_list import ManagersTypes
from include.common.globals import *
from include.libs.udevadm.udevadm import Udevadm


class FruInfoManager(FruInfoManager_abstract):
    @classmethod
    def set_fields(cls, fru_info, data, context=None):
        if context == ManagersTypes.DIMM_MANAGER:
            fru_info.serialNumber = str(data[XML_SERIAL])
            fru_info.manufacturer = str(data[XML_VENDOR])
            fru_info.partNumber = str(data[XML_PRODUCT])
        elif context == ManagersTypes.DRIVE_MANAGER:
            if XML_NODE in data.keys():
                if isinstance(data[XML_NODE], dict):
                    if data[XML_NODE][XML_AT_ID].startswith(LSHW_DISK):
                        data = data[XML_NODE]
            if "intel" in data[XML_PRODUCT].lower():
                fru_info.manufacturer = "Intel"

            fru_info.serialNumber, fru_info.modelNumber = Udevadm.get_udevadm_info(data[LSHW_LOGICALNAME])

        elif context == ManagersTypes.COMPUTE_SYSTEM_MANAGER:
            fru_info.serialNumber = data[XML_SERIAL]
            fru_info.manufacturer = data[XML_VENDOR]
            fru_info.modelNumber = data[XML_PRODUCT]

        return fru_info
