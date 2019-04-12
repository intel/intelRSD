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

import errno
import hashlib
import json
import os
import sys
import shlex
import subprocess
from distutils.util import strtobool

from cts_core.commons.error import cts_error
from cts_framework.commons.digest import FileDigest
from cts_framework.tests_managing.test_script.test_script import TestScript
from cts_framework.tests_managing.test_script.test_script_parameter import TestScriptParameter
from cts_framework.helpers.vars.env_consts import Constants

DIR_HOME_CTS_TESTS_DATA = Constants.DIR_HOME_CTS_TESTS_DATA


class TestScriptManager:
    def __init__(self):
        pass

    def analyze_test_script(self, test_script_path):
        md5 = FileDigest(test_script_path)

        test_script = TestScript()
        test_script.path = test_script_path
        test_script.name = test_script_path.split(os.path.sep)[-1].split(".")[0]

        from_file = self.read_test_script_information_from_file(test_script_path)
        try:
            if from_file is not None:
                if md5.is_md5_correct(from_file.control_sum):
                    return from_file
        except KeyError:
            pass

        print "Recreating configuration cache for script %s" % test_script_path

        code, test_script.description, error = self.execute_bash_command("%s %s" % (test_script_path, "description"))
        if code != 0:
            cts_error("Error {code} while executing \n\t'{test_script_path} description'\nError: {error}", **locals())
            sys.exit(-1)

        code, test_script_parameters, error = self.execute_bash_command("%s %s" % (test_script_path, "parameters_list"))
        if code != 0:
            cts_error("Error while executing '{test_script_path} parameters_list'. Error: {error}", **locals())
            sys.exit(-1)

        test_script_parameters = test_script_parameters.strip().split(",")
        test_script.control_sum = md5.current_digest

        for test_script_parameter in test_script_parameters:
            test_script.parameters.append(self.analyze_test_script_parameter(test_script_path, test_script_parameter))

        self.save_test_script_information_to_file(test_script_path, test_script.save_to_dict())

        return test_script

    def analyze_test_script_parameter(self, test_script_path, test_script_parameter_name):
        code, test_script_parameter_details, error = self.execute_bash_command("%s %s %s" % (test_script_path,
                                                                                             "description",
                                                                                             test_script_parameter_name))
        test_script_parameter = TestScriptParameter()
        test_script_parameter.name = test_script_parameter_name

        try:
            test_script_parameter.is_required, test_script_parameter.description = test_script_parameter_details.strip().\
                split("::")
            test_script_parameter.is_required = strtobool(test_script_parameter.is_required)
        except ValueError:
            cts_error("{script} {parameter} : Unable to parse response: '{response}'",
                      script=test_script_path, parameter=test_script_parameter_name, response=test_script_parameter_details)

        return test_script_parameter

    def make_hex(self, test_script_path):
        m = hashlib.md5()
        m.update(test_script_path)
        return m.hexdigest()

    def execute_bash_command(self, command):
        args = shlex.split(command)
        process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdoutdata, stderrdata = process.communicate()

        return process.returncode, stdoutdata, stderrdata

    def read_test_script_information_from_file(self, test_script_path):
        configuration_file_path = os.path.join(DIR_HOME_CTS_TESTS_DATA, self.make_hex(test_script_path))

        try:
            with open(configuration_file_path) as configuration_file:
                return TestScript(json.loads(configuration_file.read()))
        except:
            return None

    def save_test_script_information_to_file(self, test_script_path, test_script_description):
        configuration_file_path = os.path.join(DIR_HOME_CTS_TESTS_DATA, self.make_hex(test_script_path))

        if not os.path.exists(os.path.dirname(DIR_HOME_CTS_TESTS_DATA)):
            try:
                os.makedirs(os.path.dirname(DIR_HOME_CTS_TESTS_DATA))
            except OSError as ose:
                if ose.errno != errno.EEXIST:
                    raise
        try:
            with open(configuration_file_path, "w") as configuration_file:
                configuration_file.write(json.dumps(test_script_description))
        except:
            # there'll no boost
            pass
