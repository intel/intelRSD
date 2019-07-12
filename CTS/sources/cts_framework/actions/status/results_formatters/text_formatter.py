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

import sys

from cts_core.commons.text_helpers import to_multiline
from cts_framework.actions.status.results_formatters.formatter import Formatter
from cts_framework.build_information import BuildInformation
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.commons.enums import LoggingLevel
from cts_framework.db.dao.case_dao import CaseDAO
from cts_framework.db.dao.message_dao import MessageDAO
from cts_framework.db.dao.run_dao import RunDAO
from cts_framework.db.dao.script_dao import ScriptDAO
from cts_framework.helpers.python.result_status import ResultStatus

import os.path
from os import makedirs
from os.path import exists

VERTICAL_SEPARATOR = "\n"
HORIZONTAL_SEPARATOR = "\t"
CASE_SEPARATOR = "+" + "-" * 120
EXT = ".txt"
PARAM_USER = "User"
PARAM_PASSWORD = "Password"
TESTS_REPORT = "cts_report"
TESTS_CASE = "test-report-"

FMT = [
    dict(TIMESTAMP_FMT="%H:%M:%S",
         MULTILINE_SPLIT_AT=90,
         MULTILINE_SPLIT_SEP='\n\t\t\t\t'
         ),
    dict(TIMESTAMP_FMT="%H:%M:%S",
         MULTILINE_SPLIT_AT=90,
         MULTILINE_SPLIT_SEP='\n\t\t\t\t'
         ),
    dict(TIMESTAMP_FMT="%Y-%m-%d %H:%M:%S",
         MULTILINE_SPLIT_AT=80,
         MULTILINE_SPLIT_SEP='\n\t\t\t\t\t'
         ),
    dict(TIMESTAMP_FMT="%Y-%m-%d %H:%M:%S",
         MULTILINE_SPLIT_AT=80,
         MULTILINE_SPLIT_SEP='\n\t\t\t\t\t'
         )
]


class TextFormatter(Formatter):
    def __init__(self, show_messages=True, message_level=2):
        Formatter.__init__(self)
        self.show_messages = show_messages
        self.message_level = message_level
        self.message_level_set = LoggingLevel.LEVEL[message_level]

    def save_results(self, run_id):
        if not exists(TESTS_REPORT):
            makedirs(TESTS_REPORT)

        path = os.path.join(*(TESTS_REPORT, TESTS_CASE + str(run_id) + EXT))

        with open(path, 'w') as std:
            sys.stdout = std
            self.print_results(run_id)
        sys.stdout = sys.__stdout__

    def print_results(self, run_id):
        run_details = RunDAO.get_run_details(run_id)
        results_header = "Test run ID{horizontal_separator} #{run_id}""".format(
            horizontal_separator=HORIZONTAL_SEPARATOR,
            run_id=run_details.id)

        print "CTS version\t%s\n%s%s" % (BuildInformation.get_version(), results_header, VERTICAL_SEPARATOR)

        script_executions_results = (self.format_script_results(script_execution_id) for script_execution_id in
                                     ScriptDAO.get_scripts_execution_ids_for_run(run_id))

        lines = []
        for script_results in script_executions_results:
            for line in script_results:
                sys.stdout.write(line)
                lines.append(line)
            print VERTICAL_SEPARATOR
            lines.append(VERTICAL_SEPARATOR)
        return lines

    def format_script_results(self, script_execution_id):
        script_execution = ScriptDAO.get_script_execution_details(script_execution_id)

        status, passed, total = ScriptDAO.get_script_execution_statistics(script_execution_id)
        overall_status = ResultStatus.PASSED if status else ResultStatus.FAILED

        results_header = "Test script{horizontal_separator}{test_script_path}{vertical_separator}".format(
            horizontal_separator=HORIZONTAL_SEPARATOR, test_script_path=script_execution.script_path,
            vertical_separator=VERTICAL_SEPARATOR)

        results_header += "Overall status{horizontal_separator}{overall_status}{vertical_separator}".format(
            horizontal_separator=HORIZONTAL_SEPARATOR, overall_status=ColorPrinter.format_status(overall_status),
            vertical_separator=VERTICAL_SEPARATOR)

        results_header += "Pass rate{horizontal_separator}{passed}/{total}{vertical_separator}".format(
            horizontal_separator=HORIZONTAL_SEPARATOR, passed=passed, total=total,
            vertical_separator=VERTICAL_SEPARATOR)

        results_header += "configuration:{vertical_separator}".format(vertical_separator=VERTICAL_SEPARATOR)
        for param in script_execution.configuration.split("--"):
            if not param:
                continue

            param_name = param.split(" ")[0]
            if param_name in [PARAM_USER, PARAM_PASSWORD]:
                param_value = "**********"
            else:
                param_value = "".join(param.split(" ")[1:])

            results_header += \
                "{horizontal_separator}" \
                "{param_name:10}{horizontal_separator}" \
                "{param_value}{vertical_separator}" \
                    .format(vertical_separator=VERTICAL_SEPARATOR,
                            param_name=param_name,
                            param_value=param_value,
                            horizontal_separator=HORIZONTAL_SEPARATOR)

        results_header += "Level of details -d{level}\n".format(level=self.message_level)

        results_header += "Reported cases"
        yield results_header

        cases_results = (self.format_case_results(case_execution_id) for case_execution_id in
                         CaseDAO.get_cases_execution_ids_for_script_execution(script_execution_id))

        yield VERTICAL_SEPARATOR

        for case_result in cases_results:
            for line in case_result:
                yield line
            yield VERTICAL_SEPARATOR

    def format_case_results(self, case_execution_id):
        case_execution_details = CaseDAO.get_case_execution_details(case_execution_id)
        results_header = "Test Case: {case_name}{vertical_separator}{vertical_separator}".format(
            case_name=case_execution_details.name,
            vertical_separator=VERTICAL_SEPARATOR)

        results_footer = "{vertical_separator}{horizontal_separator}[{status}]\t{case_name}{vertical_separator}".format(
            horizontal_separator=HORIZONTAL_SEPARATOR, case_name=case_execution_details.name,
            status=ColorPrinter.format_status(case_execution_details.status), vertical_separator=VERTICAL_SEPARATOR)

        yield VERTICAL_SEPARATOR
        yield results_header

        for messages in self.format_case_messages(case_execution_id):
            yield messages
            yield VERTICAL_SEPARATOR

        yield results_footer
        yield VERTICAL_SEPARATOR
        yield CASE_SEPARATOR

    def format_case_messages(self, case_execution_id):
        if self.show_messages:
            for message in MessageDAO.get_messages_for_case_execution(case_execution_id):
                if LoggingLevel.CONTROL == message.message_level and LoggingLevel.CONTROL in self.message_level_set:
                    for (status_code, request, response) in self.request_from_control_channel(message.message_text):
                        yield "\n{horizontal_separator}request:".format(horizontal_separator=HORIZONTAL_SEPARATOR)
                        for line in request.split('\n'):
                            yield "{horizontal_separator}{row}".format(horizontal_separator=HORIZONTAL_SEPARATOR,
                                                                       row=line)

                        yield "\n{horizontal_separator}response [{code}]:" \
                            .format(horizontal_separator=HORIZONTAL_SEPARATOR, code=status_code)
                        for r in response.split('\n'):
                            yield "{horizontal_separator}{row}".format(horizontal_separator=HORIZONTAL_SEPARATOR,
                                                                       row=r)
                        yield "\n"
                else:
                    if message.message_level in self.message_level_set:
                        yield ("{horizontal_separator}{message_level}{horizontal_separator}" +
                               "{message_datetime}{horizontal_separator}{message_text}") \
                            .format(message_level=message.message_level,
                                    horizontal_separator=HORIZONTAL_SEPARATOR,
                                    message_datetime=message.message_datetime.strftime(FMT[self.message_level][
                                                                                           'TIMESTAMP_FMT']),
                                    message_text=to_multiline(message.message_text,
                                                              FMT[self.message_level]['MULTILINE_SPLIT_AT'],
                                                              FMT[self.message_level]['MULTILINE_SPLIT_SEP']),
                                    vertical_separator=VERTICAL_SEPARATOR)
