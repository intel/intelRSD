# Copyright (c) 2017-2018 Intel Corporation
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

import json

from data_model.protocols import Protocols
from pod_manager_config.pod_manager_defaults import PodManagerDefaults


class ServiceConnectionReader:
    _service_connection_configuration_file_location = '/etc/pod-manager/service-connection.json'

    @staticmethod
    def read_connection_security_parameters(service_type):
        try:
            with open(ServiceConnectionReader._service_connection_configuration_file_location) as input_file:
                service_connection = json.load(input_file)

            connection_security = service_connection['ConnectionSecurity']

            if service_type == 'lui':
                if connection_security['SslEnabledForLui']:
                    ports = connection_security['SslPortsForLui']
                    protocol = Protocols.HTTPS
                else:
                    ports = connection_security['HttpPortsForLui']
                    protocol = Protocols.HTTP
            elif service_type == 'psme':
                if connection_security['SslEnabledForPsme']:
                    ports = connection_security['SslPortsForPsme']
                    protocol = Protocols.HTTPS
                else:
                    ports = connection_security['HttpPortsForPsme']
                    protocol = Protocols.HTTP
            elif service_type == 'rmm':
                if connection_security['SslEnabledForRmm']:
                    ports = connection_security['SslPortsForRmm']
                    protocol = Protocols.HTTPS
                else:
                    ports = connection_security['HttpPortsForRmm']
                    protocol = Protocols.HTTP
            elif service_type == 'storage':
                if connection_security['SslEnabledForRss']:
                    ports = connection_security['SslPortsForRss']
                    protocol = Protocols.HTTPS
                else:
                    ports = connection_security['HttpDefaultPortsForRss']
                    protocol = Protocols.HTTP
            else:
                ports = [PodManagerDefaults.port]
                protocol = PodManagerDefaults.protocol
        except:
            ports = [PodManagerDefaults.port]
            protocol = PodManagerDefaults.protocol

        return [ports, protocol]
