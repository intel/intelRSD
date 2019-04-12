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


class TestsPackagesContainer:
    def __init__(self):
        self.packages = []
        """:type : *cts_framework.tests_managing.test_package.test_package.TestPackage """

    def filter(self, packages=None, suites=None, scripts=None, script_paths=None, remove_empty=False):
        self.packages = [package for package in self.packages if packages is None or package.name in packages]

        for package in self.packages:
            package.filter(suites=suites, scripts=scripts, script_paths=script_paths, remove_empty=remove_empty)

        if remove_empty:
            self.packages = [package for package in self.packages if package.suites]

    def set_scripts_timeout(self, scripts_timeout):
        for package in self.packages:
            package.set_scripts_timeout(scripts_timeout)

    def __iadd__(self, other):
        packages_self = {package.name: package for package in self.packages}
        packages_other = {package.name: package for package in other.packages}

        for name, package in packages_other.iteritems():
            if name not in packages_self.keys():
                self.packages.append(package)
            else:
                packages_self[name] += package

        return self
