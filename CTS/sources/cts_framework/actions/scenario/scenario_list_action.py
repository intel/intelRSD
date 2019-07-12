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

import sys

from cts_core.commons.json_scenario_loader import JsonLoader
from cts_framework.actions.action import Action
from cts_framework.build_information import BuildInformation
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.commons.logging_helper import LoggingHelper
from cts_framework.tests_managing.test_package.tests_packages_container import TestsPackagesContainer
from cts_framework.tests_managing.tests_manager import TestsManager
from cts_framework.tests_running.execution_feed import ExecutionFeed
from cts_framework.tests_running.test_run import TestRun
from cts_framework.commons.config_reader import get_configuration_from_files


class ScenarioListAction(Action):
    def __init__(self, *params, **kwargs):
        self._logger = LoggingHelper(__name__)
        # self.tests_manager = TestsManager()

        Action.__init__(self, *params, **kwargs)

    ACTION = "list"
    PARAM_NAME = "EXECUTE_TYPE"

    def fill_parser_arguments(self):
        pass

    def process_action(self, configuration):
        print "Using CTS in version %s\n" % (ColorPrinter.format_text(BuildInformation.get_version(), bold=True))

        JsonLoader().print_available_actions()
