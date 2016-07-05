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
from abc import abstractmethod

from include.psme_xml_structure.managers.manager import Manager
from include.psme_xml_structure.managers.managers_list import ManagersList, ManagersTypes

from include.psme_xml_structure.model.psme import Psme


class PsmeManager(Manager):
    @classmethod
    def get_data(cls, data, context=None):
        data = cls.prepare_data(data)
        network_service_manager = ManagersList.get_manager(ManagersTypes.NETWORK_SERVICE_MANAGER)
        serial_console_manager = ManagersList.get_manager(ManagersTypes.SERIAL_CONSOLE_MANAGER)

        psme = Psme()

        psme.networkService = network_service_manager.get_data(data, context=cls.my_context())
        psme.serialConsole = serial_console_manager.get_data(data, context=cls.my_context())

        psme = cls.set_fields(psme, data, context=context)

        return psme

    @classmethod
    @abstractmethod
    def set_fields(cls, psme, data, context=None):
        pass

    @staticmethod
    def my_context():
        return ManagersTypes.PSME_MANAGER
