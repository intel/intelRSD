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
import errno
import glob

from cts_framework.actions.action import Action
from cts_framework.helpers.vars.env_consts import Constants

from os import makedirs
from os.path import exists
import pkg_resources


class TestsDumpAction(Action):
    ACTION = "dump"
    PARAM_NAME = "TESTS_CMD"

    DEFAULT_DIR = Constants.DIR_HOME_CTS_TESTS
    TESTS_PACKAGES = Constants.TESTS_PACKAGES
    METADATA_PACKAGES = Constants.METADATA_PACKAGES
    METADATA_HOME_DIR = Constants.METADATA_HOME_DIR

    loc = []
    xmlss = []

    def process_action(self, configuration):
        self.get_package_contents()
        self.discover_local_metadata()
        self.create_folders()

    def get_package_contents(self):

        template = [x for x in pkg_resources.resource_listdir(self.TESTS_PACKAGES, "") if not x.startswith("__")]

        for folder in template:
            # dirty hack using endswith - TODO: resource_listdir lists files and dirs, not only dirs
            dirs = [x for x in pkg_resources.resource_listdir(self.TESTS_PACKAGES, folder) if not x.endswith((".py", ".pyc", ".md"))]

            for subfolder in dirs:
                req_dir = os.path.join(*(folder, subfolder))

                for test_dir in [x for x in pkg_resources.resource_listdir(self.TESTS_PACKAGES, req_dir) if not x.endswith(".pyc")]:
                    if not pkg_resources.resource_isdir(self.TESTS_PACKAGES, os.path.join(req_dir, test_dir)):
                        self.copy_to_custom_dir(req_dir, test_dir)
                    else:
                        for archeo_test_resource in [x for x in pkg_resources.resource_listdir(self.TESTS_PACKAGES, os.path.join(req_dir, test_dir))]:
                            self.copy_to_custom_dir(os.path.join(req_dir, test_dir), archeo_test_resource)

    def copy_to_custom_dir(self, dirs, filename):
        tests_dir = os.path.join(*(self.DEFAULT_DIR, dirs))

        try:
            if not exists(tests_dir):
                makedirs(tests_dir)

            if not exists(self.DEFAULT_DIR):
                makedirs(self.DEFAULT_DIR)
        except OSError as ose:
            if ose.errno != errno.EEXIST:
                raise

        file_sub_location = os.path.join(*(dirs, filename))
        file_loc = pkg_resources.resource_string(self.TESTS_PACKAGES, file_sub_location)

        with open(os.path.join(*(tests_dir, filename)), 'w') as resource:
            resource.write(file_loc)

    def create_folders(self):
        if not exists(self.METADATA_HOME_DIR):
            makedirs(self.METADATA_HOME_DIR)

        # location
        for l in self.loc:
            path = os.path.join(self.METADATA_HOME_DIR, l.replace("_", "."))
            if not exists(path):
                makedirs(path)

        # file
        for f in self.xmlss:
            meta_loc = pkg_resources.resource_string(
                self.METADATA_PACKAGES, f
            )
            relative_path, filename = self.__divide_path(f)

            with open('/'.join((self.METADATA_HOME_DIR, relative_path.replace('_', '.'), filename)), 'w') as resource:
                resource.write(meta_loc)

    @staticmethod
    def __divide_path(relative_path):
        """
        :param relative_path:
        :return: relative path without filename, filename
        """
        divided_path = relative_path.split('/')
        return '/'.join(divided_path[:-1]), divided_path[-1]

    def discover_local_metadata(self, directory=None):
        if not directory:
            directory = ""

        folders = [x for x in pkg_resources.resource_listdir(
            self.METADATA_PACKAGES, directory + "/") if not x.startswith("__")]
        for fold in folders:
            if pkg_resources.resource_isdir(self.METADATA_PACKAGES, fold + "/"):
                self.discover_local_metadata(fold)
            else:
                folder_name = "/".join((directory, fold))
                self.loc.append(folder_name)
                xml = [x for x in pkg_resources.resource_listdir(
                    self.METADATA_PACKAGES, folder_name + "/") if not x.startswith("__")]

                for files in xml:
                    if files.endswith(".xml"):
                        self.xmlss.append("/".join((folder_name, files)))
