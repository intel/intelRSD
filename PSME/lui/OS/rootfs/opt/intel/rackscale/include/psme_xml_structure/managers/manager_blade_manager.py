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
from abc import abstractmethod

from include.psme_xml_structure.managers.manager import Manager
from include.psme_xml_structure.managers.managers_list import ManagersList, ManagersTypes

from include.psme_xml_structure.model.manager_blade import ManagerBlade


class ManagerBladeManager(Manager):
    @classmethod
    def get_data(cls, data, context=None):
        data = cls.prepare_data(data)
        compute_system = ManagersList.get_manager(ManagersTypes.COMPUTE_SYSTEM_MANAGER)

        manager_blade = ManagerBlade()

        manager_blade.ComputeSystem = compute_system.get_data(data, context=cls.my_context())

        manager_blade = cls.set_fields(manager_blade, data, context=context)

        return manager_blade

    @classmethod
    @abstractmethod
    def set_fields(cls, compute_module, data, context=None):
        pass

    @staticmethod
    def my_context():
        return ManagersTypes.MANAGER_BLADE_MANAGER
