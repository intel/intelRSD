"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

from cts_framework.db.dao.base_dao import BaseDAO
from cts_framework.db.model.test_result_message_model import TestResultMessageModel


class TestResultMessageDAO(BaseDAO):
    def __init__(self):
        pass

    @BaseDAO.database_operation
    def add_message(self, message, message_level, message_datetime, test_case_result_id, database_session):
        new_message = TestResultMessageModel()
        new_message.test_result_message_message = message
        new_message.test_result_message_status = message_level
        new_message.test_result_message_datetime = message_datetime
        new_message.test_result_message_test_case_id = test_case_result_id

        database_session.add(new_message)
        database_session.commit()

    @BaseDAO.database_operation
    def get_messages_for_test_case_result(self, test_case_result_id, database_session):
        return database_session.query(TestResultMessageModel).\
            filter(TestResultMessageModel.test_result_message_test_case_id == test_case_result_id).all()

    @BaseDAO.database_operation
    def delete_test_result_messages_for_test_case_result(self, test_case_result_id, database_session):
        test_result_messages = self.get_messages_for_test_case_result(test_case_result_id)
        for test_result_message in test_result_messages:
            database_session.delete(test_result_message)

        database_session.commit()
