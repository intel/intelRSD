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
import hashlib

import os
from os import getenv
from cts_core.commons.error import cts_error
from cts_framework.build_information import BuildInformation

MAX_SIZE = 2 << 16


class DirDigest(dict):
    LABEL_METADATA = 'METADATA'
    LABEL_TESTS = 'TESTS'
    SKIP_FILES = ['__init__.py']

    def __init__(self, directory, suffix, label):
        self.directory = directory
        self.suffix = suffix
        self.label = label
        self._digest = None

        self._compute()

    @property
    def digest(self):
        return self._digest

    @property
    def official_digest(self):
        return BuildInformation.get_digest(self.label)

    def is_valid(self):
        return self.official_digest == self.digest

    def update_build_information(self, file_to_udpate):
        with open(file_to_udpate, "a") as f:
            f.write("    {label}_HASH = '{value}'\n".format(label=self.label, value=self.digest))
            f.write("    {label}_FILES = {value}\n\n".format(label=self.label, value=repr(dict(self))))

    def _compute(self):
        cur_dir = os.getcwd()
        try:
            os.chdir(self.directory)
            files = self._input_files()
            md5s = map(lambda file: FileDigest(file)._compute(), files)
            self.update(zip(files, md5s))  # file to md5 mapping

            md5 = hashlib.md5()
            md5.update(''.join(md5s))
            self._digest = md5.hexdigest()  # dir md5
        finally:
            os.chdir(cur_dir)

    def _input_files(self):
        out = []
        for root, dirs, files in os.walk('.'):
            for filename in files:
                if filename in DirDigest.SKIP_FILES:
                    continue
                if not filename.endswith(self.suffix):
                    continue
                path = os.path.join(root, filename)
                out.append(path)
        return sorted(out)

    def report_differences(self):
        if getenv("CTS_SKIP", None):
            return

        files_detected = set(self.keys())
        official_files = BuildInformation.get_files(self.label)
        official_file_names = set(official_files.keys())

        missing_files = official_file_names.difference(files_detected)
        for file in missing_files:
            cts_error("Missing file: {file}", file=file)

        unknown_files = files_detected.difference(official_file_names)
        for file in unknown_files:
            cts_error("Unknown file detected: {file}", file=file)

        for file_name in files_detected:
            try:
                current_hash = dict.__getitem__(self, file_name)
                expected_hash = official_files[file_name]
                if current_hash != expected_hash:
                    cts_error("{name} has been modified", name=file_name)
            except KeyError:
                pass

class FileDigest:
    def __init__(self, path):
        self.path = path
        self.hash_file = "%s.md5" % path
        self.current_digest = self._compute()

    def is_md5_correct(self, md5):
        return self.current_digest == md5

    def _compute(self):
        computer = hashlib.md5()
        try:
            with open(self.path, 'rb') as f:
                buf = f.read(MAX_SIZE)
                while (buf):
                    computer.update(buf)
                    buf = f.read(MAX_SIZE)
            return computer.hexdigest()
        except IOError:
            return None


