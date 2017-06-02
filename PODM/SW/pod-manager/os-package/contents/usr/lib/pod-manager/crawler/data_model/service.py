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


class Service:
    def __init__(self, service_description):
        self._service_description = service_description
        self._resources = []

    def get_service_description(self):
        return self._service_description

    def get_resources(self):
        return self._resources

    def add_resource(self, resource):
        self._resources.append(resource)

    def find_resource(self, path):
        for resource in self._resources:
            if resource.get_path() == path:
                return resource

        return None
