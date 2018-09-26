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

from os import getenv

from nvme_framework.helpers.database import *
from nvme_framework.helpers.message import Message


class Config:
    CONFIGURATION = dict(
        available_controllers=[],
    )

    def __init__(self):
        self.database_path = Constants.DATABASE_FILE_PATH
        self.configuration_from_file = None
        self.create_database_file()

    @staticmethod
    def create_database_file():
        # create database if not exist
        create_database()

    @staticmethod
    def save_available_target(tgt_address, tgt_port, tgt_name, tgt_status=None):
        # check that this element exist
        if run_check_if_exist_query(tgt_name):
                return False

        insert_element(tgt_address, tgt_port, tgt_name, tgt_status)
        Message.info('Found a new element %s -- %s:%s' % (tgt_name, tgt_address, tgt_port))
        return True

    @staticmethod
    def get_cron_time():
        return getenv('NVME_CRON', 5)

    @staticmethod
    def get_available_targets():
        return get_all_rows()

    @staticmethod
    def update_status(tgt_name, tgt_status):
        update_element(tgt_name, tgt_status)
        return True

    @staticmethod
    def remove_element_by_name(tgt_name=None):
        len_before = run_count_query(tgt_name)

        remove_element(tgt_name)
        len_after = run_count_query(tgt_name)

        if len_before == len_after:
            Message.info('Target was found')
            return False
        return True

    @staticmethod
    def remove_all_targets():
        remove_entire_table()
