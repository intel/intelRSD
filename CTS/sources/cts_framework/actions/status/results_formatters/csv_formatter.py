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
import os
import re
import sys

from os import makedirs
from os.path import exists

from cts_framework.actions.status.results_formatters.formatter import Formatter
from cts_framework.build_information import BuildInformation
from cts_framework.db.dao.case_dao import CaseDAO
from cts_framework.db.dao.message_dao import MessageDAO
from cts_framework.db.dao.run_dao import RunDAO
from cts_framework.db.dao.script_dao import ScriptDAO
from cts_framework.helpers.python.result_status import ResultStatus

EXT = ".csv"
VERTICAL_SEPARATOR = "\n"
HORIZONTAL_SEPARATOR = "\t"
PARAM_USER = "User"
PARAM_PASSWORD = "Password"
TESTS_REPORT = "cts_report"
TESTS_CASE = "test-report-"
REGEX_FOR_ID = r'\[.Id=[a-z0-9]*\]'


class CsvFormatter(Formatter):
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

        for script_results in script_executions_results:
            for line in script_results:
                # replace comma to dot to avoid conflict in opening CSV format and drop empty line
                if line is not VERTICAL_SEPARATOR:
                    sys.stdout.write(line.replace(',', '.'))
            print VERTICAL_SEPARATOR

    def format_script_results(self, script_execution_id):
        script_execution = ScriptDAO.get_script_execution_details(script_execution_id)

        status, passed, total = ScriptDAO.get_script_execution_statistics(script_execution_id)
        overall_status = ResultStatus.PASSED if status else ResultStatus.FAILED

        results_header = "Test script{horizontal_separator}{test_script_path}{vertical_separator}".format(
            horizontal_separator=HORIZONTAL_SEPARATOR, test_script_path=script_execution.script_path,
            vertical_separator=VERTICAL_SEPARATOR)

        results_header += "Overall status{horizontal_separator}{overall_status}{vertical_separator}".format(
            horizontal_separator=HORIZONTAL_SEPARATOR, overall_status=overall_status,
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
                "{param_value}{vertical_separator}".format(
                    vertical_separator=VERTICAL_SEPARATOR,
                    param_name=param_name,
                    param_value=param_value,
                    horizontal_separator=HORIZONTAL_SEPARATOR)

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
        results_header = "{horizontal_separator}{horizontal_separator}{horizontal_separator}" \
                         "{status}{horizontal_separator}" \
                         "{horizontal_separator}{case_name}{vertical_separator}".format(
                             horizontal_separator=HORIZONTAL_SEPARATOR,
                             case_name=case_execution_details.name,
                             status=case_execution_details.status,
                             vertical_separator=VERTICAL_SEPARATOR)

        yield results_header
        for messages in self.format_case_messages(case_execution_id):
            yield messages
            yield VERTICAL_SEPARATOR

    @staticmethod
    def message_hash(message_string):
        try:
            return re.search(REGEX_FOR_ID, message_string).group()
        except:
            return HORIZONTAL_SEPARATOR

    @staticmethod
    def remove_hash_id_from_message(message_string):
        try:
            return re.sub(REGEX_FOR_ID, '', message_string)
        except:
            return message_string

    def format_case_messages(self, case_execution_id):
        return (
            "{message_datetime}{horizontal_separator}{message_level}{horizontal_separator}{message_id}" \
            "{horizontal_separator}{horizontal_separator}{message_text}{vertical_separator}".format(
                message_level=message.message_level,
                horizontal_separator=HORIZONTAL_SEPARATOR,
                message_datetime=message.message_datetime,
                message_id=self.message_hash(message.message_text),
                message_text=self.remove_hash_id_from_message(message.message_text),
                vertical_separator=VERTICAL_SEPARATOR)
            for message in MessageDAO.get_messages_for_case_execution(case_execution_id))
