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

import os
from os.path import expanduser


class Constants:
    CTS = ".cts"
    DIR_HOME = expanduser("~")

    DIR_HOME_CTS = os.path.join(*(DIR_HOME, CTS))
    DIR_HOME_CTS_CONFIG = os.path.join(*(DIR_HOME_CTS, "configuration"))
    DIR_HOME_CTS_TESTS = os.path.join(*(DIR_HOME_CTS, "tests"))
    DIR_HOME_CTS_TESTS_DATA = os.path.join(*(DIR_HOME_CTS, "tests_data"))
    DIR_HOME_CTS_PERFORMANCE_DATA = os.path.join(*(DIR_HOME_CTS, "performance_data"))

    DIR_HOME_CTS_SCENARIO = os.path.join(*(DIR_HOME_CTS_TESTS, "Rack_Scale_2_5_Scenario/required"))
    DIR_HOME_CTS_SCENARIO_EXTERNAL = os.path.join(*(DIR_HOME_CTS_SCENARIO, "ext"))

    TESTS_PACKAGES = "tests_packages"
    TESTS_PACKAGES_SUBDIR = "required"
    TESTS_PACKAGES_OPTIONAL = "optional"
    TESTS_DIRECTORY_NAME = "tests"

    CTS_CONFIGURATION_FILE = os.path.join(*(DIR_HOME, CTS, "configuration", "configuration.ini"))
    CTS_CONFIGURATION_FILE_UPDATE = os.path.join(*(DIR_HOME, CTS, "configuration", "update_config.ini"))

    USE_CASE_CONFIG = os.path.join(*(DIR_HOME_CTS_TESTS_DATA, "use-case.json"))

    CTS_UPDATE_DIR = os.path.join(*(DIR_HOME, CTS, "update"))
    CTS_FILE_UPDATE = os.path.join(*(DIR_HOME, CTS, "update", "cts_update.tar.gz"))

    OPT_CTS_PATH = "/opt/cts"
    OPT_CTS_TESTS_PATH = "/opt/cts/tests"

    METADATA_PACKAGES = "metadata"
    METADATA_HOME_DIR = os.path.join(*(DIR_HOME_CTS, "metadata"))
    METADATA_REDFISH_DIR = os.path.join(*(METADATA_HOME_DIR, "redfish"))

    INTEL_RACKSCALE_NONTRIVIAL = "IntelRackScale.Nontrivial"
    INTEL_RACKSCALE_EXTENSIONS_NONTRIVIAL = "IntelRackScale.Extensions.Nontrivial"

    TESTS_PATH = [
        [OPT_CTS_TESTS_PATH + "/*"],
        [DIR_HOME_CTS_TESTS + "/*"]
    ]
    TIME_MS = 'ms'
    #  messages
    EXTERNAL_SCRIPT_WAS_NOT_FOUND = "External script was not found"

