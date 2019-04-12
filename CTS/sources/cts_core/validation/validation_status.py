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


class ValidationStatus:
    FAILED = "Failed"
    TIMEOUT = "Timeout"
    BLOCKED = "Blocked"
    WAITING = "Waiting"
    NOT_RUN = "Not Run"
    SKIPPED = "Skipped"
    UNKNOWN = "Unknown"
    RUNNING = "Running"
    PASSED = "Passed"
    PASSED_WITH_WARNINGS = "Passed with warnings"

    VALIDATION_IMPORTANCE_ORDER = [
        FAILED,
        TIMEOUT,
        BLOCKED,
        WAITING,
        NOT_RUN,
        SKIPPED,
        UNKNOWN,
        RUNNING,
        PASSED_WITH_WARNINGS,
        PASSED
    ]

    @staticmethod
    def join_statuses(*params):
        for status in ValidationStatus.VALIDATION_IMPORTANCE_ORDER:
            if status in params:
                return status

        return ValidationStatus.UNKNOWN
