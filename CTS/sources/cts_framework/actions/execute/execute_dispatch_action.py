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

from cts_framework.actions.action import Action
from cts_framework.actions.execute.execute_running_list_action import ExecuteRunningListAction

from cts_framework.actions.execute.execute_test_scripts_action import ExecuteTestScriptsAction
from cts_framework.commons.logging_helper import LoggingHelper


class ExecuteDispatchAction(Action):
    ACTION = "execute"
    PARAM_NAME = "ACTION"

    def __init__(self, *params, **kwargs):
        self.execute_running_list_action = None
        self.execute_test_scripts_action = None
        self._logger = LoggingHelper(__name__)

        Action.__init__(self, *params, **kwargs)

    def fill_parser_arguments(self):
        parsers = self.parser.add_subparsers()
        self.execute_running_list_action = ExecuteRunningListAction(parsers)
        self.execute_test_scripts_action = ExecuteTestScriptsAction(parsers)

    def process_action(self, configuration):
        self._logger.log_debug("Processing user request")

        {ExecuteRunningListAction.ACTION: self.execute_running_list_action,
         ExecuteTestScriptsAction.ACTION: self.execute_test_scripts_action}[configuration.EXECUTE_TYPE]\
            .process_action(configuration)
