#!/usr/bin/python
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
from cts_core.commons.error import cts_error


def main():
    try:
        from cts_framework.actions.actions_dispatcher import ActionsDispatcher
        actions_dispatcher = ActionsDispatcher()
        actions_dispatcher.process_application()
    except ImportError as err:
        import traceback
        cts_error("Dependencies are not met: {err:exception}; stacktrace={stack:stacktrace}",
                  err=err, stack=traceback.format_exc())
        exit(1)


if __name__ == '__main__':
    main()
