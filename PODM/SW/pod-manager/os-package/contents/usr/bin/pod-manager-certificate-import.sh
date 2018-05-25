#!/usr/bin/env bash

# Copyright (c) 2015-2018 Intel Corporation
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
CERTIFICATE_LIST=
INSERT_TO_KEYSTORE=
CERT_ALIAS=
KEYSTORE_SERVER_PATH=

function usage() {
  local bold=$(tput bold)
  local normal=$(tput sgr0)
  local script_name=`basename $0`

  cat << EOF
  Usage: ${script_name} -t|--certificate-type <"client"|"server"> -c|--certificate-file <file> [-v|--verbose] [-h|--help]

  Creates new keystore, imports given certificate to it, and copies previously created keystore to ${CONFIGURATION_DIRECTORY}

    ${bold}-t|--certificate-type${normal}      Certificate type. Acceptable values are "client" to import client certificate or "server" to import server side certificate.
    ${bold}-c|--certificate-file${normal}      Full file path to certificate. Expected file formats are: *.pfx for signed client certificate, *.crt for server root certificate.
    ${bold}-i|--insert${normal}                Inserts certificate to existing keystore. Valid only with server certificate.
    ${bold}-a|--alias${normal}                 Alias for server certificate. Required for inserting to existing keystore.
    ${bold}-v|--verbose${normal}               Enables verbosity.
    ${bold}-h|--help${normal}                  Prints this message.

  Example:
  1. Import client ca signed certificate to PODM keystore
    ${bold}sudo ${script_name} -t "client" -c /path/to/ca/file/ca_file_name.pfx${normal}

  2. Import server ca certificate to PODM keystore
    ${bold}sudo ${script_name} -t "server" -c /path/to/ca/file/ca_file_name.crt${normal}

  3. Import server ca certificate to existing PODM keystore
    ${bold}sudo ${script_name} -t "server" -i -a "alias" -c /path/to/ca/file/ca_file_name.crt${normal}

EOF

  exit 1
}

function create_keystore() {
  evaluate_command \
    "keytool -genkey -alias removable -noprompt -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${COMMON_NAME_CERTIFICATE_FILES}.jks -dname \"CN=PodManager\"" \
    "Creating keystore"\
    ${VERBOSITY}

  evaluate_command \
    "keytool -delete -alias removable -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${COMMON_NAME_CERTIFICATE_FILES}.jks" \
    "Removing root alias" \
    ${VERBOSITY}
}

function import_signed_certificate() {
  if is_server_type ${CERTIFICATE_TYPE}; then
    if ! variable_is_set ${CERT_ALIAS}; then
        CERT_ALIAS="rootCA"
    fi
    evaluate_command \
      "keytool -import -noprompt -alias ${CERT_ALIAS} -trustcacerts -file ${CERTIFICATE_FILE} -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${KEYSTORE_SERVER_PATH}${COMMON_NAME_CERTIFICATE_FILES}.jks" \
      "Importing server root certificate to keystore" \
      ${VERBOSITY}
  else
    evaluate_command \
      "keytool -importkeystore -srckeystore ${CERTIFICATE_FILE} -srcstoretype pkcs12 -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -destkeystore ${COMMON_NAME_CERTIFICATE_FILES}.jks -deststoretype JKS" \
      "Importing signed certificate to keystore" \
      ${VERBOSITY}
  fi
}

function validate_args() {
  if ! variable_is_set ${CERTIFICATE_FILE}; then
    print_error "'--certificate-file' argument not passed."
    usage
  fi

  if ! variable_is_set ${CERTIFICATE_TYPE}; then
    print_error "'--certificate-type' argument not passed."
    usage
  fi

  if variable_is_set ${INSERT_TO_KEYSTORE} && ! is_server_type ${CERTIFICATE_TYPE} ; then
    print_error "inserting is allowed only with server certificate."
    usage
  fi

  if variable_is_set ${INSERT_TO_KEYSTORE} && ! server_keystore_exists ; then
    print_error "no keystore to append to."
    usage
  fi

  if variable_is_set ${INSERT_TO_KEYSTORE} && ! variable_is_set ${CERT_ALIAS}; then
    print_error "no certificate alias provided."
    usage
  fi

  if variable_is_set ${INSERT_TO_KEYSTORE} && is_alias_in_use ${CERT_ALIAS}; then
    print_error "alias '${CERT_ALIAS}' already exists. Please choose different one."
    usage
  fi
}

function copy_jks() {
  evaluate_command \
   "cp ${COMMON_NAME_CERTIFICATE_FILES}.jks ${CONFIGURATION_DIRECTORY}" \
   "Copying keystore to ${CONFIGURATION_DIRECTORY} - for PODM usage" \
   ${VERBOSITY}
}

#############################################################
# Main section
#############################################################

if ! are_numbers_equal `id -u` 0; then
  print_error "This script must be run with super-user privileges"
  usage
fi

while [ $# -gt 0 ]
do
    opt=$1
    shift
    case ${opt} in
        -l|--list)
            CERTIFICATE_LIST=1
            ;;
        -t|--certificate-type)
            if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
                print_error "The ${opt} option requires an argument."
                usage
            fi
            CERTIFICATE_TYPE="$1"
            COMMON_NAME_CERTIFICATE_FILES="$1"
            shift
            ;;
        -c|--certificate-file)
            if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
                print_error "The ${opt} option requires an argument."
                usage
            fi
            CERTIFICATE_FILE="$1"
            shift
            ;;
        -i|--insert)
            INSERT_TO_KEYSTORE=1
            ;;
        -a|--alias)
            CERT_ALIAS="$1"
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

retrieve_vault_password

if variable_is_set ${CERTIFICATE_LIST}; then
    list_certificates
    exit
fi

validate_args

if ! variable_is_set ${INSERT_TO_KEYSTORE}; then
    cleanup ${VERBOSITY}
    create_work_directories ${VERBOSITY}
    cd ${WORK_LOCATION}
    create_keystore
    import_signed_certificate
    copy_jks
else
    KEYSTORE_SERVER_PATH="${CONFIGURATION_DIRECTORY}/"
    import_signed_certificate
fi
