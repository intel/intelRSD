"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2018 Intel Corporation
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
import json
import re
import time
import sys
from argparse import Namespace
from subprocess import Popen, PIPE

from nvme_framework.actions.action import Action
from nvme_framework.configuration.config_manager import Config as Cfg
from nvme_framework.helpers.constants import Constants
from nvme_framework.helpers.custom_decorators import deprecated
from nvme_framework.helpers.database import update_timestamp, update_local_path, get_path
from nvme_framework.helpers.json_parser import JsonParse
from nvme_framework.helpers.message import Message
from nvme_framework.helpers.performer import Performer
from nvme_framework.helpers.uuid_generator import UuidGenerator
from nvme_framework.helpers.log_capture import LogCapture


class ExecuteAction(Action):
    ACTION = 'execute'
    PARAM_NAME = 'ACTION'

    def fill_parser_arguments(self):
        self.parser.add_argument('-a', '--traddr', help='Transport address')
        self.parser.add_argument('-t', '--transport',
                                 default="rdma",
                                 help='Transport type')
        self.parser.add_argument('-q', '--hostnqn',
                                 default=UuidGenerator().get_uuid(),
                                 help='User-defined hostnqn (if default not used)')
        self.parser.add_argument('-s', '--trsvcid', help='Transport service id (e.g. IP port)')

        # Additional arguments
        self.parser.add_argument('-e', '--endpoint', action='store_true',
                                 help='Run endpoint with information about script operations')
        self.parser.add_argument('-l', '--logfile', default="",
                                 help='Set name for a log file in your current working catalog (Disables /tmp/ logs)')

    def process_action(self, configuration):
        configuration = self.precheck_configuration(configuration)
        self.pre_action(configuration)

        self.disconnect_all_stored_in_config()
        self.crone()
        sys.exit(0)

    @staticmethod
    def run_exec(*args):
        process = Popen(' '.join(args), stdout=PIPE, shell=True)
        output = []
        for line in process.stdout.readlines():
            output.append(line)

        return ''.join(output)

    # TODO some refactor needed
    def pre_action(self, configuration):
        self.uuid = str(configuration.hostnqn)

        sys.stdout = LogCapture(vars(configuration))
        if configuration.endpoint:
            Popen(' '.join(['nvme-wheel', 'rest']), shell=True, stdin=None, stdout=None, stderr=None, close_fds=True)
        self.configuration = configuration

    def precheck_configuration(self, configuration):
        configuration = vars(configuration)
        for key in configuration:
            if configuration[key] is None:
                return self.get_json_data(configuration)
        return Namespace(**configuration)

    def get_json_data(self, temp_configuration):
        try:
            with open(Constants.CONFIGURATION_FILE_PATH) as config_file:
                local_config = json.load(config_file)
            for key in temp_configuration:
                if temp_configuration[key] is None:
                    temp_configuration[key] = local_config[key]
            return Namespace(**temp_configuration)
        except StandardError:
            Message.error('Error parsing required parameters from configuration file and/or command line')
            sys.exit(1)


    def crone(self):
        time_before_next_discovery = Cfg().get_cron_time()

        while True:
            found = self.find_detached_targets()
            self.prepare_targets_for_detach(found)
            self.disconnect_lost()
            self.connect_discovered()

            time.sleep(float(time_before_next_discovery))

    @update_timestamp
    def discovery(self):
        jp = JsonParse()
        discover_command = self._sanitized_configuration_for_discover(self.configuration)

        return jp.discovery_output(self.run_exec(discover_command))

    @deprecated
    def disconnect(self, target_name=None):
        # example: nvme disconnect -n UUID/NQN/SUBNQN
        connected_elements_before = self.count_nvme_list_elements()
        self.run_exec(Constants.NVME_DISCONNECT,
                      '-n ', target_name)

        # verify that nvme disconnect works and elem was removed from nvme list
        if connected_elements_before == self.count_nvme_list_elements():
            return False
        return True

    def save_disconnect(self, target_name=None):
        connected_elements_before = self.count_nvme_list_elements()
        Performer.safe_deletions(get_path(target_name))

        if connected_elements_before == self.count_nvme_list_elements():
            return False
        return True

    @update_local_path
    def connect(self, target_name=None, target_address=None, target_port=None, acceptable_uuid=None):
        # example: nvme connect -t rdma -a 10.6.0.101 -n UUID/NQN/SUBNQN -s target_port -q initiator_UUID-i 8
        connected_elements_before = self.count_nvme_list_elements()

        self.run_exec(Constants.NVME_CONNECT,
                      Constants.NVME_T_RMA_PARAM,
                      '-a ', target_address,
                      '-n ', target_name,
                      '-s ', target_port,
                      '-q ', acceptable_uuid,
                      '-i ', '8')
        # verify that nvme connect works and new elem is on nvme list
        if connected_elements_before == self.count_nvme_list_elements():
            return False
        return True

    @staticmethod
    def _sanitized_configuration_for_discover(configuration):
        cmd = [Constants.NVME_DISCOVER]
        allowed_params_for_discover_cmd = [Constants.NVME_PARAM_TRANSPORT,
                                           Constants.NVME_PARAM_TRADDR,
                                           Constants.NVME_PARAM_TRSVCID,
                                           Constants.NVME_PARAM_HOSTNQN]
        config = vars(configuration)
        for key in allowed_params_for_discover_cmd:
            if config[key]:
                cmd.append("--%s=%s" % (key, config[key]))
        return ' '.join(cmd)

    @update_timestamp
    def connect_discovered(self):
        cfg = Cfg()
        targets_with_discovered_status = [tgts for tgts in cfg.get_available_targets()
                                          if tgts['status'] == Constants.STATUS_DISCOVERED]

        for target in targets_with_discovered_status:
            tgt_name = target[Constants.CONFIG_TARGET_NAME]
            tgt_address = target[Constants.CONFIG_TARGET_ADDRESS]
            tgt_port = target[Constants.CONFIG_TARGET_PORT]

            Message.info('Connecting to %s - %s:%s' % (tgt_name, tgt_address, tgt_port))
            if not self._connect_runner(cfg, tgt_name, tgt_address, tgt_port):
                Message.info('Connection process FAILED')
            else:
                Message.info('Connection process SUCCESS')

    def _connect_runner(self, cfg, tgt_name, tgt_address, tgt_port):
        regular_connection = (tgt_name, tgt_address, tgt_port, self.uuid)
        connection_with_prefix = (self._update_nqn_with_prefix(tgt_name),
                                  tgt_address,
                                  tgt_port,
                                  self._update_nqn_with_prefix(self.uuid))
        connection_without_prefix = (self._remove_prefix_from_nqn(tgt_name),
                                     tgt_address,
                                     tgt_port,
                                     self._remove_prefix_from_nqn(self.uuid))

        connection_types = (regular_connection, connection_with_prefix, connection_without_prefix)

        for i, connection_type in enumerate(connection_types, 1):
            if not self.connect(*connection_type):
                if i == len(connection_types):
                    return False
                continue

            cfg.update_status(tgt_name, Constants.STATUS_CONNECTED)
            Message.debug(
                'Connected to {0} using with custom values:\n\ttarget_name: {1}\n\tinitiator uuid: {2}'.format(
                    tgt_name,
                    self._update_nqn_with_prefix(tgt_address),
                    self._update_nqn_with_prefix(self.uuid)
                ))
            return True

    @staticmethod
    def _update_nqn_with_prefix(tgt_name):
        if re.search(r'{}'.format(Constants.UUID_NQN_PREFIX),
                     tgt_name):
            return tgt_name
        return str(''.join((Constants.UUID_NQN_PREFIX, tgt_name)))

    @staticmethod
    def _remove_prefix_from_nqn(nqn):
        return nqn.replace(Constants.UUID_NQN_PREFIX, '')

    @update_timestamp
    def disconnect_lost(self):
        cfg = Cfg()
        for target in cfg.get_available_targets():
            if target['status'] == Constants.STATUS_LOST:
                Message.info('Disconnecting %s' % target[Constants.CONFIG_TARGET_NAME])
                self.save_disconnect(target[Constants.CONFIG_TARGET_NAME])
                cfg.remove_element_by_name(target[Constants.CONFIG_TARGET_NAME])

    @update_timestamp
    def find_detached_targets(self):
        config = Cfg()
        discovered_targets = self.discovery()

        targets_stored_in_config = []
        targets_discovered = []

        for target in config.get_available_targets():
            targets_stored_in_config.append(target[Constants.CONFIG_TARGET_NAME])

        for discovered_target in discovered_targets:
            targets_discovered.append(discovered_target[Constants.CONFIG_TARGET_NAME])

        return list(set(targets_stored_in_config) - set(targets_discovered))

    @staticmethod
    @update_timestamp
    def prepare_targets_for_detach(targets_for_detach):
        cfg = Cfg()
        for tgt_name in targets_for_detach:
            cfg.update_status(tgt_name, Constants.STATUS_LOST)

        if len(targets_for_detach):
            Message.info('Targets for detach: %s' % ' '.join(targets_for_detach))

    def count_nvme_list_elements(self):
        output = self.run_exec(Constants.NVME_LIST, Constants.NVME_JSON_OUTPUT_PARAM)

        try:
            # nvme-cli return empty {} or nothing in nvme-cli 1.3+ if there is not any target
            json_output = json.loads(output)
            return len(json_output['Devices'])
        except KeyError:
            return 0

    def disconnect_all_stored_in_config(self):
        cfg = Cfg()
        for target in cfg.get_available_targets():
            cfg.update_status(target[Constants.CONFIG_TARGET_NAME],
                              Constants.STATUS_LOST)

        self.disconnect_lost()
        cfg.remove_all_targets()
