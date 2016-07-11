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
from cts_framework.db.dao.test_run_dao import TestRunDAO
from cts_framework.tests_running.tests_configuration_group import TestsConfigurationGroup


class TestRun:
    def __init__(self):
        self.tests_configurations = []
        self.test_run_dao = TestRunDAO()

    def add_tests_configuration_group(self, tests, tests_configuration):
        """
        :type tests: list[cts_core.framework.tests_managing.test_cases_container.TestCase]
        :param tests: tests group to execute
        :param tests_configuration: configuration for tests group
        :return:
        """
        self.tests_configurations.append(TestsConfigurationGroup(tests, tests_configuration))

    def register_run(self):
        test_run_id = self.test_run_dao.create_test_run("TODO: read comment from cli")

        print "Test run id: %s" % (test_run_id)

        for tests_configuration in self.tests_configurations:
            tests_configuration.register_run(test_run_id)

    def run_all(self, debug_mode=False):
        mark_as_blocked = False

        for tests_configuration in self.tests_configurations:
            if mark_as_blocked:
                tests_configuration.mark_as_blocked()
                continue

            group_execution_status = tests_configuration.run_all(debug_mode=debug_mode)
            if not group_execution_status and debug_mode:
                mark_as_blocked = True
