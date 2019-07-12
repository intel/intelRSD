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
import os
import shutil
import requests
import tarfile
import subprocess
import math

from configparser import ConfigParser
from cts_framework.helpers.vars.env_consts import Constants
from cts_framework.build_information import BuildInformation


class Update:
    def __init__(self):
        self.update_config = None

    def read_configuration_from_file(self, section="cts_update_server"):
        config_dict = {}
        config = ConfigParser()

        config.read(Constants.CTS_CONFIGURATION_FILE_UPDATE)
        options = config.options(section)
        for option in options:
            try:
                config_dict[option] = config.get(section, option)
            except KeyError:
                pass
        if not all(config_dict.values()):
            print('Config file is missing or corrupt.\nGenerate it with:\n\ncts version --generate')
            exit(1)
        self.update_config = config_dict

    def check_new_version_is_available(self):
        latest_path = "http://{host}/api/v1/latest".format(host=self.update_config['host'].replace("http://", ""))

        try:
            response = requests.get(latest_path,
                                    auth=(self.update_config['user'],
                                          self.update_config['password'])).json()
        except:
            print('Error while fetching information about new version.'
                  '\nUpdate server is not responding or login details are incorrect.')
            exit(1)

        if BuildInformation.get_version() >= response['latest']:
            print('CTS Version: %s\nNewest version is installed' % BuildInformation.get_version())
            return None

        print('Installed version of CTS: %s\nA new version is available: %s' %
              (BuildInformation.get_version(), response['latest']))
        return response['latest_link']

    def download_latest_package(self, download_link):
        response = requests.get("http://{host}/api/v1/{link}".format(host=self.update_config['host'],
                                                                     link=download_link), stream=True)
        cts_package_size = int(response.headers.get('content-length', 0))
        download_animation = False
        try:
            from tqdm import tqdm
            download_animation = True
        except ImportError:
            pass

        # TODO add md5 verification
        with open(Constants.CTS_FILE_UPDATE, "wb") as handle:
            if download_animation:
                for data in tqdm(response.iter_content(1024), total=math.ceil(cts_package_size//1024), unit='KB', unit_scale=True, desc="Downloading"):
                    handle.write(data)
            else:
                handle.write(response.content)

        # unpack archive
        tarfile.open(Constants.CTS_FILE_UPDATE).extractall(Constants.CTS_UPDATE_DIR)

    def print_manual_instruction(self):
        cts_dir_with_up = [e for e in os.listdir(Constants.CTS_UPDATE_DIR)
                           if os.path.isdir(os.path.join(Constants.CTS_UPDATE_DIR, e))]
        link_to_package_dir = os.path.join(*(Constants.CTS_UPDATE_DIR, cts_dir_with_up[0]))

        manual_cmd = 'sudo bash %s/%s' % (link_to_package_dir, "INSTALL.sh")
        print('\nConformance Test Suite (CTS) is ready to update.\n')

        self.copy_command_to_clipboard(manual_cmd)
        if not raw_input('Do you want install update now? [y/N]').lower() in ['y', 'yes']:
            print('For manual process run command in terminal:\n\n%s\n\n' % manual_cmd)
            print('Exiting from CTS update module..')
        else:
            if not self.run_external_installation_script("INSTALL.sh", link_to_package_dir):
                print('\nCTS wasn\'t installed automatically.')
                print('For manual process run command in terminal:\n\n%s\n\n' % manual_cmd)

    @staticmethod
    def remove_old_packages_and_folders():
        try:
            os.remove(Constants.CTS_FILE_UPDATE)
            shutil.rmtree(os.path.join(Constants.DIR_HOME_CTS, "update"), ignore_errors=True)
        except OSError:
            pass

    @staticmethod
    def create_update_dir():
        try:
            os.mkdir(os.path.join(Constants.DIR_HOME_CTS, "update"))
        except OSError:
            pass

    @staticmethod
    def run_external_installation_script(command, working_dir):
        try:
            subprocess.call(["sudo", "bash", command], cwd=working_dir)
        except:
            return False
        return True

    @staticmethod
    def copy_command_to_clipboard(command):
        try:
            from Tkinter import Tk
            r = Tk()
            r.withdraw()
            r.clipboard_clear()
            r.clipboard_append(command)
            r.update()
            r.destroy()
        except ImportError:
            pass

    def generate_update_config_ini(self):
        if os.path.exists(Constants.CTS_CONFIGURATION_FILE_UPDATE):
            print("Config file for update process exist!")
            if not raw_input('Overwrite? [y/N]').lower() in ['y', 'yes']:
                print('\nFile was not changed')
                exit(0)

        update_config_ini = "[cts_update_server]\nhost=%s\nuser=%s\npassword=%s" % \
                            self.read_information_from_raw_input()

        with open(Constants.CTS_CONFIGURATION_FILE_UPDATE, 'wb') as handle:
            handle.write(update_config_ini)

        if not os.path.isfile(Constants.CTS_CONFIGURATION_FILE_UPDATE):
            print('\nCTS can not generate config file for update process')
            exit(1)
        print('\nConfig file for update process was generated:\n%s' % Constants.CTS_CONFIGURATION_FILE_UPDATE)

    @staticmethod
    def read_information_from_raw_input():
        questions = {"host:port": None, "username": None, "password": None}
        print("\nPlease provide:")
        for q, _ in questions.items():
            questions[q] = raw_input(str("{}: ".format(q)))

        return questions["host:port"], questions["username"], questions["password"]

