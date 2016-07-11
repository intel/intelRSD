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

import glob
import imp
import inspect
import warnings

from cts_framework.tests_helpers import TestCase as TestCaseTemplate
from cts_framework.tests_managing.test_cases_container import TestCasesContainer, TestCase

CTS_DATA_PATH = "/opt/vts/"
TESTS_DIRECTORY_NAME = "tests"


class TestsManager:
    def __init__(self):
        pass

    def read_tests_information(self):
        """
        :rtype: cts_core.framework.tests_managing.test_cases_container.TestCasesContainer
        :return: container with all found test cases
        """
        test_cases_container = TestCasesContainer()
        tests_packages_names_list = [test_package_name.split("/")[-1] for test_package_name in
                                     glob.glob("%s/%s/*" % (CTS_DATA_PATH, TESTS_DIRECTORY_NAME))]


        for tests_package_name in tests_packages_names_list:
            test_cases_container += self.read_test_package(tests_package_name)

        return test_cases_container

    def read_test_package(self, tests_package_name):
        """
        :type tests_package_name: str
        :param tests_package_name: name of test package to discover
        :return: test cases container containing all discovered test cases in tests package
        :rtype: cts_core.framework.tests_managing.test_cases_container.TestCasesContainer
        """
        test_cases_container = TestCasesContainer()
        tests_scripts_paths_list = glob.glob("%s%s/%s/*/*.py" % (CTS_DATA_PATH, TESTS_DIRECTORY_NAME,
                                                                 tests_package_name))

        for tests_script_path in tests_scripts_paths_list:
            tests_suite_name, tests_script_name = tests_script_path.split("/")[-2:]

            module_name = "_".join([tests_suite_name, tests_script_path])
            try:
                test_module = self._load_module_silently(module_name, tests_script_path)
            except ImportError as err:
                print "Module %s has unmet dependencies: %s" % (tests_script_path, err.message)
                continue
            except BaseException as err:
                print "During test script %s loading unexpected error occurred %s " % (tests_script_path, err.message)
                continue

            test_cases_classes_list = self._get_test_case_classes_from_module(test_module)
            for test_case_class in test_cases_classes_list:
                test_case = TestCase()

                test_case.tests_package_name = tests_package_name
                test_case.tests_suite_name = tests_suite_name
                test_case.test_script_path = tests_script_path
                test_case.test_script_name = tests_script_name
                test_case.name = test_case_class.__name__
                test_case.test_case_class = test_case_class
                test_case.description = test_case_class.DESCRIPTION
                test_case.parameters = test_case_class.CONFIGURATION_PARAMETERS
                test_case.timeout = test_case_class.TIMEOUT

                test_cases_container.add_test_case(test_case)

        return test_cases_container

    def _get_test_case_classes_from_module(self, module):
        """
        :type module: module
        :param module: loaded test script as module
        :return: list of loaded test case classes
        :rtype: list[cts_framework.tests_helpers.test_case.TestCase]
        """
        test_cases = list()
        classes_in_test_file = inspect.getmembers(module, inspect.isclass)
        for test_class_name, test_class in classes_in_test_file:
            if not test_class.__module__ == module.__name__:
                # class defined in different module - was only imported
                continue

            if not issubclass(test_class, TestCaseTemplate):
                # class is not test case
                continue

            test_cases.append(test_class)

        return test_cases

    def _load_module_silently(self, module_name, module_path):
        with warnings.catch_warnings():
            warnings.simplefilter("ignore")
            return imp.load_source(module_name, module_path)
