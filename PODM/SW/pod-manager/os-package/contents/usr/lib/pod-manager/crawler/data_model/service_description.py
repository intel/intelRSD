# Copyright (c) 2016-2017 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import urllib.parse

from data_model.protocols import Protocols
from data_model.service_localization import ServiceLocalization
from data_model.service_parameters import ServiceParameters


class ServiceDescription:
    def __init__(self, service_localization, service_parameters):
        self._service_localization = service_localization
        self._service_parameters = service_parameters

    @staticmethod
    def from_uri(uri):
        parsed_uri = urllib.parse.urlsplit(uri)

        host = parsed_uri.hostname
        port = parsed_uri.port
        protocol = Protocols.from_string(parsed_uri.scheme)
        root_path = parsed_uri.path
        service_type = 'unknown'

        service_localization = ServiceLocalization(host, port)
        service_parameters = ServiceParameters(service_type, root_path, protocol)

        return ServiceDescription(service_localization, service_parameters)

    def __eq__(self, other):
        return self._service_localization == other.get_service_localization()

    def __str__(self):
        return '{service_localization} {service_parameters}'.format(service_localization=self._service_localization, service_parameters=self._service_parameters)

    def get_service_localization(self):
        return self._service_localization

    def get_service_parameters(self):
        return self._service_parameters
