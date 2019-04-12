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


class TestSuite:
    def __init__(self):
        self.name = ""
        self.path = ""
        self.scripts = []

    def _trim_path(self, path):
        try:
            return path.rsplit('/tests/', 1)[1]
        except:
            return path

    def filter(self, scripts=None, script_paths=None):
        if script_paths is not None:
            script_paths = [self._trim_path(path) for path in script_paths]
            self.scripts = [script for script in self.scripts if self._trim_path(script.path) in script_paths]
            return

        self.scripts = [script for script in self.scripts if scripts is None or script.name in scripts]

    def set_scripts_timeout(self, scripts_timeout):
        for script in self.scripts:
            script.timeout = scripts_timeout

    def __iadd__(self, other):
        scripts_self = {script.name: script for script in self.scripts}
        scripts_other = {script.name: script for script in other.scripts}

        for name, script in scripts_other.iteritems():
            if name not in scripts_self.keys():
                self.scripts.append(script)

        return self
