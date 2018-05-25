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

TMP_DIRECTORY="/tmp"
CONFIGURATION_DIRECTORY="/var/lib/pod-manager"
# place where client.jks file will be created, working directory
WORK_LOCATION=${TMP_DIRECTORY}"/keystore"
BASE_CERTIFICATE_FILE_NAME=
SERVER_CERT="server"
CLIENT_CERT="client"
CERTIFICATE_TYPE=

function print_error() {
  echo "error: $@" 1>&2
}

function exit_on_error() {
  exit 1
}

function variable_is_set() {
  local variable=$1
  [[ ! -z ${variable} ]]
}

function are_strings_equal() {
  local first=$1
  local second=$2

  [[ "${first}" = "${second}" ]]
}

function are_numbers_equal() {
  local first=$1
  local second=$2

  [[ "${first}" -eq "${second}" ]]
}

function is_valid_file() {
  local var=$1
  [[ -f ${var} ]]
}

function is_client_type() {
  local var=$1
  are_strings_equal ${var} ${CLIENT_CERT}
}

function is_server_type() {
  local var=$1
  are_strings_equal ${var} ${SERVER_CERT}
}

function server_keystore_exists() {
  [[ -f "${CONFIGURATION_DIRECTORY}/${SERVER_CERT}.jks" ]]
}

function is_alias_in_use() {
  local var=$1
  result=$(keytool -list -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${CONFIGURATION_DIRECTORY}/${SERVER_CERT}.jks -alias ${var})
  [[ ${result} == ${var}* ]]
}

function evaluate_command() {
  local command="$1"
  local description="$2"
  local verbose="$3"

  variable_is_set ${verbose} && echo -n "${description}... "
  eval ${command}
  if [ $? -ne 0 ]; then
      print_error "command execution failed '${command}'"
      exit_on_error
  fi
  variable_is_set ${verbose} && echo "[DONE]"
}

function cleanup() {
  local verbose=$1

  evaluate_command \
    "rm -rf ${WORK_LOCATION}" \
    "Removing ${WORK_LOCATION} directory" \
    ${verbose}
}

function retrieve_db_password() {
  vault_decrypter_result=$(java -jar /usr/lib/pod-manager/vault-decrypter.jar --password-type database)

  if [ $? -ne 0 ]; then
    print_error "Database password has not been retrieved due to errors."
    print_error "Database can not be created."
    exit_on_error
  else
    echo ${vault_decrypter_result}
  fi
}

function retrieve_vault_password() {
  vault_decrypter_result=$(java -jar /usr/lib/pod-manager/vault-decrypter.jar --password-type keystore)

  if [ $? -ne 0 ]; then
    print_error "Keystore password has not been retrieved due to errors."
    print_error "Keystore file can not be created."
    exit_on_error
  else
    JKS_PASSWORD=${vault_decrypter_result}
  fi
}

function remove_from_keystore() {
  local entry_to_delete=$1

  if [ -f ${WORK_LOCATION}/${BASE_CERTIFICATE_FILE_NAME}.jks ]; then
    local result=$(keytool -keystore ${BASE_CERTIFICATE_FILE_NAME}.jks -list -alias ${entry_to_delete} -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD})
    if [[ "$result" =~ "Alias <${entry_to_delete}> does not exist" ]]; then
      return
    fi

    keytool -delete -alias ${entry_to_delete} -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${BASE_CERTIFICATE_FILE_NAME}.jks
  fi
}

function create_work_directories() {
  local verbosity=$1

  evaluate_command \
    "mkdir -p ${WORK_LOCATION}/ca" \
    "Creating ${WORK_LOCATION}/ca directory" \
    ${verbosity}

  evaluate_command \
    "mkdir -p ${CONFIGURATION_DIRECTORY}/ca" \
    "Creating ${CONFIGURATION_DIRECTORY}/ca directory" \
    ${verbosity}
}

function set_base_certificate_file_name() {
    BASE_CERTIFICATE_FILE_NAME=$1;
}

function list_certificates() {
    command="keytool -list -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${CONFIGURATION_DIRECTORY}/${SERVER_CERT}.jks"
    eval ${command}
}