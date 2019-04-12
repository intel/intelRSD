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
import ConfigParser
from cts_framework.helpers.vars.env_consts import Constants

from collections import defaultdict


class ConfigurationFileReader:
    def __init__(self, filename=Constants.CTS_CONFIGURATION_FILE):
        self._filename = filename

    def read_file(self):
        configuration = defaultdict(dict)

        config_parser = ConfigParser.ConfigParser()
        config_parser.optionxform = str
        config_parser.read([self._filename])

        for section in config_parser.sections():
            for parameter_name, parameter_value in config_parser.items(section):
                configuration[section][parameter_name] = parameter_value

        return configuration
