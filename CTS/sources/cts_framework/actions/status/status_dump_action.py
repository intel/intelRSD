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

from sys import stderr

from cts_framework.actions.action import Action
from cts_framework.actions.status.results_formatters.csv_formatter import CsvFormatter
from cts_framework.actions.status.results_formatters.html_formatter import HtmlFormatter
from cts_framework.actions.status.results_formatters.text_formatter import TextFormatter
from cts_framework.db.dao.run_dao import RunDAO
from cts_framework.commons.enums import LoggingLevel


HTML_FORMAT = "html"
TEXT_FORMAT = "text"
CSV_FORMAT = "csv"


class StatusDumpAction(Action):
    ACTION = "dump"
    PARAM_NAME = "STATUS_TYPE"

    @staticmethod
    def _default():
        return [TEXT_FORMAT]

    def fill_parser_arguments(self):
        self.parser.add_argument("test_run_ids", type=str, nargs='*', default=None,
                                 help="IDs of test run that results will be dumped")
        self.parser.add_argument("-o", "--output_format", type=str, nargs=1, required=False, default=[TEXT_FORMAT],
                                 choices=[HTML_FORMAT, TEXT_FORMAT, CSV_FORMAT])
        self.parser.add_argument("-a", "--all", default=False, action="store_true",
                                 help="Dump all logs from tests to text file")
        self.parser.add_argument("-d", "--details", type=int, default=2, help="Level of details" + str(
            LoggingLevel.LEVEL))

    def process_action(self, configuration):
        test_runs = RunDAO.list_all_runs()
        if configuration.details < 0 or configuration.details >= len(LoggingLevel.LEVEL) or configuration.details is None:
            print "Incorrect message level. Allowed levels: {allowed}.".format(
                allowed=", ".join(map(str, xrange(len(LoggingLevel.LEVEL)))))
            return

        if configuration.all:
            if len(test_runs) is 0:
                print "There are no tests to display"
            for test_run in test_runs:
                self.create_dump(test_run.id, configuration.output_format[0], configuration.details)
        else:
            if not len(configuration.test_run_ids):
                stderr.write("No execution IDs given\n")
                try:
                    stderr.write("Dumping last one: %s" % test_runs[-1].id)
                    self.create_dump(test_runs[-1].id, configuration.output_format[0], configuration.details)
                except IndexError:
                    stderr.write("There are no tests to dump")
            else:
                for run_id in configuration.test_run_ids:
                    self.create_dump(run_id, configuration.output_format[0], configuration.details)



    @staticmethod
    def create_dump(run_id, output_format, message_level=None):
        if output_format == HTML_FORMAT:
            HtmlFormatter().save_results(run_id)
        else:
            try:
                if output_format == CSV_FORMAT:
                    CsvFormatter().save_results(run_id)
                elif output_format == TEXT_FORMAT:
                    TextFormatter(message_level=message_level).save_results(run_id)
            except AttributeError:
                print "Results for test run #%s not found" % run_id
