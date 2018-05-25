"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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

from include.common.shell_command import ShellCommand
from include.common.globals import *


class Udevadm:

    UDEVADM_SERIAL = "ID_SERIAL_SHORT="
    UDEVADM_MODEL = "ID_MODEL="

    @staticmethod
    def parse_udevadm(content):
        lines = iter(content.strip().splitlines())

        serial = ""
        model = ""
        got_serial = False
        got_model = False

        while True:
            try:
                line = lines.next()
            except StopIteration:
                break
            if Udevadm.UDEVADM_SERIAL in line:
                serial = line.split(Udevadm.UDEVADM_SERIAL)[1]
                got_serial = True
            if Udevadm.UDEVADM_MODEL in line:
                model = line.split(Udevadm.UDEVADM_MODEL)[1]
                got_model = True
            if got_model and got_serial:
                break

        return (serial, model)

    @staticmethod
    def get_udevadm_info(device):
        udevadm = ShellCommand("/sbin/udevadm info --query=property --name=" + device)
        udevadm.execute()
        info = Udevadm.parse_udevadm(udevadm.get_stdout())
        return info
