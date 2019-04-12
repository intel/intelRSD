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

from cts_framework.tests_managing.test_script.test_script_parameter import TestScriptParameter

PATH = "path"
NAME = "name"
PARAMETERS = "parameters"
DESCRIPTION = "description"
TIMEOUT = "timeout"
CONTROL_SUM = "control_sum"


class TestScript:
    def __init__(self, input_dictionary=None):
        if input_dictionary is not None:
            self.path = input_dictionary[PATH]
            self.name = input_dictionary[NAME]
            self.parameters = [TestScriptParameter(parameter_info) for parameter_info in input_dictionary[PARAMETERS]]
            """:type: list[cts_framework.tests_managing.test_script.test_script_parameter.TestScriptParameter] """
            self.description = input_dictionary[DESCRIPTION]
            self.timeout = input_dictionary[TIMEOUT]
            self.control_sum = input_dictionary[CONTROL_SUM]
        else:
            self.path = ""
            self.name = ""
            self.parameters = []
            """:type: list[cts_framework.tests_managing.test_script.test_script_parameter.TestScriptParameter] """
            self.description = ""
            self.timeout = None
            self.control_sum = ""

    def save_to_dict(self):
        output_dictionary = dict()
        output_dictionary[PATH] = self.path
        output_dictionary[NAME] = self.name
        output_dictionary[PARAMETERS] = [parameter.save_to_dict() for parameter in self.parameters]
        output_dictionary[DESCRIPTION] = self.description
        output_dictionary[TIMEOUT] = self.timeout
        output_dictionary[CONTROL_SUM] = self.control_sum

        return output_dictionary
