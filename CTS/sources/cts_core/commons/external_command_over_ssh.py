"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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
import paramiko
"""
Example:
    with ExternalConnectionOverJumpHost() as cmd:
        _, stdout, _ = cmd.exec_command("ls -la")
        print(stdout.read())
"""


class ExternalCommandOverSSH:
    """This class is tailored for context manager"""
    def __init__(self, target_hostname, target_username, target_password, target_port=22,
                 second_hostname=None, second_username=None, second_password=None, second_port=22):
        """
        Allow to make a connection over SSH into target machine. If you need a connection over jumphost, provide
        second pair of credentials (first credentials will be your jumphost).

        :param target_hostname:
        :param target_username:
        :param target_password:
        :param target_port:
        :param second_hostname:
        :param second_username:
        :param second_password:
        :param second_port:
        """
        self.target = {
            'hostname': target_hostname,
            'username': target_username,
            'password': target_password,
            'port': int(target_port)
        }

        self.jump = {
            'hostname': second_hostname,
            'username': second_username,
            'password': second_password,
            'port': int(second_port)
        }

    def __enter__(self):
        connection_channel = None

        if self.jump['hostname']:
            self._jump_connection = paramiko.SSHClient()
            self._jump_connection.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            self._jump_connection.connect(**self.jump)

            connection_transport = self._jump_connection.get_transport()
            connection_channel = connection_transport.open_channel('direct-tcpip',
                                                                   (self.target['hostname'], self.target['port']),
                                                                   (self.jump['hostname'], self.jump['port']))
        self._jump_host = paramiko.SSHClient()
        self._jump_host.set_missing_host_key_policy(paramiko.AutoAddPolicy())

        if self.jump['hostname']:
            del(self.target['port'])
            self.target['sock'] = connection_channel

        self._jump_host.connect(**self.target)
        return self._jump_host

    def __exit__(self, exc_type, exc_val, exc_tb):
        try:
            self._jump_host.close()
            self._jump_connection.close()
        except AttributeError:
            pass
