"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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
from cts_framework.db.model.message import MessageModel


class MessageDAO(BaseDAO):
    @staticmethod
    @BaseDAO.database_operation
    def log_message(case_execution_id, message, message_level, message_datetime, database_session):
        """
        :type case_execution_id: int
        :type message: str
        :type message_level: str
        :type message_datetime: str
        :rtype: int
        """
        test_message = MessageModel()
        test_message.case_execution_id = case_execution_id
        test_message.message_text = message
        test_message.message_level = message_level
        test_message.message_datetime = message_datetime
        database_session.add(test_message)
        database_session.flush()
        database_session.refresh(test_message)
        return test_message.id

    @staticmethod
    @BaseDAO.database_operation
    def get_messages_for_case_execution(case_execution_id, database_session):
        """
        :type case_execution_id: int
        :rtype: list[MessageModel]
        """
        for message in database_session.query(MessageModel).filter(MessageModel.case_execution_id == case_execution_id):
            yield message
