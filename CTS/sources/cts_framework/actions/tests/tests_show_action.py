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
from tabulate import tabulate

from cts_framework.actions.action import Action
from cts_framework.tests_managing.tests_manager import TestsManager


class TestsShowAction(Action):
    ACTION = "show"
    PARAM_NAME = "TESTS_CMD"

    def fill_parser_arguments(self):
        self.parser.add_argument("package", type=str, help="package identifier", nargs=1)
        self.parser.add_argument("test_script", type=str, nargs=1, default=None,
                                 help="test script to show")

    def process_action(self, configuration):
        package_name = configuration.package
        test_script_name = configuration.test_script

        tests_manager = TestsManager()
        packages = tests_manager.get_packages()
        packages.filter(packages=package_name, scripts=test_script_name, remove_empty=True)

        try:
            package = packages.packages[0]
            test_script_info = packages.packages[0].suites[0].scripts[0]
        except IndexError:
            print "Test script not found"
            return

        info_table = [
            ["Test script package", package.name],
            ["Test script suite", package.suites[0].name],
            ["Test script name", test_script_info.name],
            ["Test script location", test_script_info.path],
            ["Test script description", test_script_info.description],
        ]
        print tabulate(info_table, tablefmt="plain", numalign="left", stralign="left")

        parameters_table = list()

        for parameter in test_script_info.parameters:
            parameters_table.append([parameter.name, "", ""])
            parameters_table.append(["", "Description", parameter.description])
            parameters_table.append(["", "Required", ("Yes" if parameter.is_required else "No")])
            parameters_table.append(["", "", ""])

        parameters_table = [["Parameters", "", "", ""]] + [[""] + i for i in parameters_table]

        print tabulate(parameters_table, tablefmt="plain")
