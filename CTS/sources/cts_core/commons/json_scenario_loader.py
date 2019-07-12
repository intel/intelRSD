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

import json
import os
from os import listdir, path

from cts_core.commons.error import cts_error
from cts_framework.helpers.vars.env_consts import Constants


class CaseAction:
    def __init__(self, username, password, request, request_method,
                 payload, response, retry_if_fail=0, retry_timeout=0):
        self.username = username
        self.password = password
        self.request = request
        self.request_method = request_method
        self.payload = payload
        self.response = response
        self.retry_if_fail = retry_if_fail
        self.retry_timeout = retry_timeout


class JsonLoader:
    def __init__(self, filepath=Constants.DIR_HOME_CTS_SCENARIO):
        self.create_config_file()

        self._filepath = filepath
        self._available_uses_cases, self._available_uses_scenarios =\
            self.retrieve_available_actions()

    def load(self, filepath=None):
        if filepath is None:
            filepath = self._filepath

        with open(filepath, 'r') as json_data:
            try:
                return json.load(json_data)
            except ValueError as e:
                cts_error('Can not load Use Cases: %s' % e)
                exit(1)

    def print_available_actions(self):
        if len(self._available_uses_cases):
            print('INFO:: Available use cases:')
            for case_number, case in enumerate(self._available_uses_cases, 1):
                print('INFO:: \t{0}: {1} ({2})'.format(case_number, case['filename'], case['name']))
        else:
            print('INFO:: No use cases found\n')

    @staticmethod
    def create_config_file():
        with open(Constants.USE_CASE_CONFIG, 'w+') as f:
            uses_case = {'available_cases': []}
            json.dump(uses_case, f, indent=4)

    @staticmethod
    def get_use_case(name):
        with open(Constants.USE_CASE_CONFIG, 'r+') as f:
            use_cases = json.load(f)
            return [x for x in use_cases['available_cases'] if x['filename'] == name][0]


    @staticmethod
    def get_use_case_action(filepath):
        with open(filepath, 'r') as f:
            loaded_use_case = json.load(f)
            return loaded_use_case

    def add_new_use_case(self, name, description, filename, filepath=None):
        if not path.exists(Constants.USE_CASE_CONFIG):
            self.create_config_file()

        with open(Constants.USE_CASE_CONFIG, 'r+') as f:
            uses_case = json.load(f)
            uses_case['available_cases'].append({'name': name,
                                                 'description': description,
                                                 'filename': filename,
                                                 'filepath': filepath})
            f.seek(0)
            json.dump(uses_case, f, indent=4)

    def retrieve_available_actions(self):
        """
        list dir with scenario and use cases
        """
        available_files = []
        for filename in [action_files for action_files in listdir(Constants.DIR_HOME_CTS_SCENARIO) if action_files.split('.')[-1] == 'action']:
            filepath = path.join(*(Constants.DIR_HOME_CTS_SCENARIO, filename))
            if path.isdir(filepath):
                continue
            available_files.append(filename)

        return self._validate_use_case(available_files)

    def _validate_use_case(self, files):
        uses_cases = []
        uses_scenario = []

        for f in [action_files for action_files in files if action_files.split('.')[-1] == 'action']:
            filepath = path.join(*(Constants.DIR_HOME_CTS_SCENARIO, f))
            loaded_case = self.load(filepath)
            if loaded_case['type'] == "usecase":
                loaded_case['filename'] = f.split('.')[:-1][0]
                uses_cases.append(loaded_case)
                self.add_new_use_case(loaded_case['name'],
                                      loaded_case['type'],
                                      f.split('.')[:-1][0],
                                      filepath)
            if loaded_case['type'] == "scenario":
                uses_scenario.append(loaded_case)

        return uses_cases, uses_scenario

    def get_uses_cases(self):
        return self._available_uses_cases

    def get_scenarios(self):
        return self._available_uses_scenarios

    def run_use_case(self, name):
        for uc in self._available_uses_cases:
            if uc['name'] == name['name']:
                return uc['actions']

    @staticmethod
    def _remove_dir(files):
        return [x for x in files if not os.path.isdir(x)]
