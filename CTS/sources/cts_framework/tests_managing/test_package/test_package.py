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


class TestPackage:
    def __init__(self):
        self.name = ""
        self.path = ""
        self.suites = []

    def filter(self, suites=None, scripts=None, script_paths=None, remove_empty=False):
        self.suites = [suite for suite in self.suites if suites is None or suite.name in suites]

        for suite in self.suites:
            suite.filter(scripts=scripts, script_paths=script_paths)

        if remove_empty:
            self.suites = [suite for suite in self.suites if suite.scripts]

    def set_scripts_timeout(self, scripts_timeout):
        for suite in self.suites:
            suite.set_scripts_timeout(scripts_timeout)

    def __iadd__(self, other):
        suites_self = {suite.name: suite for suite in self.suites}
        suites_other = {suite.name: suite for suite in other.suites}

        for name, suite in suites_other.iteritems():
            if name not in suites_self.keys():
                self.suites.append(suite)
            else:
                suites_self[name] += suite

        return self
