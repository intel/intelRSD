"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
from subprocess import Popen, PIPE

from include.libs.xmltodict.xmltodict import parse

from include.common.exit_codes import EXIT_CODE_IO_ERROR, EXIT_CODE_NO_SUCH_FILE_OR_DIRECTORY, EXIT_CODE_UNKNOWN_ERROR


class ShellCommand:
    def __init__(self, command, params=[]):
        self._command = command
        self._params = params
        # results of command
        self._out = None
        self._err = None
        self._code = None

    def execute(self):

        try:
            self._out = ""
            self._err = ""
            proc = Popen([self._command] + self._params, stdout=PIPE, stderr=PIPE, shell=True)
            self._out, self._err = proc.communicate()
            self._code = proc.returncode
        except OSError:
            # file does not exist
            self._code = EXIT_CODE_NO_SUCH_FILE_OR_DIRECTORY
        except ValueError:
            # wrong parameters type
            self._code = EXIT_CODE_IO_ERROR
        except Exception:
            self._code = EXIT_CODE_UNKNOWN_ERROR

    def get_stdout(self):
        return self._out

    def get_stderr(self):
        return self._err

    def get_exitcode(self):
        return self._code

    def parse_output_xml_to_dict(self):
        return parse(self.get_stdout())
