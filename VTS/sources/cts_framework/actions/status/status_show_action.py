"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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
from cts_framework.db.dao.test_case_results_dao import TestCaseResultsDAO
from cts_framework.db.dao.test_result_message_dao import TestResultMessageDAO
from cts_framework.db.dao.test_run_dao import TestRunDAO


class StatusShowAction(Action):
    ACTION = "show"
    PARAM_NAME = "STATUS_TYPE"

    def fill_parser_arguments(self):
        self.parser.add_argument("test_run_ids", type=str, nargs="+", default=None,
                                 help="IDs of test runs that results will be presented")

    def process_action(self, configuration):
        test_run_ids = configuration.test_run_ids

        for test_run_id in test_run_ids:
            try:
                self.print_run_results(test_run_id)
            except AttributeError:
                print "Test run with id %s not found" % test_run_id

    def print_run_results(self, run_id):
        test_run = TestRunDAO().get_test_run_details(run_id)

        print "Test run #%s from %s" % (test_run.test_run_id, test_run.test_run_datetime)
        for test_case in TestCaseResultsDAO().get_test_cases_results_for_test_run(test_run.test_run_id):
            print "\t%s\t%s" % (test_case.test_case_status, test_case.test_case_test_case_name)
            for test_message in TestResultMessageDAO().get_messages_for_test_case_result(test_case.test_case_id):
                print "\t\t%s\t[%s] %s" % (test_message.test_result_message_status,
                                           str(test_message.test_result_message_datetime)[:-5],
                                           test_message.test_result_message_message)
