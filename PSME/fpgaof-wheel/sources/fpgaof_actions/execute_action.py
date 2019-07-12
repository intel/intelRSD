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

from sys import exit
from time import sleep

from fpgaof_actions.action import Action
from fpgaof_helpers.json_parse import JsonParse
from fpgaof_helpers.jsonrpc import JsonRpc
from fpgaof_helpers.message import Message
from fpgaof_helpers.uuid_generator import UuidGenerator
from fpgaof_helpers.constants import Constants


class ExecuteAction(Action):
    ACTION = 'execute'
    PARAM_NAME = 'ACTION'

    def fill_parser_arguments(self):
        self.parser.add_argument('-a', '--traddr',
                                 help='Address of Discovery Service')
        self.parser.add_argument('-s', '--trsvcid',
                                 help='Transport service id (e.g. IP port)')
        self.parser.add_argument('-q', '--hostuuid',
                                 default=UuidGenerator().get_uuid(),
                                 help='User-defined hostnqn (if default not used)')
        self.parser.add_argument('-e', '--exit_counter', type=int, const=12, nargs='?',
                                 help='Exit script after set unsuccessful attempts to Discovery Service(default is 12)')
        self.parser.add_argument('-t', '--time_delay',
                                 help='Set amount of time between discoveries (default is' +
                                      str(Constants.DEFAULT_SLEEP_TIME)+' seconds)')


    def process_action(self, configuration):
        self.pre_action(configuration)
        self.configuration = configuration
        self.crone()
        exit(0)

    def pre_action(self, configuration):
        self.uuid = UuidGenerator().get_uuid()
        if str(self.uuid) != str(configuration.hostuuid):
            self.uuid = configuration.hostuuid
        self.time_delay = float(configuration.time_delay if configuration.time_delay else Constants.DEFAULT_SLEEP_TIME)

    def crone(self):
        if self.configuration.exit_counter is not None:
            MAX_EXIT_COUNTER = self.configuration.exit_counter
        else:
            MAX_EXIT_COUNTER = 12
        exit_counter = 0

        while exit_counter < MAX_EXIT_COUNTER:
            found = self.discovery()

            if found is None and self.configuration.exit_counter is not None:
                exit_counter = exit_counter + 1
                Message.info('There was a problem with connecting to Discovery Service (%d/%d)' %
                             (exit_counter, MAX_EXIT_COUNTER))
                sleep(self.time_delay)

            if found is not None:
                Message.info('Connected to discovery service')
                exit_counter = 0
                if found:
                    Message.info('Target(s) found' + str(found))

                self.save_targets(found, Constants.FPGAOF_CONFIG_FILE_PATH)
                sleep(self.time_delay)

        Message.error('There was a problem with connecting to Discovery Service (%d/%d)\nExiting script...' %
                      (exit_counter, MAX_EXIT_COUNTER))
        exit(-1)

    def discovery(self):
        try:
            rpc = JsonRpc(self._get_discovery_server_url(vars(self.configuration)), self.uuid)

            discovery_entries = rpc.call()

            if discovery_entries is None:
                return None
            elif not discovery_entries:
                Message.info('Output from FPGAoF discover looks empty')
                return []

            list_of_discovered = []
            for entry in discovery_entries:
                entry_to_add = dict(
                        ip_address=entry['ip'],
                        port_num=entry['port'],
                        transport=entry['transport'])
                if entry_to_add in list_of_discovered:
                    continue
                else:
                    list_of_discovered.append(entry_to_add)
                return list_of_discovered
        except Exception as e:
            Message.error('Error getting discovery entries: ' + str(e))
            return None

    @staticmethod
    def save_targets(targets, config_file_name):
        JsonParse(targets, config_file_name)

    @staticmethod
    def _get_discovery_server_url(configuration):
        try:
            discovery_service_url = configuration[Constants.FPGAOF_PARAM_TRADDR] + ':' + \
                                    configuration[Constants.FPGAOF_PARAM_TRSVCID]
        except TypeError:
            Message.error("Error parsing Discovery Service's address and port. \n Exiting script...")
            exit(-1)
            return None
        return discovery_service_url
