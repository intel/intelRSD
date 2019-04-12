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
from os.path import exists


class MetadataFunctionalTests(unittest.TestCase):
    def setUp(self):
        pass

    def test_1_metadata_dirs_exist(self):
        print("test_1_metadata_dirs_exist")

        if not exists(Constants.METADATA_HOME_DIR):
            self.assertFalse(self, msg=Constants.METADATA_HOME_DIR + " don't exist")

    def test_2_metadata_temp_dirs_exist(self):
        print("test_2_metadata_temp_dirs_exist")

        if not exists(Constants.METADATA_TEMP_DIR):
            self.assertFalse(self, msg=Constants.METADATA_TEMP_DIR + " don't exist")
