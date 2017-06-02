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
from include.psme_xml_structure.model.psme_structure_element import PsmeStructureElement

from include.psme_xml_structure.model.compute_system import ComputeSystem
from include.psme_xml_structure.model.chassis import Chassis

class ManagerBlade(PsmeStructureElement):
    def __init__(self):
        self.ComputeSystem = ComputeSystem()
        self.chassis = Chassis("Module")
        self.chassis.drive = []
        self.serialConsole = {"enabled" : "true"}

