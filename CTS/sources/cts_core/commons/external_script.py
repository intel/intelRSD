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
import imp
import os

from cts_framework.helpers.vars.env_consts import Constants


class ExecuteExternalScript:
    def __init__(self, script_path, raw_response, expected_result):
        """
        SetUp a script path to external path and expected result

        :param script_path absolute path for external script
        """

        self.__expected_result = expected_result
        self.__raw_response = raw_response
        self.__achieved_result = None

        self.script_path = None

        if list(script_path)[0] != "/":
            script_path = os.path.join(*(Constants.DIR_HOME_CTS_SCENARIO_EXTERNAL, script_path))
            if not os.path.isfile(script_path):
                exit(-1)

        try:
            self.script_path = imp.load_source('', script_path)
        except IOError:
            print(Constants.EXTERNAL_SCRIPT_WAS_NOT_FOUND)

    def __call__(self, *args, **kwargs):
        """
        Allow to call() script with additional arguments
        """
        try:
            self.__achieved_result = self.script_path.Verify().run(raw_response=self.__raw_response,
                                                                                       expected_result=self.__expected_result,
                                                                                       *args, **kwargs)
        except AttributeError:
            print(Constants.EXTERNAL_SCRIPT_WAS_NOT_FOUND)
        return self.verify()

    def verify(self):
        return self.__get_achieved_result() == self.__expected_result

    def __get_achieved_result(self):
        return self.__achieved_result
