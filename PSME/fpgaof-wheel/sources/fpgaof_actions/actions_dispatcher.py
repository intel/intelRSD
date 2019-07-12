"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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

import argparse
from fpgaof_actions.show_uuid_action import ShowUUIDAction
from fpgaof_actions.show_version_action import ShowVersionAction
from fpgaof_actions.execute_action import ExecuteAction


class ActionsDispatcher:
    ACTIONS_HANDLERS = [ExecuteAction, ShowUUIDAction, ShowVersionAction]

    def __init__(self):
        self.parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
        subparsers = self.parser.add_subparsers()

        self.action_handlers = {action_handler.ACTION: action_handler(subparsers)
                                for action_handler in self.ACTIONS_HANDLERS}

    def process_application(self):
        configuration = self.parser.parse_args()
        self.action_handlers[configuration.ACTION].process_action(configuration)
