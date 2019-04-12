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

from copy import deepcopy
from cts_framework.tests_managing.test_package.test_package_manager import TestPackageManager
from cts_framework.tests_managing.test_package.tests_packages_container import TestsPackagesContainer
from cts_framework.helpers.vars.env_consts import Constants

TESTS_PATH = Constants.TESTS_PATH


class TestsManager:
    def __init__(self):
        self.test_package_manager = TestPackageManager()
        self._package_container = None

    def _prepare_packages(self, packages):
        packages_list = []
        for package in packages:
            packages_list.extend(glob.glob(os.path.join(*package)))

        return [x for x in packages_list if os.path.isdir(x)]

    def get_packages(self):
        if not self._package_container:
            test_package_container = TestsPackagesContainer()

            test_packages = self._prepare_packages(TESTS_PATH)
            for test_package in test_packages:
                test_package_container.packages.append(self.test_package_manager.analyze_test_package(test_package))

            self._package_container = test_package_container

        return deepcopy(self._package_container)
