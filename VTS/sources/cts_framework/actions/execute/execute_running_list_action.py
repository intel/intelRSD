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

import ConfigParser
from collections import defaultdict

from cts_framework.actions.action import Action
from cts_framework.configuration.config_property_reader import ConfigPropertyReader
from cts_framework.tests_running.test_run import TestRun

from cts_framework.tests_managing.tests_manager import TestsManager


class ExecuteRunningListAction(Action):
    ACTION = "run_list"
    PARAM_NAME = "EXECUTE_TYPE"

    def fill_parser_arguments(self):
        self.parser.add_argument("-t", "--test_run_list", type=str, nargs=1, required=True)
        self.parser.add_argument("-g", "--debug", default=False, action="store_true",
                                 help="Enables debugging mode which will stop tests execution after first failed test")

    def process_action(self, configuration):
        running_list = ConfigParser.ConfigParser()
        running_list.optionxform = str
        running_list.read(configuration.test_run_list)

        test_run = TestRun()

        for running_list_section in running_list.sections():
            self.process_running_list_section(running_list_section, running_list, test_run)

        test_run.register_run()
        test_run.run_all(debug_mode=configuration.debug)

    def process_running_list_section(self, running_list_section, running_list, test_run):
        tests_manager = TestsManager()
        test_cases_container = tests_manager.read_tests_information()

        test_package = running_list.get(running_list_section, "TEST_PACKAGE")
        try:
            test_suites = running_list.get(running_list_section, "TEST_SUITES")
        except ConfigParser.NoOptionError:
            test_suites = None

        try:
            test_cases = running_list.get(running_list_section, "TEST_CASES")
        except ConfigParser.NoOptionError:
            test_cases = None

        configuration_files = running_list.get(running_list_section, "TEST_CONFIGS").split(',')
        test_cases_configuration = ConfigPropertyReader(**self.get_configuration_from_file(configuration_files[0]))
        for configuration_file in configuration_files:
            test_cases_configuration.update_configuration(**self.get_configuration_from_file(configuration_file))

        requested_test_cases = list()

        if test_suites or test_cases:
            if test_suites:
                requested_test_cases += test_cases_container.filter_test_cases(packages_names=[test_package],
                                                                               suites_names=test_suites)
            if test_cases:
                requested_test_cases += test_cases_container.filter_test_cases(packages_names=[test_package],
                                                                               test_cases_names=test_cases)
        else:
            requested_test_cases = test_cases_container.filter_test_cases(packages_names=[test_package])

        test_run.add_tests_configuration_group(requested_test_cases, test_cases_configuration)

    def get_configuration_from_file(self, configuration_file):
        config = defaultdict(dict)

        file_config = ConfigParser.ConfigParser()
        file_config.optionxform = str
        file_config.read(configuration_file)

        for section_name in file_config.sections():
            for param_name, param_value in file_config.items(section_name):
                config[param_name] = param_value

        return config
