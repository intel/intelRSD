# Copyright (c) 2016-2018 Intel Corporation
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

from data_model.service_description import ServiceDescription
from data_model.service_localization import ServiceLocalization
from data_model.service_parameters import ServiceParameters
from pod_manager_config.pod_manager_defaults import PodManagerDefaults
from pod_manager_config.readers.service_connection_reader import ServiceConnectionReader


class LeasesParser:
    services_register_location = '/tmp/leases'

    @staticmethod
    def parse_line(line):
        line_split = line.split()
        host = line_split[1]
        service_type = line_split[2]
        [ports, protocol] = ServiceConnectionReader.read_connection_security_parameters(service_type)

        service_parameters = ServiceParameters(service_type, PodManagerDefaults.root_path, protocol)

        service_descriptions = []

        for port in ports:
            service_localization = ServiceLocalization(host, port)
            service_description = ServiceDescription(service_localization, service_parameters)
            service_descriptions.append(service_description)

        return service_descriptions
