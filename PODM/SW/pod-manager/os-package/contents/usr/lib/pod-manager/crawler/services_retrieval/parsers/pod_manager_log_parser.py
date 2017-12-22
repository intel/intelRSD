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

import re
import urllib.parse

from data_model.service_description import ServiceDescription
from data_model.service_localization import ServiceLocalization
from data_model.service_parameters import ServiceParameters


# FIXME: Handle compressed pod-manager-services-detection.log files, not only the most actual one
# FIXME: Handle repetitions of services from pod-manager-services-detection.log
class PodManagerLogParser:
    services_register_location = '/var/log/pod-manager/pod-manager-services-detection.log'
    _service_detected_pattern = re.compile(r'\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2},\d{3} \[EE-ManagedScheduledExecutorService-TasksExecutor-Thread-\d+\] INFO  c.i.p.d.e.ServiceDetectionListenerImpl - Service \{URI=(.*?), type=(.*?), UUID=(.*?)\} detected')

    @staticmethod
    def parse_line(line):
        match = PodManagerLogParser._service_detected_pattern.match(line)

        uri = match.group(1)
        service_type = match.group(2)
        parsed_uri = urllib.parse.urlsplit(uri)

        service_localization = ServiceLocalization(parsed_uri.hostname, parsed_uri.port)
        service_parameters = ServiceParameters(service_type, parsed_uri.path, parsed_uri.scheme)

        return [ServiceDescription(service_localization, service_parameters)]
