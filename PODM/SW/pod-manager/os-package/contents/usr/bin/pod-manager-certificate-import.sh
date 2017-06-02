#!/usr/bin/env bash

# Copyright (c) 2015-2017 Intel Corporation
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

#############################################################
# Script to prepare certificate and corresponding keystore
#############################################################

source $(dirname $0)/../lib/pod-manager/certificate-common-functions.sh

VERBOSITY=
CERTIFICATE_FILE=
CA_KEY_FILE_NAME=root.key
CA_CERTIFICATE_FILE_NAME=root.crt

function usage() {
  local bold=$(tput bold)
  local normal=$(tput sgr0)
  local script_name=`basename $0`

  cat << EOF
  Usage: ${script_name} -c|--certificate-file <file> [-v|--verbose] [-h|--help]

  Creates new keystore, imports given certificate to it, and copies previously created keystore to ${CONFIGURATION_DIRECTORY}

    ${bold}-c|--certificate-file${normal}      Filepath to certificate
    ${bold}-v|--verbose${normal}               Enables verbosity
    ${bold}-h|--help${normal}                  Prints this message

  Example:
    ${bold}sudo ${script_name} -c /path/to/ca/file/ca.pfx${normal}

EOF

  exit 1
}

function create_keystore() {
  evaluate_command \
    "keytool -genkey -alias removable -noprompt -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${COMMON_NAME_CLIENT_FILES}.jks -dname \"CN=PodManager\"" \
    "Creating keystore"\
    ${VERBOSITY}

  evaluate_command \
    "keytool -delete -alias removable -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${COMMON_NAME_CLIENT_FILES}.jks" \
    "Removing root alias" \
    ${VERBOSITY}
}

function import_signed_certificate() {
  evaluate_command \
    "keytool -importkeystore -srckeystore ${CERTIFICATE_FILE} -srcstoretype pkcs12 -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -destkeystore ${COMMON_NAME_CLIENT_FILES}.jks -deststoretype JKS" \
    "Importing signed certificate to keystore" \
    ${VERBOSITY}
}

function validate_args() {
  if ! variable_is_set ${CERTIFICATE_FILE}; then
    print_error "'--certificate-file' argument not passed."
    usage
  fi
}

function copy_jks() {
  evaluate_command \
   "cp ${COMMON_NAME_CLIENT_FILES}.jks ${CONFIGURATION_DIRECTORY}" \
   "Copying keystore to ${CONFIGURATION_DIRECTORY} - for PODM usage" \
   ${VERBOSITY}
}

#############################################################
# Main section
#############################################################

if ! are_equal `id -u` 0; then
  print_error "This script must be run with super-user privileges"
  usage
fi

while [ $# -gt 0 ]
do
    opt=$1
    shift
    case ${opt} in
        -c|--certificate-file)
            if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
                print_error "The ${opt} option requires an argument."
                usage
            fi
            CERTIFICATE_FILE="$1"
            shift
            ;;
        -v|--verbose)
            VERBOSITY=1
            ;;
        -h|--help)
            usage
            ;;
        *)
            if [ "${opt:0:1}" = "-" ]; then
                print_error "${opt}: unknown option."
                usage
            fi
            ;;
    esac
done
trap cleanup INT

validate_args
cleanup ${VERBOSITY}
create_work_directories ${VERBOSITY}
retrieve_vault_password
cd ${WORK_LOCATION}
create_keystore
import_signed_certificate
copy_jks
