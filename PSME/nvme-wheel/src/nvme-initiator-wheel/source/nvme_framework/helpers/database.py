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

import sqlite3
from time import time
from nvme_framework.helpers.constants import Constants
from nvme_framework.helpers.discover_local_nvme_nodes import DiscoverLocal
from nvme_framework.helpers.message import Message


def update_timestamp(func):
    def updated_timestamp(*args, **kwargs):
        make_query(Constants.UPDATE_TIME, 'last-operation', int(time()))
        return func(*args, **kwargs)
    return updated_timestamp


def update_local_path(func):
    """
    Use this decorator only for connect functions, when you need verify the drive was successfully mounted
    """
    def updated_local_path(*args, **kwargs):
        dl = DiscoverLocal()
        func_return = func(*args, **kwargs)

        # if a new target was connected, update database entry with nvme path
        try:
            update_node_id(args[1], dl.get_recently_added_nvme_node())
        except:
            Message.error('Target name was not specify')
        return func_return
    return updated_local_path


def db_connect():
    return sqlite3.connect(Constants.DATABASE_FILE_PATH)


def make_query(cmd, *argv):
    conn = db_connect()
    try:
        with conn:
            elements = conn.execute(cmd, argv).fetchall()
    except sqlite3.IntegrityError:
        raise sqlite3.IntegrityError
    conn.close()
    return elements


def run_count_query(name):
    elements = make_query(Constants.COUNT_QUERY)
    if not name:
        return 0
    return len(elements)


def run_check_if_exist_query(name):
    elements = make_query(Constants.COUNT_QUERY)

    element_exist = False
    for element in elements:
        if name in element:
            element_exist = True
            break
    return element_exist


def create_database():
    for cmd in [Constants.DATABASE_CREATE,
                Constants.TIME_DB_CREATE,
                Constants.TIME_UNIQUE]:
        make_query(cmd)


def get_all_rows():
    elements = make_query(Constants.SELECT_ALL_FROM_NVME)
    return create_dict(elements)


def create_dict(db_result):
    rows_list = []
    db_schema = ["address", "port", "name", "status", "nvme_id"]
    for row in db_result:
        rows_dict = {}
        for e in range(len(row)):
            rows_dict[db_schema[e]] = row[e]
        rows_list.append(rows_dict)
    return rows_list


def insert_element(address, port, name, status):
    make_query(Constants.INSERT_INTO_DATABASE, address, port, name, status)


def update_element(name, status):
    make_query(Constants.UPDATE_STATUS, status, name)


def remove_element(name):
    make_query(Constants.REMOVE_ELEMENT, name)


def get_path(name):
    return make_query(Constants.SELECT_NODE_ID, name)[0][0]


def update_node_id(name, new_path):
    make_query(Constants.UPDATE_NODE_ID, new_path, name)


def get_last_timestamp():
    return make_query(Constants.SELECT_TIME)[0][0]


def remove_entire_table():
    make_query(Constants.DROP_TABLE)


if __name__ == "__main__":
    create_database()
