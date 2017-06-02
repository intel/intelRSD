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
CA_CERTIFICATE=
CLIENT_CERTIFICATE=
TMP_DIRECTORY="/tmp"
CA_KEY_FILE_NAME=root.key
CA_CERTIFICATE_FILE_NAME=root.crt
CONFIGURATION_DIRECTORY="/var/lib/pod-manager"
# place where client.jks file will be created, working directory
WORK_LOCATION=${TMP_DIRECTORY}"/keystore"
# common name that will be used during certificate generation process
COMMON_NAME_CLIENT_FILES="client"
USER_NAME="podm"

function usage() {
  local bold=$(tput bold)
  local normal=$(tput sgr0)
  local script_name=`basename $0`

  cat << EOF
  Usage: ${script_name} -s|--ca-certificate <ca-certificate> [-c|--client-certificate <client-certificate>] [-v|--verbose] [-h|--help]

  Creates client certificate and signs it with provided or created CA certificate.

    ${bold}-s|--ca-certificate${normal}          CA certificate. Can be either a subject string in format "/C=PL/ST=State/L=locality/O=organization/CN=fqdn_or_ip"
                                 or path to provided CA certificate, eg. /path/to/ca/file/ca.pfx
    ${bold}-c|--client-certificate${normal}      Dname string describing client certificate, eg. "C=PL,ST=State,L=locality,O=organization,CN=fqdn_or_ip"
                                 When this parameter is omitted, then interactive mode is enabled.
    ${bold}-v|--verbose${normal}                 Enables verbosity.
    ${bold}-h|--help${normal}                    Prints this message.

  Examples:
  1) Create client certificate and sign it with created CA certificate.
     Your client certificate will be created in ${CONFIGURATION_DIRECTORY}. DO NOT move *.jks file from that location.
     Your CA certificate will be created in ${CONFIGURATION_DIRECTORY}/ca. Keystore file will be created using 'keytool'.
     For further information please visit: https://docs.oracle.com/javase/8/docs/technotes/tools/unix/keytool.html

     ${bold}sudo ${script_name} --ca-certificate "/C=PL/ST=State/L=locality/O=organization/CN=fqdn_or_ip" --client-certificate "C=PL,ST=State,L=locality,O=organization,CN=fqdn_or_ip"${normal}

  2) Create client certificate and sign it with provided CA certificate.
     Your client certificate will be created in ${CONFIGURATION_DIRECTORY}. DO NOT move *.jks file from that location.

     ${bold}sudo ${script_name} --ca-certificate /path/to/ca/file/ca.pfx --client-certificate "C=PL,ST=State,L=locality,O=organization,CN=fqdn_or_ip"${normal}

  3) Create client certificate and sign it with provided CA certificate - using interactive mode.
     Your certificate will be created in ${CONFIGURATION_DIRECTORY}. DO NOT move *.jks file from that location.

     ${bold}sudo ${script_name} --ca-certificate /path/to/ca/file/ca.pfx${normal}

EOF

  exit 1
}

function copy_important_files() {
  evaluate_command \
   "cp ${COMMON_NAME_CLIENT_FILES}.jks ${CONFIGURATION_DIRECTORY}" \
   "Copying keystore to ${CONFIGURATION_DIRECTORY} - for PODM usage" \
   ${VERBOSITY}

  evaluate_command \
    "cp -r ca/* ${CONFIGURATION_DIRECTORY}/ca && chown -R ${USER_NAME}:${USER_NAME} ${CONFIGURATION_DIRECTORY}/${COMMON_NAME_CLIENT_FILES}.jks && chown -R ${USER_NAME}:${USER_NAME} ${CONFIGURATION_DIRECTORY}/ca/*" \
    "Copying CA files to ${CONFIGURATION_DIRECTORY}/ca" \
    ${VERBOSITY}

  evaluate_command \
    "rm -f ${CONFIGURATION_DIRECTORY}/ca/*.srl" \
    "Removing useless files from ${CONFIGURATION_DIRECTORY}/ca" \
    ${VERBOSITY}
}

function create_base_certificate() {
  local dname_option=""

  if variable_is_set ${CLIENT_CERTIFICATE}; then
    dname_option="-dname \"$CLIENT_CERTIFICATE\""
  fi
  remove_from_keystore ${ALIAS}
  evaluate_command \
    "keytool -genkeypair $dname_option -alias ${ALIAS} -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${COMMON_NAME_CLIENT_FILES}.jks -keyalg ec -sigalg SHA256withECDSA -validity 900" \
    "Creating base certificate" \
    ${VERBOSITY}
}

function create_csr() {
  evaluate_command \
    "keytool -keystore ${COMMON_NAME_CLIENT_FILES}.jks -certreq -alias ${ALIAS} -keyalg ec -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -file ${COMMON_NAME_CLIENT_FILES}.csr" \
    "Generating certificate signing request" \
    ${VERBOSITY}
}

function create_CA() {
  local subject_option=""


  if [ ! -z ${CLIENT_CERTIFICATE} ]; then
    subject_option="-subj ${CA_CERTIFICATE}"
  fi

  evaluate_command \
    "openssl ecparam -name secp521r1 -genkey -out ca/${CA_KEY_FILE_NAME}" \
    "Generating key" \
    ${VERBOSITY}

  evaluate_command \
    "openssl req ${subject_option} -new -x509 -sha256 -days 365 -key ca/${CA_KEY_FILE_NAME} -out ca/${CA_CERTIFICATE_FILE_NAME} -passin pass:${JKS_PASSWORD} -passout pass:${JKS_PASSWORD} -sha256 -days 900" \
    "Generating CA certificate signed with generated key" \
    ${VERBOSITY}

  evaluate_command \
    "openssl pkcs12 -export -out ca/root.pfx -inkey ca/${CA_KEY_FILE_NAME} -in ca/${CA_CERTIFICATE_FILE_NAME} -certfile ca/${CA_CERTIFICATE_FILE_NAME}" \
    "Exporting key and certificate to pfx file" \
    1
}

function retrieve_key_and_certificate_from_ca() {
  local provided_ca_file=${CA_CERTIFICATE}
  local ca_file_base_name=$(basename "${CA_CERTIFICATE}" %.*)
  local ca_private_key=${ca_file_base_name%.*}.key
  CA_CERTIFICATE_FILE_NAME=${ca_file_base_name%.*}.crt
  CA_KEY_FILE_NAME=${ca_private_key}

  read -s -p "Please provide password of CA keystore file: " CA_PASSWD
  evaluate_command \
    "openssl pkcs12 -nokeys -in ${provided_ca_file} -passin pass:${CA_PASSWD} -out ${WORK_LOCATION}/ca/${CA_CERTIFICATE_FILE_NAME}" \
    "Exporting provided CA certificate" \
    ${VERBOSITY}

  evaluate_command \
    "openssl pkcs12 -nocerts -in ${provided_ca_file} -passin pass:${CA_PASSWD} -out ${WORK_LOCATION}/ca/${CA_KEY_FILE_NAME} -nodes" \
    "Export provided CA private key" \
    ${VERBOSITY}
}

function sign_certificate() {
  local ca_certificate_file_path=${WORK_LOCATION}/ca/${CA_CERTIFICATE_FILE_NAME}
  local ca_key_file_path=${WORK_LOCATION}/ca/${CA_KEY_FILE_NAME}

  read -s -p "Please provide CA password: " CA_PASSWD
  evaluate_command \
    "openssl x509 -req -CA ${ca_certificate_file_path} -CAkey ${ca_key_file_path} -passin pass:${CA_PASSWD} -in ${COMMON_NAME_CLIENT_FILES}.csr -out ${COMMON_NAME_CLIENT_FILES}.crt -CAcreateserial -days 900"\
    "Signing certificate with provided password" \
    ${VERBOSITY}
}

function add_signed_cert_to_keystore() {
  local root_alias=root

  remove_from_keystore ${root_alias}
  evaluate_command \
    "keytool -import -trustcacerts -noprompt -keystore ${COMMON_NAME_CLIENT_FILES}.jks -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -file ca/${CA_CERTIFICATE_FILE_NAME} -alias ${root_alias}" \
    "Importing CA root certificate to keystore" \
    ${VERBOSITY}

  evaluate_command \
    "keytool -import -trustcacerts -noprompt -keystore ${COMMON_NAME_CLIENT_FILES}.jks -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -file ${COMMON_NAME_CLIENT_FILES}.crt -alias ${ALIAS}" \
    "Importing ${ALIAS} certificate to keystore" \
    ${VERBOSITY}
}

function manage_certificates() {
  cd ${WORK_LOCATION}
  create_base_certificate

  if is_valid_file ${CA_CERTIFICATE}; then
    retrieve_key_and_certificate_from_ca
  else
    create_CA
  fi

  create_csr
  sign_certificate
  add_signed_cert_to_keystore
}

function validate_args() {
  if ! variable_is_set ${CA_CERTIFICATE}; then
    print_error "'--ca-certificate' option is required."
    usage
  fi
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
        -c|--client-certificate)
            if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
                print_error "The ${opt} option requires an argument."
                usage
            fi
            CLIENT_CERTIFICATE="$1"
            shift
            ;;
        -s|--ca-certificate)
            if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
                print_error "The ${opt} option requires an argument."
                usage
            fi
            CA_CERTIFICATE="$1"
            shift
            ;;
        -v|--verbose)
            VERBOSITY=1
            ;;
        -h|--help)
            usage;;
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
retrieve_vault_password ${VERBOSITY}
manage_certificates
copy_important_files
