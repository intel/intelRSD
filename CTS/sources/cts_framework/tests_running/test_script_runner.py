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

import shlex
import subprocess
import atexit
import time
from cts_framework.db.dao.script_dao import ScriptDAO
from cts_framework.logging.logging_stream import LoggingStream
from cts_framework.tests_running.execution_exceptions import CommandTimedOut, CommandFailed


class TestScriptRunner:
    def __init__(self, test_script, configuration):
        """
        :type test_script: cts_framework.tests_managing.test_script.test_script.TestScript
        """
        self.test_script = test_script

        self.configuration = ""
        for configuration_parameter in self.test_script.parameters:
            try:
                self.configuration += "--%s %s " % (configuration_parameter.name,
                                                    configuration[configuration_parameter.name])
            except KeyError:
                if configuration_parameter.is_required:
                    # TODO: mark test as blocked
                    pass
                else:
                    pass

        self.test_run_id = None
        self.test_script_execution_id = None

    def register_execution(self, test_run_id):
        self.test_run_id = test_run_id
        self.test_script_execution_id = ScriptDAO.register_script_execution(self.test_script.path, test_run_id,
                                                                                       self.configuration,
                                                                                       self.test_script.control_sum)

    def execute(self):
        if self.test_run_id is None or self.test_script_execution_id is None:
            raise RuntimeError("Execution not registered test script is not allowed")

        if self.test_script.timeout is not None:
            timeout_command = "timeout %s " % self.test_script.timeout
        else:
            timeout_command = ""
        execution_command = "%s%s execute %s" % (timeout_command, self.test_script.path, self.configuration)

        logging_stream = LoggingStream(self.test_run_id, self.test_script_execution_id, self.test_script)
        logging_stream.enable_stream()

        try:
            self._execute_test_script(execution_command, logging_stream)
        except CommandTimedOut:
            logging_stream.raise_timeout()
        except CommandFailed:
            logging_stream.raise_failed()

        logging_stream.disable_stream(configuration=self.configuration)

        return logging_stream.get_overall_execution_status()

    def _execute_test_script(self, execution_command, logging_stream):
        args = shlex.split(execution_command)
        global process
        process = subprocess.Popen(args, stdout=subprocess.PIPE, close_fds=True)
        atexit.register(stop_script)

        while True:
            message = process.stdout.readline()

            if not message and process.poll() is not None:
                if process.poll() == 124:
                    # timeout
                    raise CommandTimedOut()
                elif process.poll():
                    # other non zero exit code
                    raise CommandFailed()
                else:
                    break
            if message:
                logging_stream.write(message.strip())


def stop_script():
    global process

    for i in range(2):
        if process.poll() == None:
            time.sleep(1)
        else:
            return
    process.kill()
