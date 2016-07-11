#!/usr/bin/python
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

def main():
    import sys
    sys.path.append("/opt/cts_core/lib/")

    try:
        from cts_framework.actions.actions_dispatcher import ActionsDispatcher
        actions_dispatcher = ActionsDispatcher()
        actions_dispatcher.process_aplication()
    except ImportError as err:
        import traceback
        print traceback.format_exc()
        print "Dependencies are not met: %s" % err.message
        exit(1)



if __name__ == '__main__':
    main()
