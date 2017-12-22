"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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
from os import access, path, errno, X_OK

from cts_framework.tests_managing.test_script.test_script_manager import TestScriptManager
from cts_framework.tests_managing.test_suite.test_suite import TestSuite


class TestSuiteManager:
    def __init__(self):
        self.test_script_manager = TestScriptManager()

    def analyze_test_suite(self, test_suite_path):
        """
        :type test_suite_path: str
        :return:
        """
        test_suite = TestSuite()
        test_suite.name = test_suite_path.split(path.sep)[-1]
        test_suite.path = test_suite_path
        test_scripts_paths_list = [test_script for test_script in glob.glob("%s%s*" % (test_suite_path, path.sep)) if
                                   access(test_script, X_OK)]

        test_suite.scripts = []
        for test_script_path in test_scripts_paths_list:
            try:
                test_suite.scripts.append(self.test_script_manager.analyze_test_script(test_script_path))
            except OSError as err:
                if err.errno in [errno.ENOEXEC, errno.EACCES]:
                    # script was not executable
                    print "Unable to parse %s" % test_script_path
                    continue
                else:
                    raise
        return test_suite
