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

class ResultStatus:
    FAILED = "Failed"
    TIMEOUT = "Timeout"
    BLOCKED = "Blocked"
    UNKNOWN = "Unknown"
    PASSED_WITH_WARNING = "Passed with warning"
    SKIPPED = "Skipped"
    PASSED = "Passed"

    NOT_RUN = "Not Run"
    WAITING = "Waiting"
    RUNNING = "Running"

    @staticmethod
    def join_statuses(status_1, status_2):
        for status in [ResultStatus.FAILED, ResultStatus.TIMEOUT, ResultStatus.BLOCKED, ResultStatus.UNKNOWN,
                       ResultStatus.PASSED_WITH_WARNING, ResultStatus.SKIPPED, ResultStatus.PASSED]:
            if status in [status_1, status_2]:
                return status

        return ResultStatus.UNKNOWN
