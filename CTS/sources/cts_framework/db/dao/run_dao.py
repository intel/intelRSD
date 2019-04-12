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

import sys
from cts_framework.db.dao.base_dao import BaseDAO
from cts_framework.db.dao.script_dao import ScriptDAO
from cts_framework.db.model.run_model import RunModel


class RunDAO(BaseDAO):
    @staticmethod
    @BaseDAO.database_operation
    def register_run(tag, cmd, database_session):
        run = RunModel()
        run.tag = tag
        run.cmd = cmd
        database_session.add(run)
        database_session.flush()
        database_session.refresh(run)
        return run.id

    @staticmethod
    @BaseDAO.database_operation
    def list_all_runs(database_session):
        """
        :rtype: list[RunModel]
        """
        return database_session.query(RunModel).all()

    @staticmethod
    @BaseDAO.database_operation
    def get_run_details(run_id, database_session):
        """
        :type run_id: int
        :rtype: RunModel
        """
        return database_session.query(RunModel).filter(RunModel.id == run_id).first()

    @staticmethod
    @BaseDAO.database_operation
    def delete_run_results(run_id, database_session):
        run = RunDAO.get_run_details(run_id)
        if run is not None:
            database_session.delete(run)
            database_session.flush()
        else:
            sys.stderr.write("Run no %s does not exist\n\n" % run_id)

    @staticmethod
    @BaseDAO.database_operation
    def get_overall_run_result(run_id, database_session):
        """
        :type run_id: int
        :rtype: bool
        """
        overall_status = True
        total = 0
        passed = 0
        for script_id in ScriptDAO.get_scripts_execution_ids_for_run(run_id):
            total += 1
            status, _, _ = ScriptDAO.get_script_execution_statistics(script_id)
            overall_status = overall_status and status
            if status:
                passed += 1

        return overall_status, passed, total
