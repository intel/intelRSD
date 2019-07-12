#!/usr/bin/env bash

# Copyright (c) 2018-2019 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


if [ -z ${PODM_POSTGRES_USER_PASSWORD} ]; then
    print "PODM_POSTGRES_USER_PASSWORD environment variable is required"
    exit 1
fi

PODM_POSTGRES_USER_NAME="podm"
PODM_COMPONENTS='nodecomposer detector tagger eventservice accessverifier'

function ensure_podm_db_user_exists() {
  echo
  echo "|Ensure - ${PODM_POSTGRES_USER_NAME} - user exists..."
  set +e
  psql --username "$POSTGRES_USER" -tAc "SELECT 1 FROM pg_roles WHERE rolname='${PODM_POSTGRES_USER_NAME}'" | grep -q 1
  if [ "$?" -eq "0" ]; then
    set -e
    echo "User - ${PODM_POSTGRES_USER_NAME} - exists, ensure it has right password..."
    psql --username "$POSTGRES_USER" -c "ALTER USER ${PODM_POSTGRES_USER_NAME} WITH CREATEDB PASSWORD '${PODM_POSTGRES_USER_PASSWORD}'"
  else
    set -e
    echo "User - ${PODM_POSTGRES_USER_NAME} - does not exists, create..."
    psql --username "$POSTGRES_USER" -c "CREATE USER ${PODM_POSTGRES_USER_NAME} WITH CREATEDB PASSWORD '${PODM_POSTGRES_USER_PASSWORD}'"
  fi

  echo "...done|"
}

function terminate() {
  local component=$1
  echo "|Terminate all remaining connections to database - ${component}..."
  psql --username "$POSTGRES_USER" -c "SELECT pg_terminate_backend(pid) FROM pg_stat_activity WHERE datname = '${component}' AND pid <> pg_backend_pid()"
  echo "...done|"
}

function drop_database() {
  local component=$1
  echo "|Drop - ${component} - database..."
  dropdb --username "$POSTGRES_USER" --if-exists ${component}
  echo "...done|"
}

function create_database() {
  local component=$1
  echo "|Create - ${component} - database..."
  createdb --username "$POSTGRES_USER" ${component}
  echo "...done|"
}

function grant_privileges_on_database() {
  local component=$1
  echo "|Grant PRIVILEGES on - ${component} - database to ${PODM_POSTGRES_USER_NAME}"
  psql --username "$POSTGRES_USER" -c "GRANT ALL PRIVILEGES ON DATABASE ${component} to ${PODM_POSTGRES_USER_NAME}"
  echo "...done|"
}

function recreate_database() {
  local component=$1
  echo "-------------------------------------------------------"
  echo "-> RECREATE - ${component} - db"
  echo "-------------------------------------------------------"
  terminate ${component}
  drop_database ${component}
  create_database ${component}
  grant_privileges_on_database ${component}
  echo "-> DONE recreate - ${component} - db"
  echo
}

function recreate_all_databases_and_user() {
  ensure_podm_db_user_exists
  for component in ${PODM_COMPONENTS}
  do
    recreate_database ${component}
  done
}

function recreate_user_and_grant_privileges() {
  ensure_podm_db_user_exists
  for component in ${PODM_COMPONENTS}
  do
    grant_privileges_on_database ${component}
  done
}

