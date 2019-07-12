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
from re import match
from os import environ
import sys

from cts_core.commons.error import cts_error
from cts_core.commons.replay_controller import ReplayController
from cts_framework.actions.action import Action
from cts_framework.actions.execute.execute_test_scripts_action import ExecuteTestScriptsAction
from cts_framework.build_information import BuildInformation
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.commons.logging_helper import LoggingHelper
from cts_framework.db.dao.script_dao import ScriptDAO
from cts_framework.tests_managing.test_package.tests_packages_container import TestsPackagesContainer
from cts_framework.tests_managing.tests_manager import TestsManager
from cts_framework.tests_running.execution_feed import ExecutionFeed


def split_replay_id(replay_id):
    """converts replay_id provided by the user into script execution id
    :type replay_id: str
    :rtype: (Boolean, int)
    """

    m = match(r"^(\d+)$", replay_id.strip())
    if m:
        return None, int(m.groups()[0])

    cts_error("Replay id has invalid format. Expected: unsigned integer")
    return True, None


class ReplayTestRunAction(Action):
    ACTION = "replay"
    PARAM_NAME = "ACTION"

    def __init__(self, *params, **kwargs):
        Action.__init__(self, *params, **kwargs)

        self._logger = LoggingHelper(__name__)

    def fill_parser_arguments(self):
        self.parser.add_argument("replay_id", help="ID of the test script run to replay", type=str, nargs=1)

    def process_action(self, configuration):
        replay_id = configuration.replay_id[0]
        print "Using CTS in version %s to replay execution %s" \
              % (ColorPrinter.format_text(BuildInformation.get_version(), bold=True), replay_id)

        error, script_execution_id = split_replay_id(replay_id)
        if error:
            return

        # TODO: warn user when he tries to replay using newer CTS

        script_execution = ScriptDAO.get_script_execution_details(script_execution_id)
        if script_execution is None:
            cts_error("Recording for script execution id={id:ignore} not found", id=script_execution_id)
            return

        script_path = script_execution.script_path
        configuration = self._configuration_from_string(script_execution.configuration)

        test_plan = self._prepare_test_plan(script_path)

        environ[ReplayController.CTS_REPLAY_SCRIPT_EXECUTION_ID] = str(script_execution_id)
        self._execute(configuration, test_plan)

    def _configuration_from_string(self, configuration_str):
        configuration = {b[0]: b[1] for b in
                         (a.strip().split(' ', 1) for a in filter(None, configuration_str.split('--')))}
        return configuration

    def _prepare_test_plan(self, script_path):
        test_plan = TestsPackagesContainer()
        tests_manager = TestsManager()
        test_scripts_found = tests_manager.get_packages()
        test_scripts_found.filter(script_paths=[script_path], remove_empty=True)
        test_plan += test_scripts_found
        if not test_plan.packages:
            print "Script to execute not found in any package"
            sys.exit(0)
        return test_plan

    def _execute(self, configuration, test_plan):
        """
        :type configuration: dict
        :type test_plan: cts_framework.tests_managing.test_package.tests_packages_container.TestsPackagesContainer
        """
        message = "Executing "
        print "Executing:"
        for package in test_plan.packages:
            for suite in package.suites:
                for script in suite.scripts:
                    print "\t* %s from suite %s from package %s" % (script.name, suite.name, package.name)
                    message += "%s from suite %s from package %s, " % (script.name, suite.name, package.name)
        self._logger.log_debug(message)
        execution_feed = ExecutionFeed(test_plan, configuration)
        ExecuteTestScriptsAction.execute_configuration_group(execution_feed)
