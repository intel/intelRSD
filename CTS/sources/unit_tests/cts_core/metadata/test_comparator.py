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
from cts_core.metadata.comparator import Comparator


class ComparatorUnitTest(unittest.TestCase):
    def setUp(self):
        pass

    def test_merger_results(self):
        self.assertEqual(Comparator.EQUAL, Comparator().merge_result(Comparator()).result)
        self.assertEqual(Comparator.EQUAL, Comparator().merge_result(Comparator(), Comparator()).result)
        self.assertEqual(Comparator.NON_EQUAL, Comparator().merge_result(Comparator().set_not_equal()).result)
        self.assertEqual(Comparator.NON_EQUAL, Comparator().merge_result(Comparator(), Comparator().set_not_equal()).result)
        self.assertEqual(Comparator.NON_EQUAL, Comparator().merge_result(Comparator().set_not_equal(),
                                                                         Comparator()).result)
