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
from cts_framework.db.dao.test_case_results_dao import TestCaseResultsDAO
from cts_framework.db.model.test_run_model import TestRunModel


class TestRunDAO(BaseDAO):
    def __init__(self):
        pass

    @BaseDAO.database_operation
    def create_test_run(self, test_run_comment, database_session):
        test_run = TestRunModel()
        test_run.test_run_comment = test_run_comment
        database_session.add(test_run)
        database_session.commit()
        database_session.refresh(test_run)
        return test_run.test_run_id

    @BaseDAO.database_operation
    def list_all_test_runs(self, database_session):
        return database_session.query(TestRunModel).all()

    @BaseDAO.database_operation
    def get_test_run_details(self, test_run_id, database_session):
        return database_session.query(TestRunModel).filter(TestRunModel.test_run_id == test_run_id).first()

    @BaseDAO.database_operation
    def delete_test_run_results(self, test_run_id, database_session):
        test_run = self.get_test_run_details(test_run_id)
        database_session.delete(test_run)
        TestCaseResultsDAO().delete_test_cases_results_for_test_run(test_run_id)
        database_session.commit()
