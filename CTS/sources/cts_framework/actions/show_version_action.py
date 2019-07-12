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
import argparse

from cts_framework.actions.action import Action
from cts_framework.actions.update.update import Update
from cts_framework.build_information import BuildInformation


class ShowVersionAction(Action):
    ACTION = "version"
    PARAM_NAME = "ACTION"

    def fill_parser_arguments(self):
        self.parser.add_argument("-u", "--update", default=False, action='store_true',
                                 help=argparse.SUPPRESS)
        self.parser.add_argument("-g", "--generate", default=False, action='store_true',
                                 help=argparse.SUPPRESS)

    def process_action(self, configuration):
        if configuration.update:
            self.update_process()
        elif configuration.generate:
            self.generate_config()
        else:
            print "CTS Version: %s" % BuildInformation.get_version()

    @staticmethod
    def update_process():
        update = Update()
        update.remove_old_packages_and_folders()
        update.read_configuration_from_file()
        update.create_update_dir()

        package_link = update.check_new_version_is_available()
        if package_link is not None:
            update.download_latest_package(package_link)
            update.print_manual_instruction()

    @staticmethod
    def generate_config():
        Update().generate_update_config_ini()
