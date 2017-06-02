"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

ETHTOOL_SPEED = "Speed: "
ETHTOOL_GBS = "Gb/s"
ETHTOOL_MBS = "Mb/s"
ETHTOOL_SPEED_SECTION = 1
ETHTOOL_VAL_SECTION = 0

def _parse_ehtool(content):
    lines = iter(content.strip().splitlines())
    mbs = -1
    while True:
        try:
            line = lines.next()
        except StopIteration:
            break
        if ETHTOOL_SPEED in line:
            speed = line.split(ETHTOOL_SPEED)[ETHTOOL_SPEED_SECTION]
            if ETHTOOL_GBS in speed:
                mbs = int(speed.split(ETHTOOL_GBS)[ETHTOOL_VAL_SECTION])*1000
                break
            if ETHTOOL_MBS in speed:
                mbs = int(speed.split(ETHTOOL_MBS)[ETHTOOL_VAL_SECTION])
                break
    return {NET_SPEED : mbs}

def get_ethtool_info(interface):
    ethtool = ShellCommand("ethtool " + interface)
    ethtool.execute()
    info = _parse_ehtool(ethtool.get_stdout())
    return info
