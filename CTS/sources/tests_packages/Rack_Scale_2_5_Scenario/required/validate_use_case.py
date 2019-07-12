#!/usr/bin/env python
"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2018 Intel Corporation
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
from time import sleep

from cts_core.commons.error import cts_error, cts_message
from cts_core.commons.external_command_over_ssh import ExternalCommandOverSSH
from cts_core.commons.external_script import ExecuteExternalScript
from cts_core.commons.json_scenario_loader import CaseAction, JsonLoader
from cts_core.commons.api_caller import ApiCaller
from cts_core.commons.services import ServiceTypes, RegisterService
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.metadata.metadata_constants import MetadataConstants2_5 as MetadataConstants
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.helpers.python.cts_test_script import CtsTestScript
from cts_framework.helpers.python.test_script import TestScript


def CaseInformation(fn):
    def case_information_wrapper(*args, **kwargs):
        name = args[1]
        step = args[4]

        cts_message('')
        cts_message('Step: %s' % step)
        cts_message('Name: %s' % name)

        result = fn(*args, **kwargs)

        cts_message('End of %s (%s)' % (name, step))
        return result
    return case_information_wrapper


@RegisterService(ServiceTypes.SCENARIO_2_5)
class ValidateUseCase(CtsTestScript):
    TIMEOUT = 600
    DESCRIPTION = """test case validating api responses against provided use case"""

    CONFIGURATION_PARAMETERS = [
        TestScript.ConfigurationParameter(parameter_name="UseCase",
                                          parameter_description="Use Case name",
                                          parameter_type=str,
                                          is_required=False,
                                          parameter_default_value=[])
    ]

    def run(self):
        try:
            json_loader = JsonLoader()
        except OSError:
            print('ERROR:: No proper action found. Please reinstall a CTS')
            return

        if self.metadata_container is None:
            return

        if not self.configuration.UseCase:
            json_loader.print_available_actions()
            return

        try:
            actions_to_take = json_loader.get_use_case(self.configuration.UseCase)
            specific_action = json_loader.get_use_case_action(actions_to_take['filepath'])
        except (IndexError, TypeError):
            json_loader.print_available_actions()
            print('ERROR:: The selected action was not found. Exiting..')
            return

        print('TEST_CASE::API crawling')
        api_explorer = ApiExplorer(self.metadata_container, self.configuration)
        self.discovery_container, status = api_explorer.discover(MetadataConstants.SERVICE_ROOT_URI,
                                                                 MetadataConstants.SERVICE_ROOT)
        configuration = self.configuration
        self.api_caller = ApiCaller(configuration)

        print('TEST_CASE::Action Runner')
        status = self.action_runner(specific_action)
        print('STATUS::{status}'.format(status=status))

    def action_runner(self, specific_action):
        actions_status = ValidationStatus.RUNNING

        for action_step, action in enumerate(specific_action['actions'], 1):
            step_counter = "%d/%d" % (action_step, len(specific_action['actions']))
            action_type = action['type']
            if action_type == 'timeout':
                try:
                    self.perform_timeout(step_counter, action['value'])
                except IndexError:
                    self.perform_timeout(step_counter)
                continue

            retry_if_fail, retry_timeout = self.__get_retry_information(action)
            action_args = (specific_action['name'],
                           action['name'],
                           action['action'],
                           step_counter,
                           int(retry_if_fail),
                           int(retry_timeout))

            if action_type == 'external':
                actions_status = ValidationStatus.join_statuses(self.perform_external_command(*action_args))
                continue
            elif action_type == 'usecase':
                actions_status = ValidationStatus.join_statuses(self.perform_action(*action_args))
        return actions_status

    @staticmethod
    def perform_timeout(step_counter, timeout=1):
        cts_message("Step: %s" % step_counter)
        cts_message("Timeout: %s" % timeout)
        sleep(float(timeout))

    @CaseInformation
    def perform_external_command(self, test_name, case_name, action, step_counter, retry=0, timeout=0):
        """
        Perform command on external host

        :param test_name: !! used by decorator
        :param case_name: !! used by decorator
        :param action:
        :param step_counter: !! used by decorator
        :param retry:
        :param timeout:
        :return:
        """
        #  CONNECTION PHASE
        response_from_remote_host = self._connection_phase(action)

        #  VERIFICATION PHASE
        print(response_from_remote_host)
        verification_status = self._verification_phase(action, raw_response=response_from_remote_host)

        if not verification_status:
            return ValidationStatus.FAILED
        return ValidationStatus.PASSED

    @staticmethod
    def run_commands_on_host(commands, credentials):
        responses_from_ssh_target = []

        with ExternalCommandOverSSH(**credentials) as host:
            for cmd in commands:
                _, stdout, stderr = host.exec_command(cmd)
                err = stderr.read()
                if err is not '':
                    cts_error(err)
                responses_from_ssh_target.append(stdout.read())

        return responses_from_ssh_target[-1]

    def _connection_phase(self, action):
        """
        Do actions on remote and return last response
        """
        tgt_credential = action['target_credential']
        command = action['command']

        first_target = {
            "target_hostname": tgt_credential['hostname'],
            "target_username": tgt_credential['username'],
            "target_password": tgt_credential['password'],
            "target_port": tgt_credential['port']
        }

        try:
            second_tgt_credential = action['jump_target_credential']
            second_target = {
                "second_hostname": second_tgt_credential['hostname'],
                "second_username": second_tgt_credential['username'],
                "second_password": second_tgt_credential['password'],
                "second_port": second_tgt_credential['port']
            }
        except KeyError:
            return self.run_commands_on_host(command, first_target)

        return self.run_commands_on_host(command, self.__merge_dicts(first_target,
                                                                     second_target))

    def _verification_phase(self, action, raw_response=None):
        external_command = action['response']['command']
        expected_response = action['response']['result']

        command_type, command_location = self.__verify_command_type(external_command)
        command_args = self.__verify_command_args(external_command)

        external_action = ExecuteExternalScript(script_path=command_location,
                                                raw_response=raw_response,
                                                expected_result=expected_response)
        if not external_action(**command_args):
            cts_error('Script response IS NOT equal to excepted.')
            return False
        cts_message('Script response is equal to excepted.')
        return True

    @CaseInformation
    def perform_action(self, test_name, case_name, action, step_counter, retry=0, timeout=0):
        case_action = CaseAction(**action)

        cts_message("_____")
        cts_message("{name}: {step}\n\t\t {description}".format(name=test_name,
                                                                step=step_counter,
                                                                description=case_name))

        cts_message("{request_method}: {request}".format(request_method=action['request_method'],
                                                         request=action['request'].replace("{", "(").replace("}", ")")))

        resource_type, resource_id, resource_action = self._get_target_information(case_action.request)
        discovered_systems = self.discovery_container.get_resources(resource_type,
                                                                    any_child_version=True)

        if not discovered_systems:
            cts_error('Cannot find %s' % resource_type)
            return ValidationStatus.BLOCKED

        try:
            computer_odata_id = self.discovery_container.get(discovered_systems[resource_id][0]).odata_id
        except IndexError:
            cts_error('Cannot find %s' % resource_type)
            return ValidationStatus.BLOCKED

        action_endpoint = '{odata_id}/{action_name}'.format(odata_id=computer_odata_id,
                                                            action_name='/'.join(resource_action))

        status, _, _, _ = self.api_caller.post_resource(action_endpoint,
                                                        self.discovery_container,
                                                        case_action.payload,
                                                        acceptable_return_codes=case_action.response)

        if (not status) and (retry > 0):
            cts_message("Operation failed, left retry %s" % retry)
            cts_message("Timeout: %s" % timeout)
            sleep(float(timeout))

            self.perform_action(test_name, case_name, action, step_counter, retry=(retry - 1), timeout=timeout)

        if not status:
            cts_error('Cannot execute action %s' % action_endpoint)
            return ValidationStatus.FAILED

        cts_message('End of: %s' % case_name)
        return ValidationStatus.PASSED

    def _get_target_information(self, request):
        request_cleaned = request.replace('{', '').replace('}', '').split('/')

        resource_type = request_cleaned[0]
        resource_id = self._choose_resource(request_cleaned[1])
        action = request_cleaned[2:]

        return resource_type, resource_id, action

    @staticmethod
    def _choose_resource(number):
        return 0 if number == 'any' else int(number)

    @staticmethod
    def __verify_command_type(external_cmd):
        command_type, command_location = (None,) * 2
        try:
            command_type = external_cmd[0]
            if command_type.lower() != 'python':
                cts_message('CTS support only Python external scripts')
                return False

            command_location = external_cmd[1]
        except IndexError:
            cts_error('Missing verification command details')
        return command_type, command_location

    def __verify_command_args(self, external_cmd):
        command_args = None
        try:
            command_args = self.__generate_dict_from_custom(external_cmd[2:])
        except IndexError:
            pass
        return command_args

    @staticmethod
    def __get_retry_information(action):
        """
        :param action:
        :return: retry_if_fail, retry_timeout
        """
        retry_if_fail, retry_timeout = 0, 1
        try:
            retry_if_fail, retry_timeout = action['action']['retry_if_fail'], \
                                           action['action']['retry_timeout']
        except KeyError:
            pass
        return retry_if_fail, retry_timeout

    #  HELPERS
    @staticmethod
    def __generate_dict_from_custom(list_values):
        dict_from_list = {}
        for element in list_values:
            k, v = element.split(":")
            dict_from_list[k] = v
        return dict_from_list

    @staticmethod
    def __merge_dicts(a, b):
        c = a.copy()
        c.update(b)
        return c


ValidateUseCase.service_cli()
