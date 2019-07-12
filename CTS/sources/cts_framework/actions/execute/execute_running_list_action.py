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

import ConfigParser

import sys

from cts_framework.actions.action import Action
from cts_framework.build_information import BuildInformation
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.commons.logging_helper import LoggingHelper
from cts_framework.tests_managing.test_package.tests_packages_container import TestsPackagesContainer
from cts_framework.tests_managing.tests_manager import TestsManager
from cts_framework.tests_running.execution_feed import ExecutionFeed
from cts_framework.tests_running.test_run import TestRun
from cts_framework.commons.config_reader import get_configuration_from_files


class ExecuteRunningListAction(Action):
    def __init__(self, *params, **kwargs):
        self._logger = LoggingHelper(__name__)
        self.tests_manager = TestsManager()

        Action.__init__(self, *params, **kwargs)

    ACTION = "run_list"
    PARAM_NAME = "EXECUTE_TYPE"

    def fill_parser_arguments(self):
        self.parser.add_argument("test_run_list", type=str, nargs=1)
        self.parser.add_argument("-g", "--debug", default=False, action="store_true",
                                 help="Enables debugging mode which will stop tests execution after first failed test")
        self.parser.add_argument("-x", "--return_tests_status_as_exit_code", action="store_true", default=False,
                                 help="When flag set CTS framework will return calculate exit code basing on tests results.")

    def process_action(self, configuration):
        print "Using CTS in version %s" % (ColorPrinter.format_text(BuildInformation.get_version(), bold=True))

        running_list = ConfigParser.ConfigParser()
        running_list.optionxform = str
        running_list.read(configuration.test_run_list)

        test_run = TestRun()

        for running_list_section in running_list.sections():
            self.process_running_list_section(running_list_section, running_list, test_run)

        test_run.register_all()

        execution_status = test_run.run_all()

        if not execution_status and configuration.return_tests_status_as_exit_code:
            print "Tests finished with status failed, exit code set to 4"
            exit(4)

    def process_running_list_section(self, running_list_section, running_list, test_run):
        test_package_name = running_list.get(running_list_section, "TEST_PACKAGE")
        try:
            requested_test_suites = running_list.get(running_list_section, "TEST_SUITES")
        except ConfigParser.NoOptionError:
            requested_test_suites = None

        try:
            requested_test_scripts = running_list.get(running_list_section, "TEST_SCRIPTS")
        except ConfigParser.NoOptionError:
            requested_test_scripts = None

        configuration_files = [file_name.strip() for file_name in running_list.get(running_list_section, "TEST_CONFIGS").split(',')]
        test_cases_configuration = get_configuration_from_files(configuration_files)
        if test_cases_configuration is None:
            sys.exit(-1)

        test_plan = TestsPackagesContainer()

        if requested_test_suites or requested_test_scripts:
            if requested_test_suites:
                test_suites_found = self.tests_manager.get_packages()
                test_suites_found.filter(packages=[test_package_name], suites=requested_test_suites, remove_empty=True)
                test_plan += test_suites_found
            if requested_test_scripts:
                test_scripts_found = self.tests_manager.get_packages()

                test_scripts_found.filter(packages=[test_package_name], scripts=requested_test_scripts,
                                          remove_empty=True)
                test_plan += test_scripts_found
        else:
            test_plan += self.tests_manager.get_packages()
            test_plan.filter(packages=[test_package_name])

        test_run.add_execution_feed(ExecutionFeed(test_plan, test_cases_configuration))
