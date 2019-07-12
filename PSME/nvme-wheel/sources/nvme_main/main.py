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
from nvme_framework.helpers.precheck import Precheck
from nvme_framework.configuration.config_manager import Config as Cfg


def main():
    Precheck()      # verify prerequisites
    Cfg()           # check or create config file

    try:
        from nvme_framework.actions.actions_dispatcher import ActionsDispatcher
        action_dispatcher = ActionsDispatcher()
        action_dispatcher.process_application()
    except ImportError as err:
        print(err)
        exit(1)


if __name__ == '__main__':
    main()
