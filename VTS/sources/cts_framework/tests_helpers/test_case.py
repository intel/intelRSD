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

from abc import abstractmethod

from cts_framework.db.dao.test_case_results_dao import TestCaseResultsDAO
from cts_framework.tests_helpers.result_status import ResultStatus


class TestCase:
    """
    base test case class
    """
    TIMEOUT = 60
    """:type: int"""
    DESCRIPTION = "Not specified"
    """:type: str"""
    CONFIGURATION_PARAMETERS = []
    """:type: list[TestCase.ConfigurationParameter]"""

    def __init__(self, test_case_details, configuration):
        """
        :type test_case_details: cts_framework.tests_managing.test_cases_container.TestCase
        :type configuration: cts_framework.configuration.config_property_reader.ConfigPropertyReader
        :param test_case_details: test case details
        :param configuration:  test case configuration
        """
        self.test_case_result_id = None
        self.test_case_result_dao = TestCaseResultsDAO()
        self.test_case_details = test_case_details

        self.configuration = configuration
        self.files_to_save = dict()

    @abstractmethod
    def run(self):
        self.set_status(ResultStatus.UNKNOWN)

    def before_run(self):
        pass

    def after_run(self):
        pass

    def register_test_execution(self, test_run_id):
        test_case_name = "%s.%s" % (self.test_case_details.tests_suite_name, self.test_case_details.name)
        self.test_case_result_id = self.test_case_result_dao.register_test_case_execution(test_run_id,
                                                                                          ResultStatus.WAITING,
                                                                                          test_case_name)

    def set_status_failed(self):
        self.set_status(ResultStatus.FAILED)

    def set_status_passed(self):
        self.set_status(ResultStatus.PASSED)

    def get_status(self):
        return self.test_case_result_dao.get_test_case_status(self.test_case_result_id)

    def set_status(self, status):
        if not self.test_case_result_id:
            raise RuntimeError("Test case not registered.")

        TestCaseResultsDAO().set_test_execution_status(self.test_case_result_id, status)

    def log_file(self, file_content, file_name):
        self.files_to_save[file_name] = file_content

    class ConfigurationParameter:
        def __init__(self, parameter_name, parameter_description, parameter_type, is_required, parameter_default_value):
            self.name = parameter_name
            self.description = parameter_description
            self.type = parameter_type
            self.is_required = is_required
            self.default_value = parameter_default_value
