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
from cts_framework.db.dao.run_dao import RunDAO


class StatusDeleteAction(Action):
    ACTION = "delete"
    PARAM_NAME = "STATUS_TYPE"

    def fill_parser_arguments(self):
        self.parser.add_argument("test_run_ids", type=str, nargs="+", default=None,
                                 help="IDs of test runs that results will be deleted")

    def process_action(self, configuration):
        test_run_ids = configuration.test_run_ids

        for test_run_id in test_run_ids:
            RunDAO.delete_run_results(test_run_id)
