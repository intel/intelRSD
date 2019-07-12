"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

import sys
import os
from datetime import datetime
from nvme_framework.helpers.constants import Constants


class LogCapture(object):
    def __init__(self, configuration):
        self.configuration = configuration
        self.terminal = sys.stdout = sys.stderr
        if configuration['logfile']:
            self.log = open("".join((os.getcwd(), "/", configuration['logfile'], '.log')), "a")
        else:
            self.log = open(Constants.LOG_FILE_PATH, "a+")
        self.initial_message()

    def write(self, message):
        self.terminal.write(message)
        self.log.write(message)

    def initial_message(self):
        self.log.write(" ".join(("\nNVME-Wheel script log file from", str(datetime.now()), Constants.VERTICAL_SEPARATOR,
                                 'Used parameters:', str(sys.argv[0]).split("/")[-1])))
        for key in sys.argv[1:]:
            self.log.write(" " + key)
        self.log.write("".join((Constants.VERTICAL_SEPARATOR, Constants.HORIZONTAL_SEPARATOR,
                                Constants.VERTICAL_SEPARATOR)))
