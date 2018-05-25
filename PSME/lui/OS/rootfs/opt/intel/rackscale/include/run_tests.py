"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017-2018 Intel Corporation
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

import unittest2
import include.unit_tests
import sys
from colour_runner.runner import ColourTextTestRunner

def main():
    all_tests = unittest2.TestLoader().loadTestsFromModule(include.unit_tests)
    runner = ColourTextTestRunner(buffer=True, verbosity=2)
    success = runner.run(all_tests).wasSuccessful()
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
