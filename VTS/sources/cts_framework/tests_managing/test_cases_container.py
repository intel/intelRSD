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


class TestCase:
    def __init__(self):
        self.tests_package_name = None
        self.tests_suite_name = None
        self.test_script_path = None
        self.test_script_name = None
        self.test_case_class = None
        self.name = None
        self.description = None
        self.timeout = None
        self.parameters = None
        """:type: list[cts_framework.tests_helpers.test_case.TestCase.ConfigurationParameter]"""


class TestCasesContainer:
    def __init__(self, test_cases=None):
        """
        :type test_cases: list[TestCase]
        :param test_cases: list of test cases
        """
        if test_cases is None:
            test_cases = []

        self._test_cases = test_cases

    def add_test_case(self, test_case):
        """
        :type test_case: TestCase
        :param test_case: test case to add
        """
        self._test_cases.append(test_case)

    def filter_test_cases(self, packages_names=None, suites_names=None, scripts_names=None, test_cases_names=None):
        """
        :type test_cases_names: list[str]
        :type scripts_names: list[str]
        :type suites_names: list[str]
        :type packages_names: list[str]
        :param packages_names: packages names list to filter
        :param suites_names: suites names list to filter
        :param scripts_names: test scripts names list to filter
        :param test_cases_names: test cases names names list to filter
        :rtype: list[TestCase]
        """
        filtered_test_cases = self._test_cases

        for attribute, possible_values in [("tests_package_name", packages_names), ("tests_suite_name", suites_names),
                                           ("test_script_name", scripts_names), ("name", test_cases_names)]:
            filtered_test_cases = self._filter_test_cases_by_single_attribute(filtered_test_cases, attribute,
                                                                              possible_values)

        return filtered_test_cases

    def _filter_test_cases_by_single_attribute(self, test_cases_list, attribute, possible_values_list):
        if not possible_values_list:
            return test_cases_list

        return [test_case for test_case in test_cases_list if getattr(test_case, attribute) in possible_values_list]

    def __iadd__(self, other):
        """
        :type other: TestCasesContainer
        :param other: test cases container to merge
        """
        self._test_cases += other._test_cases
        return self
