# Copyright (c) 2016-2018 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import logging
import os
import ssl
import subprocess
import tempfile


class SecurityContextInitializer:
    _devnull = open(os.devnull, 'w')
    temp = tempfile.mkdtemp()

    _java_key_store_location = '/var/lib/pod-manager/client.jks'
    _vault_decrypter_location = '/usr/lib/pod-manager/vault-decrypter.jar'
    _client_pkcs_file = str(temp) + '/temp_client.p12'
    _client_pem_file = str(temp) + '/client.pem'

    @staticmethod
    def _retrieve_vault_password():
        password = subprocess.check_output([
            'java',
            '-jar',
            SecurityContextInitializer._vault_decrypter_location,
            '--password-type',
            'keystore'
        ])

        return password.strip().decode('utf-8')

    @staticmethod
    def _convert_jks_to_pem(pem_file_name):
        password = SecurityContextInitializer._retrieve_vault_password()
        subprocess.check_output(['rm', '-f', SecurityContextInitializer._client_pkcs_file])
        subprocess.check_output('echo {0} | keytool -storepass {0} -alias podmanagerclient -importkeystore -srckeystore {1} -srcstoretype JKS -destkeystore {2} -deststoretype pkcs12'.format(password, SecurityContextInitializer._java_key_store_location, SecurityContextInitializer._client_pkcs_file), shell=True, stderr=SecurityContextInitializer._devnull)
        subprocess.check_output(['openssl', 'pkcs12', '-in', SecurityContextInitializer._client_pkcs_file, '-passin', 'pass:{}'.format(password), '-out', pem_file_name, '-nodes', '-clcerts'], stderr=SecurityContextInitializer._devnull)
        os.remove(SecurityContextInitializer._client_pkcs_file)

    @staticmethod
    def init_security_context():
        logging.debug('========== Initialising security context')

        temporary_pem_file_name = SecurityContextInitializer._client_pem_file
        context = ssl.SSLContext(ssl.PROTOCOL_SSLv23)
        SecurityContextInitializer._convert_jks_to_pem(temporary_pem_file_name)
        context.load_cert_chain(temporary_pem_file_name)
        os.remove(temporary_pem_file_name)

        context.verify_mode = ssl.CERT_NONE

        return context
