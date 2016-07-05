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

from include.psme_xml_structure.managers.compute_system_manager import ComputeSystemManager as ComputeSystem_abstract
from include.common.shell_command import ShellCommand
from include.libs.xmltodict import xmltodict
from include.libs.kernelparams import kernelparams
from include.libs.kernelparams.location import Location
from include.common.globals import *

class ComputeSystemManager(ComputeSystem_abstract):
    @classmethod
    def set_fields(cls, compute_system, data, context=None):
        loc = kernelparams.read_location()
        compute_system.location = int(loc.blade)

        for node in data:
            if type(node) != dict:
                continue
            if node == LSHW_FIRMWARE:
                break

        return compute_system

    @classmethod
    def prepare_data(cls, data):
        if data is None:
            lshw = ShellCommand("lshw -xml")
            lshw.execute()
            data = lshw.parse_output_xml_to_dict()
        return data
