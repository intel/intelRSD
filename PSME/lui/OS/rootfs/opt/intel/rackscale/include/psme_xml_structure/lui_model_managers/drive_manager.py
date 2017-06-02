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
from include.common.shell_command import ShellCommand

from include.psme_xml_structure.managers.drive_manager import DriveManager as DriveManager_abstract
from include.common.globals import *

DRIVE_RPM_DATA_SIZE = 2
DRIVE_RPM_SECTION = 0

class DriveManager(DriveManager_abstract):
    @classmethod
    def set_fields(cls, drive, data, context=None):
        try:
            if XML_NODE in data.keys():
                if isinstance(data[XML_NODE], dict):
                    if data[XML_NODE][XML_AT_ID].startswith(LSHW_DISK):
                        data = data[XML_NODE]
            capabilities = data[LSHW_CAPABILITIES][LSHW_CAPABILITY]
            if not isinstance(capabilities, list):
                capabilities = [capabilities]
            for cap in capabilities:
                rpm_info = cap[XML_AT_ID].split(LSHW_RPM)
                if len(rpm_info) == DRIVE_RPM_DATA_SIZE:
                    try:
                        drive_hdparm = ShellCommand("hdparm -I " + str(data[LSHW_LOGICALNAME]) + " | grep Rotation | cut -d':' -f2")
                        drive_hdparm.execute()
                        drive.rpm = int(drive_hdparm.get_stdout())
                    except ValueError:
                        # unable to parse hdparm output - it returned "solid state drive" in "rotation" section
                        drive.rpm = 0

            if DISK_SSD_TYPE in data[XML_PRODUCT]:
                drive.type = DISK_SSD_TYPE
                del drive.rpm
            else:
                drive.type = DISK_HDD_TYPE
            drive.capacityGB = int(int(data[LSHW_SIZE][XML_HASH_TEXT]) / 1000 ** 3)
            drive.physicalId = data[LSHW_PHYSID]
            drive.firmwareVersion = data[XML_VERSION]
        except (KeyError, TypeError):
            return None
        return drive

    @classmethod
    def prepare_data(cls, data):
        ret = []
        drives = data[LSHW_DRIVES]
        if not isinstance(drives, list):
            drives = [drives]
        for drive in drives:
            if drive[LSHW_PCI_STORAGE_CNTRL] in data[LSHW_STORAGE][LSHW_BUSINFO]:
                ret.append(drive)
        return ret

    @classmethod
    def split_data(cls, data):
        return data

