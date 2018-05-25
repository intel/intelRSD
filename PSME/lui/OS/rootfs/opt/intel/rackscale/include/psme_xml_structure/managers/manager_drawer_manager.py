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
from abc import abstractmethod

from include.psme_xml_structure.managers.manager import Manager
from include.psme_xml_structure.managers.managers_list import ManagersList, ManagersTypes

from include.psme_xml_structure.model.manager_drawer import ManagerDrawer


class ManagerDrawerManager(Manager):
    @classmethod
    def get_data(cls, data, context=None):
        data = cls.prepare_data(data)
        psme_manager = ManagersList.get_manager(ManagersTypes.PSME_MANAGER)
        compute_module_manager = ManagersList.get_manager(ManagersTypes.MANAGER_BLADE_MANAGER)

        manager_drawer = ManagerDrawer()
        manager_drawer.manager = compute_module_manager.get_data(data, context=cls.my_context())

        manager_drawer = cls.set_fields(manager_drawer, data, context=context)

        cls.move_drives_to_chassis(manager_drawer)

        return manager_drawer

    @classmethod
    @abstractmethod
    def set_fields(cls, manager_drawer, data, context=None):
        pass

    @staticmethod
    def my_context():
        return ManagersTypes.MANAGER_DRAWER_MANAGER

    @classmethod
    def move_drives_to_chassis(cls, manager_drawer):
        if hasattr(manager_drawer.manager.ComputeSystem.storageSubsystem, 'storageController'):
            for storageSubsystem in manager_drawer.manager.ComputeSystem.storageSubsystem.storageController:
                for drive in list(storageSubsystem.drive):
                    storageSubsystem.drive.remove(drive)
                    manager_drawer.manager.chassis.drive.append(drive)
