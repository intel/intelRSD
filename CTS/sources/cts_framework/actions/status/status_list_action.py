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
from cts_core.commons.text_helpers import to_multiline
from cts_framework.actions.action import Action
from cts_framework.db.dao.run_dao import RunDAO
from cts_framework.db.dao.script_dao import ScriptDAO


class StatusListAction(Action):
    ACTION = "list"
    PARAM_NAME = "STATUS_TYPE"

    def process_action(self, configuration):
        test_runs = RunDAO.list_all_runs()
        print "Run ID\tDate\n"

        for test_run in test_runs:
            print "%s\n[%s]\t%s\n\n\t%s\n" % ("-"*89, test_run.id, test_run.run_datetime,
                                              to_multiline(test_run.cmd, 80, '\\\n\t'))
            for script_execution_id in ScriptDAO.get_scripts_execution_ids_for_run(test_run.id):
                script_execution = ScriptDAO.get_script_execution_details(script_execution_id)
                cmd = "\t[id=%d]\n\t%s execute %s" % (script_execution_id,
                                                      script_execution.script_path,
                                                      script_execution.configuration)
                print to_multiline(cmd, 80, ' \\\n\t')

        print '\n\n'
