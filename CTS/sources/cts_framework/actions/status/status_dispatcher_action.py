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
from cts_framework.actions.status.status_delete_action import StatusDeleteAction
from cts_framework.actions.status.status_dump_action import StatusDumpAction
from cts_framework.actions.status.status_list_action import StatusListAction
from cts_framework.actions.status.status_show_action import StatusShowAction
from cts_framework.commons.logging_helper import LoggingHelper


class StatusDispatchAction(Action):
    ACTION = "status"
    PARAM_NAME = "ACTION"

    def __init__(self, *params, **kwargs):
        self._logger = LoggingHelper(__name__)

        Action.__init__(self, *params, **kwargs)

    def fill_parser_arguments(self):
        parsers = self.parser.add_subparsers()
        self.status_list_action = StatusListAction(parsers)
        self.status_show_action = StatusShowAction(parsers)
        self.status_delete_action = StatusDeleteAction(parsers)
        self.status_dump_action = StatusDumpAction(parsers)

    def process_action(self, configuration):
        self._logger.log_debug("Processing user request")

        {StatusListAction.ACTION: self.status_list_action, StatusShowAction.ACTION: self.status_show_action,
         StatusDeleteAction.ACTION: self.status_delete_action, StatusDumpAction.ACTION: self.status_dump_action}[
            configuration.STATUS_TYPE].process_action(configuration)
