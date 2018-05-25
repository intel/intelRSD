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
CA_CERTIFICATE_DNAME=
CA_CERTIFICATE_FILE=
CERTIFICATE_DNAME=
TMP_DIRECTORY="/tmp"
CA_KEY_FILE_NAME=root.key
CA_CERTIFICATE_FILE_NAME=root.crt
CONFIGURATION_DIRECTORY="/var/lib/pod-manager"
# place where client.jks file will be created, working directory
WORK_LOCATION=${TMP_DIRECTORY}"/keystore"
USER_NAME="podm"

function usage() {
  local bold=$(tput bold)
  local normal=$(tput sgr0)
  local script_name=`basename $0`

  cat << EOF
  Usage: ${script_name} -t|--certificate-type <"client"|"server"> -s|--ca-certificate-file <ca-certificate> [-c|--certificate <certificate>] [-v|--verbose] [-h|--help]

  Creates client/server certificate and signs it with provided or created CA certificate.

    ${bold}-t|--certificate-type${normal}        Certificate type. Acceptable values are "client" to generate client certificate or "server" to generate server side certificate.
    ${bold}-c|--ca-certificate-dname${normal}    CA certificate distinguished name. String describing certificate in format "/C=PL/ST=State/L=locality/O=organization/CN=fqdn_or_ip".
    ${bold}-f|--ca-certificate-file${normal}     Path to provided CA certificate, eg. /path/to/ca/file/ca.pfx.
    ${bold}-d|--certificate-dname${normal}       Client or server certificate distinguished name. String describing certificate in format "C=PL,ST=State,L=locality,O=organization,CN=fqdn_or_ip".
                                 When this parameter is omitted, then interactive mode is enabled.
    ${bold}-v|--verbose${normal}                 Enables verbosity.
    ${bold}-h|--help${normal}                    Prints this message.

  Examples:
  1) Create certificate and sign it with created CA certificate.
     Your certificate will be created in ${CONFIGURATION_DIRECTORY}. DO NOT move *.jks file from that location.
     Your CA certificate will be created in ${CONFIGURATION_DIRECTORY}/ca. Keystore file will be created using 'keytool'.
     For further information please visit: https://docs.oracle.com/javase/8/docs/technotes/tools/unix/keytool.html

     client:
     ${bold}sudo ${script_name} --certificate-type "client" --ca-certificate-dname "/C=PL/ST=State/L=locality/O=organization/CN=fqdn_or_ip" --certificate-dname "C=PL,ST=State,L=locality,O=organization,CN=fqdn_or_ip"${normal}

     server:
     ${bold}sudo ${script_name} --certificate-type "server" --ca-certificate-dname "/C=PL/ST=State/L=locality/O=organization/CN=fqdn_or_ip" --certificate-dname "/C=PL/ST=State/L=locality/O=organization/CN=fqdn_or_ip"${normal}

  2) Create certificate and sign it with provided CA certificate.
     Your certificate will be created in ${CONFIGURATION_DIRECTORY}. DO NOT move *.jks file from that location.

     client:
     ${bold}sudo ${script_name} --certificate-type "client" --ca-certificate-file /path/to/ca/file/ca_file_name.pfx --certificate-dname "C=PL,ST=State,L=locality,O=organization,CN=fqdn_or_ip"${normal}

     server:
     ${bold}sudo ${script_name} --certificate-type "server" --ca-certificate-file /path/to/ca/file/ca_file_name.pfx --certificate-dname "/C=PL/ST=State/L=locality/O=organization/CN=fqdn_or_ip"${normal}

  3) Create certificate and sign it with provided CA certificate - using interactive mode.
     Your certificate will be created in ${CONFIGURATION_DIRECTORY}. DO NOT move *.jks file from that location.

     client:
     ${bold}sudo ${script_name} --certificate-type "client" --ca-certificate-file /path/to/ca/file/ca_file_name.pfx${normal}

     server:
     ${bold}sudo ${script_name} --certificate-type "server" --ca-certificate-file /path/to/ca/file/ca_file_name.pfx${normal}

EOF

  exit 1
}

function copy_important_files() {
  evaluate_command \
   "cp ${BASE_CERTIFICATE_FILE_NAME}.jks ${CONFIGURATION_DIRECTORY}" \
   "Copying keystore to ${CONFIGURATION_DIRECTORY} - for PODM usage" \
   ${VERBOSITY}

  if is_server_type ${CERTIFICATE_TYPE}; then
    evaluate_command \
     "cp ${BASE_CERTIFICATE_FILE_NAME}.crt ${BASE_CERTIFICATE_FILE_NAME}.key ${CONFIGURATION_DIRECTORY}" \
     "Copying server certificate and key to ${CONFIGURATION_DIRECTORY} - for PODM usage" \
     ${VERBOSITY}
  fi

  evaluate_command \
    "cp -r ca/* ${CONFIGURATION_DIRECTORY}/ca && chown -R ${USER_NAME}:${USER_NAME} ${CONFIGURATION_DIRECTORY}/${BASE_CERTIFICATE_FILE_NAME}.jks && chown -R ${USER_NAME}:${USER_NAME} ${CONFIGURATION_DIRECTORY}/ca/*" \
    "Copying CA files to ${CONFIGURATION_DIRECTORY}/ca" \
    ${VERBOSITY}

  evaluate_command \
    "rm -f ${CONFIGURATION_DIRECTORY}/ca/*.srl" \
    "Removing useless files from ${CONFIGURATION_DIRECTORY}/ca" \
    ${VERBOSITY}
}

function create_base_server_certificate() {
  local subject_option=""

  if variable_is_set ${CERTIFICATE_DNAME}; then
    subject_option="-subj \"$CERTIFICATE_DNAME\""
  fi

  remove_from_keystore ${ALIAS} ${BASE_CERTIFICATE_FILE_NAME}
  evaluate_command \
    "openssl ecparam -genkey -name secp521r1 -param_enc named_curve -out ${BASE_CERTIFICATE_FILE_NAME}.key" \
    "Creating server key file" \
    ${VERBOSITY}
  evaluate_command \
    "openssl req ${subject_option} -new -x509 -key ${BASE_CERTIFICATE_FILE_NAME}.key -days 1000 -passin pass:${JKS_PASSWORD} -passout pass:${JKS_PASSWORD} -out ${BASE_CERTIFICATE_FILE_NAME}.crt" \
    "Creating server certificate file" \
    ${VERBOSITY}
}

function create_base_certificate() {
  local dname_option=""

  if variable_is_set ${CERTIFICATE_DNAME}; then
    dname_option="-dname \"$CERTIFICATE_DNAME\""
  fi
  remove_from_keystore ${ALIAS} ${BASE_CERTIFICATE_FILE_NAME}
  evaluate_command \
    "keytool -genkeypair $dname_option -alias ${ALIAS} -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -keystore ${BASE_CERTIFICATE_FILE_NAME}.jks -keyalg ec -sigalg SHA256withECDSA -validity 900" \
    "Creating base certificate" \
    ${VERBOSITY}
}

function create_csr() {
  if is_server_type ${CERTIFICATE_TYPE}; then
     evaluate_command \
    "openssl x509 -x509toreq -in ${BASE_CERTIFICATE_FILE_NAME}.crt -signkey ${BASE_CERTIFICATE_FILE_NAME}.key -out ${BASE_CERTIFICATE_FILE_NAME}.csr" \
    "Generating certificate signing request" \
    ${VERBOSITY}
  else
    evaluate_command \
    "keytool -keystore ${BASE_CERTIFICATE_FILE_NAME}.jks -certreq -alias ${ALIAS} -keyalg ec -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -file ${BASE_CERTIFICATE_FILE_NAME}.csr" \
    "Generating certificate signing request" \
    ${VERBOSITY}
  fi
}

function create_CA() {
  local subject_option="-subj ${CA_CERTIFICATE_DNAME}"

  evaluate_command \
    "openssl ecparam -name secp521r1 -genkey -out ca/${CA_KEY_FILE_NAME}" \
    "Generating key" \
    ${VERBOSITY}

  evaluate_command \
    "openssl req ${subject_option} -new -x509 -sha256 -days 365 -key ca/${CA_KEY_FILE_NAME} -out ca/${CA_CERTIFICATE_FILE_NAME} -passin pass:${JKS_PASSWORD} -passout pass:${JKS_PASSWORD} -sha256 -days 900" \
    "Generating CA certificate signed with generated key" \
    ${VERBOSITY}

  evaluate_command \
    "openssl pkcs12 -export -out ca/root.pfx -inkey ca/${CA_KEY_FILE_NAME} -passin pass:${JKS_PASSWORD} -passout pass:${JKS_PASSWORD} -in ca/${CA_CERTIFICATE_FILE_NAME} -certfile ca/${CA_CERTIFICATE_FILE_NAME}" \
    "Exporting key and certificate to pfx file" \
    1
}

function retrieve_key_and_certificate_from_ca() {
  local provided_ca_file=${CA_CERTIFICATE_FILE}
  local ca_file_base_name=$(basename "${CA_CERTIFICATE_FILE}" %.*)
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

  evaluate_command \
    "openssl x509 -req -CA ${ca_certificate_file_path} -CAkey ${ca_key_file_path} -passin pass:${JKS_PASSWORD} -in ${BASE_CERTIFICATE_FILE_NAME}.csr -out ${BASE_CERTIFICATE_FILE_NAME}.crt -CAcreateserial -days 900"\
    "Signing certificate with provided password" \
    ${VERBOSITY}
}

function add_signed_cert_to_keystore() {
  local root_alias=root

  remove_from_keystore ${root_alias} ${BASE_CERTIFICATE_FILE_NAME}
  evaluate_command \
     "keytool -import -trustcacerts -noprompt -keystore ${BASE_CERTIFICATE_FILE_NAME}.jks -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -file ca/${CA_CERTIFICATE_FILE_NAME} -alias ${root_alias}" \
     "Importing CA root certificate to keystore" \
     ${VERBOSITY}

  if is_client_type ${CERTIFICATE_TYPE}; then
    evaluate_command \
      "keytool -import -trustcacerts -noprompt -keystore ${BASE_CERTIFICATE_FILE_NAME}.jks -keypass ${JKS_PASSWORD} -storepass ${JKS_PASSWORD} -file ${BASE_CERTIFICATE_FILE_NAME}.crt -alias ${ALIAS}" \
      "Importing ${ALIAS} certificate to keystore" \
      ${VERBOSITY}
  fi
}

function manage_certificates() {
  cd ${WORK_LOCATION}

  if is_server_type ${CERTIFICATE_TYPE}; then
    create_base_server_certificate
  else
    create_base_certificate
  fi

  if is_valid_file ${CA_CERTIFICATE_FILE}; then
    retrieve_key_and_certificate_from_ca
  else
    create_CA
  fi

  create_csr
  sign_certificate
  add_signed_cert_to_keystore
}

function validate_args() {
  if (! variable_is_set ${CA_CERTIFICATE_FILE}) && (! variable_is_set ${CA_CERTIFICATE_DNAME}); then
    print_error "'--ca-certificate-dname' or '--ca-certificate-file' option is required."
    usage
  fi

  if ! variable_is_set ${CERTIFICATE_TYPE}; then
    print_error "'--certificate-type' option is required."
    usage
  fi
}

function set_certificate_entry_alias() {
  if is_client_type ${CERTIFICATE_TYPE}; then
    ALIAS="PodManagerClient"
  else
    ALIAS="ServerCertificate"
  fi
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
        -t|--certificate-type)
            if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
                print_error "The ${opt} option requires an argument."
                usage
            fi

            if (! is_client_type $1) && (! is_server_type $1); then
                print_error "Supported certificate types are [\"client\", \"server\"]."
                usage
            fi

            CERTIFICATE_TYPE=$1;
            set_base_certificate_file_name $1
            set_certificate_entry_alias;

            shift
            ;;
        -d|--certificate-dname)
            if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
                print_error "The ${opt} option requires an argument."
                usage
            fi
            CERTIFICATE_DNAME="$1"
            shift
            ;;
        -c|--ca-certificate-dname)
            if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
                print_error "The ${opt} option requires an argument."
                usage
            fi
            CA_CERTIFICATE_DNAME="$1"
            shift
            ;;
        -f|--ca-certificate-file)
            if [ $# -eq 0 -o "${1:0:1}" = "-" ]; then
                print_error "The ${opt} option requires an argument."
                usage
            fi
            CA_CERTIFICATE_FILE="$1"
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
