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
import errno
import sys

from subprocess import Popen, PIPE

from nvme_framework.helpers.constants import Constants
from nvme_framework.helpers.message import Message


class Precheck:
    REQUIRED_NVME_MODULE_LIST = ['nvme_rdma',
                                 'nvme_fabrics']

    SUPPORTED_VERSION_NVME_CLI = {'min': 1.3,
                                  'max': 1.7,
                                  'recommend': 1.3}

    def __init__(self):
        self._all_modules_are_present = self._check_nvme_cli_modules()
        self._proper_nvme_cli_version = self._check_nvme_cli_version()

        if not self._all_modules_are_present or not self._proper_nvme_cli_version:
            Message.error_color("Unfulfilled part of the requirements. Exiting NVMe-Wheel... ")
            sys.exit(errno.EPERM)

    def _check_nvme_cli_modules(self):
        local_modules = self.__external_command(True, Constants.LSMOD_GREP_NVME)
        available_modules = []

        for module in local_modules:
            try:
                if module.split()[0] in self.REQUIRED_NVME_MODULE_LIST:
                    available_modules.append(module)
            except IndexError:
                continue

        if len(available_modules) != len(self.REQUIRED_NVME_MODULE_LIST):
            Message.error_color("Missing module: {missing}".
                                format(missing=list(set(self.REQUIRED_NVME_MODULE_LIST) - set(available_modules))))
            return False
        return True

    def _check_nvme_cli_version(self):
        try:
            nvme_version = float(self.__external_command(False, Constants.NVME_VERSION).split()[2])
        except (ValueError, IndexError):
            self.__print_information_about_supported_version_nvme_cli()
            return False

        if not (self.SUPPORTED_VERSION_NVME_CLI['min'] <= nvme_version <= self.SUPPORTED_VERSION_NVME_CLI['max']):
            self.__print_information_about_supported_version_nvme_cli()
            return False

        if nvme_version != self.SUPPORTED_VERSION_NVME_CLI['recommend']:
            Message.warning_color('Recommended version of nvme-cli == {recommend_version}, found {version}'.
                                  format(recommend_version=self.SUPPORTED_VERSION_NVME_CLI['recommend'],
                                         version=nvme_version))
        return True

    @staticmethod
    def __external_command(return_as_list, *args):
        process = Popen(' '.join(args), stdout=PIPE, shell=True)
        output = []
        for line in process.stdout.readlines():
            output.append(line)

        if return_as_list:
            return output
        return ' '.join(output)

    def __print_information_about_supported_version_nvme_cli(self):
        Message.info('Supported versions of nvme-cli:')
        for key, value in self.SUPPORTED_VERSION_NVME_CLI.iteritems():
            Message.info('{key}: {value}'.format(key=key, value=value))

        Message.error_color('Can not find a proper version of nvme-cli')
