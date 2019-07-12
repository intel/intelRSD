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

import os


class Constants:
    VERSION_TEXT_FPGAoF_WHEEL = 'FPGAoF Wheel'
    FPGAOF_WHEEL_VERSION = '0.85'

    DEFAULT_SLEEP_TIME = 10

    FPGAOF_PARAM_TRADDR = 'traddr'
    FPGAOF_PARAM_TRSVCID = 'trsvcid'
    FPGAOF_PARAM_CONFIG_FILE_NAME = 'config_file'
    FPGAOF_PARAM_TIME_DELAY = 'time_delay'
    FPGAOF_PARAM_EXIT_COUNTER = 'exit_counter'

    FPGAOF_CONFIG_FILE_PATH = '/etc/opae/discovery_service.cfg'
