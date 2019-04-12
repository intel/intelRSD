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
from sources.smoke_tests.smoke_framework import SmokeFramework
from sources.smoke_tests.cts_commands import CtsCommands as cts_cmd

class SmokeTestsCTS(unittest.TestCase):
    def setUp(self):
        self.sf = SmokeFramework()

    def test_1_cts_exist(self):
        print("test_1_cts_exist")

        out, err, rc = self.sf.get_output_from_commnad(cts_cmd.CTS_VERSION_STRING)
        out = out.lower().replace(":", "").split()[:2]

        self.assertListEqual(out, cts_cmd.CTS_VERSION_STRING.lower().split())

    def test_2_cts_has_sos_functionality(self):
        print("test_2_cts_has_sos_functionality")

        out, err, rc = self.sf.get_output_from_commnad(cts_cmd.CTS_SOS_FUNC_CMD)
        out = out.lower().split()[:3]

        self.assertListEqual(out, cts_cmd.CTS_SOS_CMD_RESP.lower().split())
