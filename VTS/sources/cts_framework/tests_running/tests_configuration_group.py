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
from cts_framework.configuration.config_property_reader import ConfigPropertyReader, ValueNotFound
from cts_framework.tests_helpers import ResultStatus
from cts_framework.tests_running.single_test_runner import SingleTestRunner


class TestsConfigurationGroup:
    def __init__(self, tests, tests_configuration):
        """
        :type tests: list[cts_framework.tests_managing.test_cases_container.TestCase]
        :type tests_configuration: cts_framework.configuration.config_property_reader.ConfigPropertyReader
        :param tests: list of tests to execute
        :param tests_configuration: configuration for tests in list
        """
        self.tests = list()
        for test_case in tests:
            try:
                self.tests.append(test_case.test_case_class(test_case,
                                                            self.parse_test_case_configuration(test_case,
                                                                                               tests_configuration)))
            except BaseException as err:
                print "Unhandled exception %s while creating test case object %s, test case will not be executed" % \
                      (err.message, test_case.test_script_path)

    def parse_test_case_configuration(self, test_case, tests_configuration):
        """
        :type test_case: cts_framework.tests_managing.test_cases_container.TestCase
        :param tests_configuration: cts_framework.configuration.config_property_reader.ConfigPropertyReader
        :rtype: cts_framework.configuration.config_property_reader.ConfigPropertyReader
        """
        test_configuration = ConfigPropertyReader()
        for configuration_parameter in test_case.parameters:
            try:
                test_configuration.update_configuration(**{configuration_parameter.name:
                                                               tests_configuration[configuration_parameter.name]})
            except ValueNotFound:
                if configuration_parameter.default_value is not None:
                    test_configuration.update_configuration(**{configuration_parameter.name:
                                                                   configuration_parameter.default_value})

        return test_configuration

    def register_run(self, tests_run_id):
        """
        :param tests_run_id: int
        :param tests_run_id: id of tests run
        :return: None
        """
        for test_case in self.tests:
            test_case.register_test_execution(tests_run_id)

    def run_all(self, debug_mode=False):
        mark_as_blocked = False
        for test_case in self.tests:
            if mark_as_blocked:
                # one of previous tests failed, so rest is marked as blocked
                test_case.set_status(ResultStatus.BLOCKED)
                continue

            if not SingleTestRunner(test_case).run() and debug_mode:
                # this tests failed, so all next tests shall fail
                mark_as_blocked = True

        return not mark_as_blocked

    def mark_as_blocked(self):
        for test_case in self.tests:
            test_case.set_status(ResultStatus.BLOCKED)
