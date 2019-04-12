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


class TestsListAction(Action):
    ACTION = "list"
    PARAM_NAME = "TESTS_CMD"

    def fill_parser_arguments(self):
        self.parser.add_argument("-p", "--packages", type=str, help="packages identifier", nargs="+")
        self.parser.add_argument("-s", "--suites", type=str, nargs="+", help="list of test suites to show")

    def process_action(self, configuration):
        tests_manager = TestsManager()

        packages_container = tests_manager.get_packages()
        packages_container.filter(packages=configuration.packages, suites=configuration.suites)

        results = self.print_packages(packages_container.packages)

        headers = ["Tests package", "Test suite name", "Test script  name"]
        print "\n"
        print tabulate(sorted(results), headers=headers)

    def print_packages(self, packages):
        results = list()

        for package in packages:
            results += self.print_package(package)

        return results

    def print_package(self, package):
        return [[package.name] + suite for suite in self.print_suites(package.suites)]

    def print_suites(self, suites):
        results = list()

        for suite in suites:
            results += self.print_suite(suite)

        return results

    def print_suite(self, suite):
        return [[suite.name] + script for script in self.print_scripts(suite.scripts)]

    def print_scripts(self, scripts):
        return [[script.name] for script in scripts]
