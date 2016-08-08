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


class ExecuteTestScriptsAction(Action):
    ACTION = "tests"
    PARAM_NAME = "EXECUTE_TYPE"

    def fill_parser_arguments(self):
        self.parser.add_argument("-p", "--package", help="Test package to execute", type=str,
                                                      required=True)
        self.parser.add_argument("-s", "--test_suites", type=str, nargs="+",
                                                      help="Test suites to execute")

        self.parser.add_argument("-t", "--test_cases", type=str, nargs="+",
                                                      help="Test cases to execute")

        self.parser.add_argument("-c", "--config_files", help="Configuration files for test cases",
                                                      required=True, nargs="+")
        self.parser.add_argument("-g", "--debug", default=False, action="store_true",
                                 help="Enables debugging mode which will stop tests execution after first failed test")

    def process_action(self, configuration):
        tests_manager = TestsManager()

        configuration_files = configuration.config_files
        test_cases_configuration = ConfigPropertyReader(**self.get_configuration_from_file(configuration_files[0]))
        for configuration_file in configuration_files:
            test_cases_configuration.update_configuration(**self.get_configuration_from_file(configuration_file))


        test_run = TestRun()

        test_cases_container = tests_manager.read_tests_information()

        requested_test_cases = list()
        if configuration.test_suites or configuration.test_cases:
            if configuration.test_suites:
                requested_test_cases += test_cases_container.filter_test_cases(packages_names=[configuration.package],
                                                                               suites_names=configuration.test_suites)
            if configuration.test_cases:
                requested_test_cases += test_cases_container.filter_test_cases(packages_names=[configuration.package],
                                                                               test_cases_names=configuration.test_cases)
        else:
            requested_test_cases = test_cases_container.filter_test_cases(packages_names=[configuration.package])

        if not requested_test_cases:
            print "Test cases not found"
            exit(1)

        test_run.add_tests_configuration_group(requested_test_cases, test_cases_configuration)

        test_run.register_run()
        test_run.run_all(debug_mode=configuration.debug)

    def get_configuration_from_file(self, configuration_file):
        config = defaultdict(dict)

        file_config = ConfigParser.ConfigParser()
        file_config.optionxform = str
        file_config.read(configuration_file)

        for section_name in file_config.sections():
            for param_name, param_value in file_config.items(section_name):
                config[param_name] = param_value

        if not config:
            print "incorrect configuration file %s" % configuration_file

        return config
