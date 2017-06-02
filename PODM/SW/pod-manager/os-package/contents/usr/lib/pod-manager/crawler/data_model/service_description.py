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


class ServiceDescription:
    def __init__(self, service_localization, service_parameters):
        self._service_localization = service_localization
        self._service_parameters = service_parameters

    def __eq__(self, other):
        return self._service_localization == other.get_service_localization()

    def get_service_localization(self):
        return self._service_localization

    def get_service_parameters(self):
        return self._service_parameters
