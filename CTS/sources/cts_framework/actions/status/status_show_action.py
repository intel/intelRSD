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
from cts_framework.actions.status.results_formatters.text_formatter import TextFormatter
from cts_framework.commons.enums import LoggingLevel
from cts_framework.db.dao.run_dao import RunDAO


class StatusShowAction(Action):
    ACTION = "show"
    PARAM_NAME = "STATUS_TYPE"

    def fill_parser_arguments(self):
        self.parser.add_argument("test_run_ids", type=str, nargs="*", default=None,
                                 help="IDs of test runs that results will be presented")

        self.parser.add_argument("--turn_off_messages", default=False, action="store_true",
                                 help="Turn off showing messages")

        self.parser.add_argument("-d", "--details", type=int, default=2, help="Level of details" + str(
            LoggingLevel.LEVEL))

    def process_action(self, configuration):
        if configuration.details < 0 or configuration.details >= len(LoggingLevel.LEVEL):
            print "Incorrect message level. Allowed levels: {allowed}".format(
                allowed=", ".join(map(str, xrange(len(LoggingLevel.LEVEL)))))
            return

        test_runs = RunDAO.list_all_runs()
        run_ids = configuration.test_run_ids
        if len(run_ids) is 0:
            try:
                print "Showing last one: %s" % test_runs[-1].id
                run_ids.append(test_runs[-1].id)
            except IndexError:
                print "There are no tests to display"

        text_formatter = TextFormatter(show_messages=(not configuration.turn_off_messages),
                                       message_level=configuration.details)

        for run_id in run_ids:
            try:
                text_formatter.print_results(run_id)
            except AttributeError:
                print "results for test run #%s not found" % run_id
