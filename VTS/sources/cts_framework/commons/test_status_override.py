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

from cts_framework.tests_helpers.result_status import ResultStatus

class TestStatusOverride:

    @classmethod
    def reset(cls):
        cls.status = ResultStatus.PASSED

    @classmethod
    def set_status(cls, status):
        if hasattr(cls, 'status'):
            cls.status = ResultStatus.join_statuses(status, cls.status)
        else:
            cls.status = status


    @classmethod
    def get_joined_status(cls, status):
        if hasattr(cls, 'status'):
            return ResultStatus.join_statuses(status, cls.status)

        return status
