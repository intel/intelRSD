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


class ServiceLocalization:
    def __init__(self, host, port):
        self._host = host
        self._port = port

    def __eq__(self, other):
        return self._host == other.get_host() and self._port == other.get_port()

    def __str__(self):
        return '{host}:{port}'.format(host=self._host, port=self._port)

    def get_host(self):
        return self._host

    def get_port(self):
        return self._port
