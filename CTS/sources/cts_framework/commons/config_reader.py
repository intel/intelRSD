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

import os.path

from cts_core.commons.error import cts_error


def get_configuration_from_files(*configuration_files):
    configuration = dict()
    for configuration_file in configuration_files:
        config = get_configuration_from_file(configuration_file)
        if config is not None:
            configuration.update(config)
    return configuration


def get_configuration_from_file(configuration_file):
    configuration = dict()
    file_config = ConfigParser.ConfigParser()
    file_config.optionxform = str
    try:
        file_config.read(configuration_file)
    except Exception as err:
        cts_error("Unable to parse configuration file. Err={err:exception}", err=err)
        return None

    for section_name in file_config.sections():
        for param_name, param_value in file_config.items(section_name):
            configuration[param_name] = param_value

    return configuration


def certificates_files_are_valid(config_file):
    certificates_files = []
    needed_cert_files = ["CertificateCertFile", "CertificateKeyFile"]
    for x in needed_cert_files:
        try:
            certificates_files.append(config_file[x])
        except KeyError:
            pass

    #if CertFile or KeyFile was not added, rise alarm
    if len(certificates_files) == 1:
        cts_error("Missing certificate element in config file (Please, check that your config has both: {err}",
                  err=needed_cert_files)
        return False

    for f in certificates_files:
        if not os.path.isfile(f):
            cts_error("Certificate error: {cert_path} don't exist", cert_path=f)
            return False
    return True
