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

from include.psme_xml_structure.model.dimm import Dimm


class DimmManager(Manager):
    @classmethod
    def get_data(cls, data, context=None):
        data = cls.prepare_data(data)
        ret = []
        fru_info_manager = ManagersList.get_manager(ManagersTypes.FRU_INFO_MANAGER)

        data = cls.split_data(data)
        for data_block in data:
            dimm = Dimm()

            dimm.fruInfo = fru_info_manager.get_data(data_block, context=cls.my_context())

            dimm = cls.set_fields(dimm, data_block, context=context)

            ret.append(dimm)

        return ret

    @classmethod
    @abstractmethod
    def set_fields(cls, dimm, data, context=None):
        pass

    @classmethod
    @abstractmethod
    def split_data(cls, data):
        # function splitting data and returning list of data blocks
        # one block for each dimm
        pass

    @staticmethod
    def my_context():
        return ManagersTypes.DIMM_MANAGER
