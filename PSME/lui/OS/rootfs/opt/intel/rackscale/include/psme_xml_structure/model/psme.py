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
from include.psme_xml_structure.model.psme_structure_element import PsmeStructureElement

from include.psme_xml_structure.model.status import Status
from include.psme_xml_structure.model.network_service import NetworkService
from include.psme_xml_structure.model.serial_console import SerialConsole


class Psme(PsmeStructureElement):
    def __init__(self):
        self.firmwareVersion = None
        self.ipv4address = None
        self.networkServices = [NetworkService()]
        self.serialConsole = SerialConsole()
