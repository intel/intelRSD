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

import collections
import json


class DataToJsonConverter:
    @staticmethod
    def dumps(data, minimal):
        json_data_representation = []

        for service in data.get_services():
            service_description = service.get_service_description()

            json_service_representation = collections.OrderedDict()
            json_service_representation['host'] = service_description.get_service_localization().get_host()
            json_service_representation['port'] = service_description.get_service_localization().get_port()
            json_service_representation['service_type'] = service_description.get_service_parameters().get_service_type()
            json_service_representation['number_of_resources'] = len(service.get_resources())
            json_service_representation['retrieval_duration'] = service.get_retrieval_duration()
            json_service_representation['resources'] = []

            for resource in service.get_resources():
                json_resource_representation = collections.OrderedDict()
                json_resource_representation['path'] = resource.get_path()
                json_resource_representation['retrieval_duration'] = resource.get_retrieval_duration()
                if not minimal:
                    json_resource_representation['status_code'] = resource.get_status_code()
                    json_resource_representation['headers'] = collections.OrderedDict(resource.get_headers())

                json_resource_representation['body'] = json.loads(resource.get_body(), object_pairs_hook=collections.OrderedDict)

                json_service_representation['resources'].append(json_resource_representation)

            json_service_representation['resources'] = sorted(
                json_service_representation['resources'],
                key=lambda r: r['path']
            )

            json_data_representation.append(json_service_representation)

        return json.dumps(json_data_representation, indent=4)
