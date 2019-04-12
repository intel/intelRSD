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

from itertools import chain

from cts_framework.tests_running.test_script_runner import TestScriptRunner


class ExecutionFeed:
    """Bundle of test selection and configuration"""
    def __init__(self, packages_container, configuration):
        """
        :type packages_container:  cts_framework.tests_managing.test_package.tests_packages_container.TestsPackagesContainer
        :type configuration: dict
        """
        self.packages_container = packages_container
        self.configuration = configuration

        packages = packages_container.packages
        suites = list(chain(*[package.suites for package in packages]))
        scripts = list(chain(*[suite.scripts for suite in suites]))
        self.script_runners = [TestScriptRunner(script, self.configuration) for script in scripts]

    def register_all(self, test_run_id):
        for script_runner in self.script_runners:
            script_runner.register_execution(test_run_id)

    def run_all(self):
        status = True
        for script_runner in self.script_runners:
            status = script_runner.execute() and status

        return status
