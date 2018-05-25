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


class JsonUtils:
    @staticmethod
    def extract_descendants(node, descendants_key):
        values = []

        if not (type(node) == dict or type(node) == list):
            return values

        if type(node) == list:
            for item in node:
                values += JsonUtils.extract_descendants(item, descendants_key)
        else:
            for sub_key in node:
                if sub_key == descendants_key:
                    values.append(node[sub_key])
                else:
                    values += JsonUtils.extract_descendants(node[sub_key], descendants_key)

        return values
