#!/usr/bin/env python3

# Copyright (c) 2017 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import enum
import os
import re
import subprocess
import sys


def init_parser():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description='''
compare given public key with debian package and check if its signature is correct

COMMANDS:
    check-signature <public_key_file> <deb_package_file>
        Checks if deb package (localized in <deb_package_file>) signature conforms to public key stored in file localized in <public_key_file>.

        Prints line with status of signature check:
            * SHA-1 <fingerprint> OK, Package signature verified
            * SHA-1 <fingerprint> FAILED, Package signature verification failed
            * FAILED, Package is not signed
            * Unknown error while checking signature

        <fingerprint> is SHA-1 fingerprint of signature used to verification

    check-signature-and-install <public_key_file> <deb_package_file>
        Performs the same operation as with "check-signature" command, and then - if signature is correct - install package
        '''
    )

    parser.add_argument('--command', '-c', choices=['check-signature', 'check-signature-and-install'], required=True)

    parser.add_argument('--public-key-file', '-k', required=True)

    parser.add_argument('--deb-package-file', '-p', required=True)

    return parser


def import_public_key(public_key_file_name):
    subprocess.call(['gpg', '--import', public_key_file_name], stderr=subprocess.PIPE)


def get_fingerprint(public_key_file_name):
    pipe = subprocess.Popen(['gpg', '--with-fingerprint', '--with-colons', public_key_file_name], stdout=subprocess.PIPE)
    output = pipe.communicate()[0]
    output_string = output.decode('utf-8')
    fingerprint = re.findall(r'fpr:.*?:.*?:.*?:.*?:.*?:.*?:.*?:.*?:(.*?):', output_string)[-1]
    return fingerprint


class Status(enum.Enum):
    OK = 1
    FAILED = 2
    NOT_SIGNED = 3


class DpkgSigReturnCodes(enum.Enum):
    GOOD_SIGNATURE = 0


def get_gpg_version():
    pipe = subprocess.Popen(['gpg', '--version'], stdout=subprocess.PIPE)
    output = pipe.communicate()[0]
    output_string = output.decode('utf-8')
    version = output_string.split()[2]
    return version


def compare_fingerprints(matched_fingerprint, valid_fingerprint):
    return Status.OK if matched_fingerprint == valid_fingerprint else Status.FAILED


def check_output_when_good_signature_code_was_returned(output_lines, valid_fingerprint):
    if len(output_lines) > 1:
        matched_fingerprint = output_lines[1].split()[2]
        status = compare_fingerprints(matched_fingerprint, valid_fingerprint)
    else:
        status = Status.NOT_SIGNED

    return status


def verify_signature(deb_package_file_name, valid_fingerprint):
    pipe = subprocess.Popen(['dpkg-sig', '--verify', deb_package_file_name], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output = pipe.communicate()[0]
    output_lines = output.decode('utf-8').splitlines()

    if pipe.returncode == DpkgSigReturnCodes.GOOD_SIGNATURE.value:
        status = check_output_when_good_signature_code_was_returned(output_lines, valid_fingerprint)
    else:
        status = Status.FAILED

    return status


def check_signature(public_key_file_name, deb_package_file_name):
    print('Checking package signature:')

    communicates = {
        Status.OK: 'SHA-1 {fingerprint} OK, Package signature verified',
        Status.FAILED: 'SHA-1 {fingerprint} FAILED, Package signature verification failed',
        Status.NOT_SIGNED: 'FAILED, Package is not signed'
    }

    import_public_key(public_key_file_name)
    fingerprint = get_fingerprint(public_key_file_name)
    status = verify_signature(deb_package_file_name, fingerprint)

    communicate = communicates.get(status, 'Unknown error while checking signature')

    communicate = communicate.format(fingerprint=fingerprint)

    print(communicate)

    if status != Status.OK:
        sys.exit()


def install(deb_package_file_name):
    print('Installing Pod Manager:')
    subprocess.call(['sudo', 'dpkg', '--install', deb_package_file_name])


def validate_arguments(arguments):
    if not os.path.exists(arguments.public_key_file):
        sys.exit('Provided public key file does not exist!')

    if not os.path.exists(arguments.deb_package_file):
        sys.exit('Provided deb package file does not exist!')


def check_gpg_version():
    supported_gpg_versions = ['1.4.20']
    gpg_version = get_gpg_version()
    if not gpg_version in supported_gpg_versions:
        sys.exit('ERROR: supported gpg versions: {supported_gpg_versions}, found gpg version: {gpg_version}'.format(supported_gpg_versions=supported_gpg_versions, gpg_version=gpg_version))


def main():
    check_gpg_version()

    parser = init_parser()
    arguments = parser.parse_args()

    validate_arguments(arguments)

    if arguments.command == 'check-signature':
        check_signature(arguments.public_key_file, arguments.deb_package_file)
    elif arguments.command == 'check-signature-and-install':
        check_signature(arguments.public_key_file, arguments.deb_package_file)
        install(arguments.deb_package_file)

if __name__ == '__main__':
    main()
