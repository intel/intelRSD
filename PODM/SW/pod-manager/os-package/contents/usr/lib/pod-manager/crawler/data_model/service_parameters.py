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


class ServiceParameters:
    def __init__(self, service_type, root_path, protocol):
        self._service_type = service_type
        self._root_path = root_path
        self._protocol = protocol

    def __str__(self):
        return '{service_type} {protocol}'.format(service_type=self._service_type, protocol=self._protocol)

    def get_service_type(self):
        return self._service_type

    def get_root_path(self):
        return self._root_path

    def get_protocol(self):
        return self._protocol
