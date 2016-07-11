"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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
from tabulate import tabulate

from cts_framework.actions.action import Action
from cts_framework.tests_managing.tests_manager import TestsManager


class TestsShowAction(Action):
    ACTION = "show"
    PARAM_NAME = "TESTS_CMD"

    def fill_parser_arguments(self):
        self.parser.add_argument("-p", "--package", type=str, help="package identifier", nargs=1, required=True)
        self.parser.add_argument("-t", "--test_case", type=str, nargs=1, default=None, required=True,
                                 help="test case name")

    def process_action(self, configuration):
        package_name = configuration.package
        test_case_name = configuration.test_case

        tests_manager = TestsManager()

        try:
            test_case_info = tests_manager.read_tests_information().filter_test_cases(packages_names=package_name,
                                                                                      test_cases_names=test_case_name)[0]
        except IndexError:
            print "Test case not found"
            exit(0)
        info_table = [
            ["Test case name", test_case_info.name],
            ["Test case package", test_case_info.tests_package_name],
            ["Test case suite", test_case_info.tests_suite_name],
            ["Test case location", test_case_info.test_script_path],
            ["Test case timeout", test_case_info.timeout],
            ["Test case description", test_case_info.description],
        ]

        print tabulate(info_table, tablefmt="plain", numalign="left", stralign="left")

        parameters_table = list()

        for parameter in test_case_info.parameters:
            parameters_table.append([parameter.name, "", ""])
            parameters_table.append(["", "Description", parameter.description ])
            parameters_table.append(["", "Type", str(parameter.type)])
            parameters_table.append(["", "Required", ("Yes" if parameter.is_required else "No")])
            if not parameter.is_required:
                parameters_table.append(["", "Default value", parameter.default_value])

            parameters_table.append(["", "", ""])

        parameters_table = [["Parameters", "", "", ""]] + [[""] + i for i in parameters_table]

        print tabulate(parameters_table, tablefmt="plain")
