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
from cts_framework.db.model.case_execution import CaseExecutionModel
from cts_framework.helpers.python.result_status import ResultStatus


class CaseDAO(BaseDAO):
    @staticmethod
    @BaseDAO.database_operation
    def register_case_execution(case_name, script_execution_id, database_session):
        """
        :type case_name: str
        :type script_execution_id: int
        :rtype: int
        """
        case = CaseExecutionModel()
        case.name = case_name
        case.script_execution_id = script_execution_id
        case.status = ResultStatus.UNKNOWN
        database_session.add(case)
        database_session.flush()
        database_session.refresh(case)
        return case.id

    @staticmethod
    @BaseDAO.database_operation
    def set_case_execution_status(case_execution_id, status, database_session):
        """
        :type case_execution_id: int
        :type status: str
        """
        test_case_execution = database_session.query(CaseExecutionModel).filter(
            CaseExecutionModel.id == case_execution_id).first()

        test_case_execution.status = status
        database_session.flush()
        database_session.refresh(test_case_execution)

    @staticmethod
    @BaseDAO.database_operation
    def get_cases_execution_ids_for_script_execution(script_execution_id, database_session):
        """
        :type script_execution_id: int
        :rtype: list[int]
        """
        return [case_execution.id for case_execution in
                database_session.query(CaseExecutionModel).filter(CaseExecutionModel.script_execution_id == script_execution_id).
                all()]

    @staticmethod
    @BaseDAO.database_operation
    def get_case_execution_details(case_execution_id, database_session):
        """
        :type case_execution_id: int
        :rtype: CaseExecutionModel
        """
        return database_session.query(CaseExecutionModel).filter(
            CaseExecutionModel.id == case_execution_id).first()
