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

from include.psme_xml_structure.managers.manager_blade_manager import \
    ManagerBladeManager as ManagerBladeManager_abstract
from include.libs.kernelparams import kernelparams
from include.libs.kernelparams.location import Location

class ManagerBladeManager(ManagerBladeManager_abstract):
    @classmethod
    def set_fields(cls, compute_module, data, context=None):
        loc = kernelparams.read_location()
        compute_module.chassis.locationOffset = int(loc.module)
        compute_module.chassis.parentId = int(loc.drawer)
        return compute_module

