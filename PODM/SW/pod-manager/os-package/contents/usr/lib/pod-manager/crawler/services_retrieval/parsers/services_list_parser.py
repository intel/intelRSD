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

import urllib.parse

from data_model.protocols import Protocols
from data_model.service_description import ServiceDescription
from data_model.service_localization import ServiceLocalization
from data_model.service_parameters import ServiceParameters


class ServicesListParser:
    services_register_location = '/tmp/services.list'

    @staticmethod
    def parse_line(line):
        line_split = line.split()
        uri = line_split[0]
        parsed_uri = urllib.parse.urlsplit(uri)
        root_path = parsed_uri.path
        protocol = Protocols.from_string(parsed_uri.scheme)
        service_type = line_split[1]

        service_localization = ServiceLocalization(parsed_uri.hostname, parsed_uri.port)
        service_parameters = ServiceParameters(service_type, root_path, protocol)

        return [ServiceDescription(service_localization, service_parameters)]
