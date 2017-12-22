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

import collections
import json

from data_model.data import Data
from data_model.resource import Resource
from data_model.service import Service
from data_model.service_description import ServiceDescription
from data_model.service_localization import ServiceLocalization
from data_model.service_parameters import ServiceParameters


class JsonToDataConverter:
    @staticmethod
    def load(file_name):
        with open(file_name) as input_file:
            content = input_file.read()

        json_data_representation = json.loads(content, object_pairs_hook=collections.OrderedDict)

        data = Data()

        for json_service_representation in json_data_representation:
            service = Service(ServiceDescription(
                ServiceLocalization(json_service_representation['host'], json_service_representation['port']),
                ServiceParameters(json_service_representation['service_type'], '/redfish/v1', None)
            ))

            for json_resource_representation in json_service_representation['resources']:
                resource = Resource(
                    json_resource_representation['path'],
                    json_resource_representation['status_code'],
                    json_resource_representation['headers'],
                    json_resource_representation['body']
                )

                service.add_resource(resource)

            data.add_service(service)

        return data
