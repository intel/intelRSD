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

import os.path

from data_model.protocols import Protocols
from data_model.service_description import ServiceDescription
from data_model.service_localization import ServiceLocalization
from data_model.service_parameters import ServiceParameters
from parsers.service_retriever import ServiceRetriever
from utils.json_utils import JsonUtils


class LeasesParser(ServiceRetriever):
    _leases_file_location = '/tmp/leases'
    _service_connection_configuration_file_location = '/etc/pod-manager/service-connection.json'

    @staticmethod
    def _additional_work():
        descriptions = []

        if os.path.isfile(LeasesParser._leases_file_location):
            with open(LeasesParser._leases_file_location) as leases_file:
                lines = leases_file.readlines()

                for line in lines:
                    line_split = line.split()

                    host = line_split[1]

                    service_type = line_split[2]

                    [port, protocol] = LeasesParser._read_connection_security_parameters(service_type)

                    description = ServiceDescription(
                        ServiceLocalization(host, port),
                        ServiceParameters(service_type, ServiceRetriever._pod_manager_root_path, protocol)
                    )

                    descriptions.append(description)

        return descriptions

    @staticmethod
    def _read_connection_security_parameters(service_type):
        try:
            with open(LeasesParser._service_connection_configuration_file_location) as file:
                service_connection = JsonUtils.load(file)

            connection_security = service_connection['ConnectionSecurity']

            if service_type == 'lui':
                if connection_security['SslEnabledForLui']:
                    port = connection_security['SslPortForLui']
                    protocol = Protocols.HTTPS
                else:
                    port = connection_security['DefaultPortForLui']
                    protocol = Protocols.HTTP
            elif service_type == 'psme':
                if connection_security['SslEnabledForPsme']:
                    port = connection_security['SslPortForPsme']
                    protocol = Protocols.HTTPS
                else:
                    port = connection_security['DefaultPortForPsme']
                    protocol = Protocols.HTTP
            elif service_type == 'rmm':
                if connection_security['SslEnabledForRmm']:
                    port = connection_security['SslPortForRmm']
                    protocol = Protocols.HTTPS
                else:
                    port = connection_security['DefaultPortForRmm']
                    protocol = Protocols.HTTP
            elif service_type == 'storage':
                if connection_security['SslEnabledForRss']:
                    port = connection_security['SslPortForRss']
                    protocol = Protocols.HTTPS
                else:
                    port = connection_security['DefaultPortForRss']
                    protocol = Protocols.HTTP
            else:
                port = ServiceRetriever._default_port
                protocol = ServiceRetriever._default_protocol
        except:
            port = ServiceRetriever._default_port
            protocol = ServiceRetriever._default_protocol

        return [port, protocol]
