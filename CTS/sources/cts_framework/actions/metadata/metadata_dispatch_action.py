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
from cts_framework.actions.metadata.metadata_diff_action import MetadataDiffAction
from cts_framework.actions.metadata.metadata_report_action import MetadataReportAction
from cts_framework.commons.logging_helper import LoggingHelper


class MetadataDispatchAction(Action):
    ACTION = "metadata"
    PARAM_NAME = "ACTION"

    def __init__(self, *params, **kwargs):
        self.metadata_diff_action, self.metadata_report_action = None, None

        self._logger = LoggingHelper(__name__)
        Action.__init__(self, *params, **kwargs)

    def fill_parser_arguments(self):
        parsers = self.parser.add_subparsers()
        self.metadata_diff_action = MetadataDiffAction(parsers)
        self.metadata_report_action = MetadataReportAction(parsers)

    def process_action(self, configuration):
        self._logger.log_debug("Processing user request")

        {MetadataDiffAction.ACTION: self.metadata_diff_action,
         MetadataReportAction.ACTION: self.metadata_report_action}[configuration.EXECUTE_TYPE]\
            .process_action(configuration)
