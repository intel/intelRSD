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
from include.libs.kernelparams.location import Location

def read_location():
    kernel_params = ShellCommand("cat /proc/cmdline")
    kernel_params.execute()
    location = Location()
    try:
        location_param = kernel_params.get_stdout().split("location=")[1]
        parsed_params = location_param.split(",")
        try:
            pod, rack, drawer, module, blade = parsed_params
        except ValueError:
            pod = "pod=0"
            rack, drawer, module, blade = parsed_params
        location = Location(pod.split("=")[1], rack.split("=")[1],
                drawer.split("=")[1], module.split("=")[1], blade.split("=")[1])
    except IndexError, ValueError:
        pass

    return location

