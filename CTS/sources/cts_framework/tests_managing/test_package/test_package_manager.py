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

import glob
import os


from cts_framework.tests_managing.test_package.test_package import TestPackage
from cts_framework.tests_managing.test_suite.test_suite_manager import TestSuiteManager


class TestPackageManager:
    def __init__(self):
        self.test_suite_manager = TestSuiteManager()

    def analyze_test_package(self, package_path):
        test_package = TestPackage()

        test_package.path = package_path
        test_package.name = package_path.split(os.path.sep)[-1]
        test_suites_paths_list = glob.glob("%s%s*" % (package_path, os.path.sep))
        test_package.suites = [self.test_suite_manager.analyze_test_suite(test_suite_path) for test_suite_path in
                               test_suites_paths_list]

        return test_package
