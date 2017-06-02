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

ALIAS="PodManagerClient"
TMP_DIRECTORY="/tmp"
CONFIGURATION_DIRECTORY="/var/lib/pod-manager"
# place where client.jks file will be created, working directory
WORK_LOCATION=${TMP_DIRECTORY}"/keystore"
# common name that will be used during certificate generation process
COMMON_NAME_CLIENT_FILES="client"

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

function are_equal() {
  local first=$1
  local second=$2

  [[ "${first}" -eq "${second}" ]]
}

function is_valid_file() {
  local var=$1
  [[ -f ${var} ]]
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

  if [ -f ${WORK_LOCATION}/${COMMON_NAME_CLIENT_FILES}.jks ]; then
    local result=$(keytool -keystore ${COMMON_NAME_CLIENT_FILES}.jks -list -alias ${entry_to_delete} -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD})
    if [[ "$result" =~ "Alias <${entry_to_delete}> does not exist" ]]; then
      return
    fi

    keytool -delete -alias ${entry_to_delete} -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${COMMON_NAME_CLIENT_FILES}.jks -keyalg ec -sigalg SHA256withECDSA -validity 365
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
