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


class Resource:
    def __init__(self, path, status_code, headers, body):
        self._path = path
        self._status_code = status_code
        self._headers = headers
        self._body = body

    def get_path(self):
        return self._path

    def get_status_code(self):
        return self._status_code

    def get_headers(self):
        return self._headers

    def get_body(self):
        return self._body
