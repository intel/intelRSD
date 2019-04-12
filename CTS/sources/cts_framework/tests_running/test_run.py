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

from sys import argv
from cts_framework.db.dao.run_dao import RunDAO


class TestRun:
    def __init__(self):
        self.configuration_groups = []
        """ :type: *cts_framework.tests_running.configuration_group.ConfigurationGroup"""

    def add_execution_feed(self, execution_feed):
        """

        :type execution_feed: cts_framework.tests_running.execution_feed.ExecutionFeed
        :return:
        """
        self.configuration_groups.append(execution_feed)

    def register_all(self):
        cmd = " ".join(argv)
        test_run_id = RunDAO.register_run("TBD", cmd)
        for configuration_group in self.configuration_groups:
            configuration_group.register_all(test_run_id)

    def run_all(self):
        status = True

        for configuration_group in self.configuration_groups:
            status = configuration_group.run_all() and status

        return status
