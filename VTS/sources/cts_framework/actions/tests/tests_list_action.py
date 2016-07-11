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


class TestsListAction(Action):
    ACTION = "list"
    PARAM_NAME = "TESTS_CMD"

    def fill_parser_arguments(self):
        self.parser.add_argument("-p", "--package", type=str, help="package identifier", nargs=1)
        self.parser.add_argument("-s", "--suite", type=str, nargs="*",
                                 help="list of test suites to show (if none provided show all)")
        self.parser.add_argument("-t", "--show_tests", action="store_true", default=False,
                                 help="show test cases")

    def process_action(self, configuration):
        tests_manager = TestsManager()

        show_suites = False if configuration.suite is None else True
        show_test_cases = configuration.show_tests

        test_cases = tests_manager.read_tests_information()
        filtered_test_cases = test_cases.filter_test_cases(packages_names=configuration.package, suites_names=configuration.suite)

        results = [[test_case.tests_package_name, test_case.tests_suite_name, test_case.name] for test_case in
                   filtered_test_cases]

        if not show_suites:
            results = [[result[0]] + result[2:] for result in results]

        if not show_test_cases:
            results = [result[:-1] for result in results]

        # getting rid of duplicates
        results = list(set([tuple(result) for result in results]))

        headers = ["Tests package"]
        if show_suites:
            headers.append("Test suite name")
        if show_test_cases:
            headers.append("Test case name")

        print tabulate(sorted(results), headers=headers)
