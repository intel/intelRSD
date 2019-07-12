"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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
import os
from abc import abstractmethod

from cts_core.commons.services import ServiceTypes
from cts_core.metadata.metadata_manager import MetadataManager
from cts_framework.commons.config_reader import get_configuration_from_file, get_configuration_from_files
from cts_framework.configuration.configuration import Configuration


class Loader(object):
    @abstractmethod
    def load(self, metadata_ref, qualifiers):
        pass


class DirLoader(Loader):
    def load(self, metadata_ref, qualifiers):
        if os.path.isdir(metadata_ref):
            metadata_manager = MetadataManager(qualifiers)
            if metadata_manager.read_metadata_from_dir(metadata_ref):
                return metadata_manager.metadata_container
        return None


class RemoteLoader(Loader):
    def load(self, metadata_ref, qualifiers):
        if os.path.isfile(metadata_ref):
            metadata_manager = MetadataManager(qualifiers)
            configuration = get_configuration_from_file(metadata_ref)
            if configuration is None:
                return None
            if metadata_manager.download_metadata(Configuration(**configuration)):
                return metadata_manager.metadata_container
        return None


class OfficialReleaseLoader(Loader):
    def load(self, metadata_ref, qualifiers):
        try:
            metadata_ref = self._autocomplete_name(metadata_ref)
        except KeyError:
            pass

        metadata_manager = MetadataManager(qualifiers)
        if metadata_manager.read_metadata_for_services(metadata_ref):
            return metadata_manager.metadata_container
        return None

    @staticmethod
    def _autocomplete_name(metadata_ref):
        return {
            '2_5': ServiceTypes.RACKSCALE_2_5,
            '2018_1': ServiceTypes.REDFISH_2018_1,
            '2018_2': ServiceTypes.REDFISH_2018_2,
            '2018_3': ServiceTypes.REDFISH_2018_3,
            '2019_1': ServiceTypes.REDFISH_2019_1
        }[metadata_ref]


LOADER_CHAIN = vars()['Loader'].__subclasses__()
