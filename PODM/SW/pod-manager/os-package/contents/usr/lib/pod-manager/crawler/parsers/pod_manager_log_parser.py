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

import glob
import gzip
import re
import urllib.parse

from data_model.service_description import ServiceDescription
from data_model.service_localization import ServiceLocalization
from data_model.service_parameters import ServiceParameters
from parsers.service_retriever import ServiceRetriever


class PodManagerLogParser(ServiceRetriever):
    _pod_manager_log_file_location = '/var/log/pod-manager/pod-manager-services-detection.log'
    _pod_manager_archive_log_file_pattern = '/var/log/pod-manager/pod-manager-services-detection.[0-9].log.gz'
    _service_detected_pattern = r'\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2},\d{3} \[EE-ManagedScheduledExecutorService-TasksExecutor-Thread-\d+\] INFO  c.i.p.d.e.ServiceDetectionListenerImpl - Service \{URI=(.*?), type=(.*?), UUID=(.*?)\} detected'
    _service_detected_regular_expression = re.compile(_service_detected_pattern)

    @staticmethod
    def _additional_work():
        descriptions = []

        archived_log_files_locations = glob.glob(PodManagerLogParser._pod_manager_archive_log_file_pattern)
        archived_log_files_locations.sort()
        archived_log_files_locations.reverse()

        for archived_log_file_location in archived_log_files_locations:
            with gzip.open(archived_log_file_location, 'rt') as archived_log_file:
                PodManagerLogParser._append_descriptions_from_log(descriptions, archived_log_file)

        with open(PodManagerLogParser._pod_manager_log_file_location) as log_file:
            PodManagerLogParser._append_descriptions_from_log(descriptions, log_file)

        return descriptions

    @staticmethod
    def _append_descriptions_from_log(descriptions, file):
        for line in file:
            match = PodManagerLogParser._service_detected_regular_expression.match(line)

            if match is not None:
                uri = match.group(1)
                service_type = match.group(2)

                parsed = urllib.parse.urlparse(uri)
                protocol = parsed.scheme
                host = parsed.hostname
                port = parsed.port
                root_path = parsed.path

                description = ServiceDescription(
                    ServiceLocalization(host, port),
                    ServiceParameters(service_type, root_path, protocol)
                )

                if description in descriptions:
                    descriptions.remove(description)

                descriptions.append(description)
