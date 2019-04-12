"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

NAME = "name"
DESCRIPTION = "description"
IS_REQUIRED = "is_required"


class TestScriptParameter:
    def __init__(self, input_dictionary=None):
        if input_dictionary is not None:
            self.name = input_dictionary[NAME]
            self.description = input_dictionary[DESCRIPTION]
            self.is_required = input_dictionary[IS_REQUIRED]
        else:
            self.name = ""
            self.description = ""
            self.is_required = None

    def save_to_dict(self):
        return {
            NAME: self.name,
            DESCRIPTION: self.description,
            IS_REQUIRED: self.is_required
        }
