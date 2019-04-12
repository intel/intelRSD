"""
 * @copyright
 * Copyright (c) 2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

from json import dump as jsondump
from os import getcwd


class JsonParse:
    def __init__(self, available_targets, file_name):
        configuration = dict(
            title="Discovery manager configure file",
            description="List of all fpga targets provided by discovery manager",
            targets=available_targets
        )

        with open(file_name, 'w') as config_file:
            jsondump(configuration, config_file, sort_keys=True, indent=4)
