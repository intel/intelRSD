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

import unittest

from cts_framework.helpers.vars.env_consts import Constants
from sources.smoke_tests.cts_commands import CtsCommands as cts_cmd
from sources.smoke_tests.smoke_framework import SmokeFramework
from os import listdir, removedirs
from os.path import exists


class MetadataFunctionalTests(unittest.TestCase):
    def setUp(self):
        self.sf = SmokeFramework()

    def test_1_check_return_code(self):
        print("test_1_check_return_code")

        out, err, rc = self.sf.get_output_from_commnad(cts_cmd.CTS_TESTS_DUMP)

        if not rc:
            self.assertFalse(self, msg="Return code %s" % rc)

    def test_2_tests_dump_dir_exist(self):
        print("test_2_tests_dump_dir_exist")

        if not exists(Constants.DIR_HOME_CTS_TESTS):
            self.assertFalse(self, msg=Constants.DIR_HOME_CTS_TESTS + " don't exist")

    def test_3_tests_temp_dir_exist(self):
        print("test_3_tests_temp_dir_exist")

        if not exists(Constants.DIR_HOME_CTS_TESTS_DATA):
            self.assertFalse(self, msg=Constants.DIR_HOME_CTS_TESTS_DATA + " don't exist")

    def test_4_check_that_test_data_was_recreated(self):
        print("test_4_check_that_test_data_was_recreated")

        # try:
        #     removedirs(Constans.DIR_HOME_CTS_TESTS_DATA)
        # except OSError as ose:
        #     print ose
        # if exists(Constans.DIR_HOME_CTS_TESTS_DATA):
        #     self.assertFalse(self, "Can't remove %s" % Constans.DIR_HOME_CTS_TESTS_DATA)

        self.sf.get_output_from_commnad(cts_cmd.CTS_TESTS_DUMP)

        if exists(Constants.DIR_HOME_CTS_TESTS_DATA):
            self.assertGreaterEqual(len(listdir(Constants.DIR_HOME_CTS_TESTS_DATA)), 1)
