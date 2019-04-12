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

from os import getpid
from sqlalchemy import or_, func
from cts_framework.db.dao.base_dao import BaseDAO
from cts_framework.db.model.case_execution import CaseExecutionModel
from cts_framework.db.model.script_execution_model import ScriptExecutionModel
from cts_framework.helpers.python.result_status import ResultStatus


class ScriptDAO(BaseDAO):
    @staticmethod
    @BaseDAO.database_operation
    def register_script_execution(script_path, run_id, configuration, control_sum, database_session):
        """
        :type script_path: str
        :type run_id: int
        :type configuration: str
        :type control_sum: str
        :rtype: int
        """
        script = ScriptExecutionModel()
        script.script_path = script_path
        script.run_id = run_id
        script.configuration = configuration
        script.control_sum = control_sum
        script.pid = getpid()
        database_session.add(script)
        database_session.flush()
        database_session.refresh(script)
        return script.id

    @staticmethod
    @BaseDAO.database_operation
    def get_scripts_execution_ids_for_run(run_id, database_session):
        """
        :type run_id: int
        :rtype: list[int]
        """
        return (script_execution.id for script_execution in
                database_session.query(ScriptExecutionModel.id).filter(ScriptExecutionModel.run_id == run_id).all())

    @staticmethod
    @BaseDAO.database_operation
    def get_script_execution_details(test_script_execution_id, database_session):
        """
        :type test_script_execution_id: int
        :rtype: ScriptExecutionModel
        """
        return database_session.query(ScriptExecutionModel).filter(
            ScriptExecutionModel.id == test_script_execution_id).first()

    @staticmethod
    @BaseDAO.database_operation
    def get_script_execution_statistics(test_script_execution_id, database_session):
        """
        :type test_script_execution_id: int
        :rtype: bool, int, int
        :return: overall_status, total passed, total
        """
        passed = database_session.query(CaseExecutionModel). \
            filter(CaseExecutionModel.script_execution_id == test_script_execution_id). \
            filter(CaseExecutionModel.status == ResultStatus.PASSED).count()

        failed = database_session.query(CaseExecutionModel). \
            filter(CaseExecutionModel.script_execution_id == test_script_execution_id). \
            filter(or_(CaseExecutionModel.status == ResultStatus.FAILED,
                       CaseExecutionModel.status == ResultStatus.TIMEOUT,
                       CaseExecutionModel.status == ResultStatus.BLOCKED)).count()
        total = database_session.query(CaseExecutionModel).\
            filter(CaseExecutionModel.script_execution_id == test_script_execution_id).count()

        return not bool(failed), passed, total

    @staticmethod
    @BaseDAO.database_operation
    def get_last_script_execution_id(database_session):
        max = database_session.query(func.max(ScriptExecutionModel.id).label('last_id')).one()
        return max.last_id
