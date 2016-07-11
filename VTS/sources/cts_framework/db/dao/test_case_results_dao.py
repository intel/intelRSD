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
from cts_framework.db.dao.test_result_message_dao import TestResultMessageDAO
from cts_framework.db.model.test_case_result_model import TestCaseResultModel


class TestCaseResultsDAO(BaseDAO):
    def __init__(self):
        pass

    @BaseDAO.database_operation
    def register_test_case_execution(self, test_run_id, status, test_case_name, database_session):
        test_case_result = TestCaseResultModel()
        test_case_result.test_case_status = status
        test_case_result.test_case_test_case_name = test_case_name

        test_case_result.test_case_test_run_id = test_run_id

        database_session.add(test_case_result)
        database_session.commit()
        database_session.refresh(test_case_result)

        return test_case_result.test_case_id

    @BaseDAO.database_operation
    def set_test_execution_status(self, test_case_result_id, status, database_session):
        test_case = database_session.query(TestCaseResultModel).\
            filter(TestCaseResultModel.test_case_id == test_case_result_id).first()
        test_case.test_case_status = status

        database_session.commit()

    @BaseDAO.database_operation
    def get_test_case_status(self, test_case_result_id, database_session):
        return database_session.query(TestCaseResultModel.test_case_id).\
            filter(TestCaseResultModel.test_case_id == test_case_result_id).first().test_case_id

    @BaseDAO.database_operation
    def get_test_case_result(self, test_case_result_id, database_session):
        """
        :type test_case_result_id: int
        :rtype: TestCaseResultModel
        """
        return database_session.query(TestCaseResultModel).\
            filter(TestCaseResultModel.test_case_id == test_case_result_id).first()

    @BaseDAO.database_operation
    def get_test_cases_results_for_test_run(self, test_run_id, database_session):
        """
        :rtype: list[TestCaseResultModel]
        """
        return database_session.query(TestCaseResultModel).\
            filter(TestCaseResultModel.test_case_test_run_id == test_run_id).all()

    @BaseDAO.database_operation
    def delete_test_cases_results_for_test_run(self, test_run_id, database_session):
        test_cases_results = self.get_test_cases_results_for_test_run(test_run_id)
        for test_case_result in test_cases_results:
            self.delete_test_case_result(test_case_result.test_case_id)

    @BaseDAO.database_operation
    def delete_test_case_result(self, test_case_result_id, database_session):
        TestResultMessageDAO().delete_test_result_messages_for_test_case_result(test_case_result_id)
        test_case_result = self.get_test_case_result(test_case_result_id)
        database_session.delete(test_case_result)
        database_session.commit()
