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
from subprocess import Popen, PIPE

from fpgaof_helpers.message import Message

class UuidGenerator:
    def __init__(self):
        self.uuid = self.read_uuid()

    def get_uuid(self):
        return self.uuid

    @staticmethod
    def read_uuid():
        read_command = "sudo cat /sys/class/dmi/id/product_uuid"
        process = Popen(read_command, stdout=PIPE, shell=True)
        output = []

        for line in process.stdout.readlines():
            output.append(line)

        sanitize_output = ''.join(output).replace("\n", "").replace(" ", "")

        if not sanitize_output:
            Message.error("Can\'t read UUID, run script with higher privileges")
            sys.exit(1)
        return sanitize_output.lower()


if __name__ == "__main__":
    print(UuidGenerator().get_uuid())
