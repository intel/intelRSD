"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2018 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

import os.path
from os.path import expanduser
from datetime import datetime


class Constants:
    HOME_DIR = expanduser('~')
    NVME_PATH_DELETE_CONTROLLER = '/sys/class/nvme/{node_id}/delete_controller'

    STATUS_CONNECTED = 'connected'
    STATUS_DISCOVERED = 'discovered'
    STATUS_LOST = 'lost'

    CONFIG_AVAILABLE_CONTROLLERS = 'available_controllers'
    CONFIG_CRON_IN_SECONDS = 'cron_timer_in_seconds'
    CONFIG_TARGET_NAME = 'name'
    CONFIG_TARGET_ADDRESS = 'address'
    CONFIG_TARGET_PORT = 'port'

    CONFIG_FILE_PATH = os.path.join(*(HOME_DIR, "nvme-wheel.config"))
    CONFIGURATION_FILE_PATH = ''.join(('/', os.path.join('etc', 'psme', 'nvme-wheel-configuration.json')))
    DATABASE_FILE_PATH = os.path.join(*(HOME_DIR, "nvme-wheel.db"))
    LOG_FILE_PATH = "".join(('/tmp', '/nvme-wheel-', str(datetime.now()), '.log'))

    NVME_PARAM_TRADDR = 'traddr'
    NVME_PARAM_TRANSPORT = 'transport'
    NVME_PARAM_HOSTNQN = 'hostnqn'
    NVME_PARAM_TRSVCID = 'trsvcid'

    HORIZONTAL_SEPARATOR = str("*"*40)
    VERTICAL_SEPARATOR = str("\n"*4)

    VERSION_TEXT_NVME_WHEEL = 'NVME Wheel'

    # Actions on NVMe cli
    NVME_DISCOVER = 'nvme discover'
    NVME_LIST = 'nvme list'
    NVME_CONNECT_ALL = 'nvme connect-all'
    NVME_CONNECT = 'nvme connect'
    NVME_DISCONNECT = 'nvme disconnect'

    NVME_VERSION = 'nvme version'

    # Actions on operating system
    LSMOD_GREP_NVME = "lsmod | grep nvme"

    # Params for commands on NVMe cli
    NVME_JSON_OUTPUT_PARAM = '-o json'
    NVME_T_RMA_PARAM = '-t rdma'
    NVME_RDMA = 'rdma'

    UUID_NQN_PREFIX = 'nqn.2014-08.org.nvmexpress:uuid:'

    # Database query
    COUNT_QUERY = "SELECT name FROM nvme GROUP BY name"
    DATABASE_CREATE = "CREATE TABLE IF NOT EXISTS nvme (address TEXT, port TEXT, name TEXT, status TEXT, node_id TEXT NOT NULL DEFAULT '')"
    DROP_TABLE = "DROP TABLE nvme"
    INSERT_INTO_DATABASE = "INSERT INTO nvme(address, port, name, status) VALUES (?, ?, ?, ?)"
    REMOVE_ELEMENT = "DELETE FROM nvme WHERE name = ?"
    SELECT_ALL_FROM_NVME = "SELECT * FROM nvme"
    SELECT_NODE_ID = "SELECT node_id FROM nvme WHERE name = ?"
    UPDATE_NODE_ID = "UPDATE nvme SET node_id = ? WHERE name = ?"
    UPDATE_STATUS = "UPDATE nvme SET status = ? WHERE name = ?"

    # Timestamp database
    TIME_DB_CREATE = "CREATE TABLE IF NOT EXISTS time (id integer PRIMARY KEY, name TEXT, stamp INTEGER)"
    TIME_UNIQUE = "CREATE UNIQUE INDEX IF NOT EXISTS idx_name ON time (name)"
    UPDATE_TIME = "REPLACE INTO time (name, stamp) VALUES (?, ?)"
    SELECT_TIME = "SELECT stamp FROM time"

    # NVME ISSUE
    NVME_DISCOVERY_GENERIC_ERROR = 'Failed to write to /dev/nvme-fabrics: Connection reset by peer'
