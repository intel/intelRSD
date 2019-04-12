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

import logging


class LoggingHelper:
    def __init__(self, name=None):
        """
        :type name: str
        """
        if name is None:
            name = "Anonymous logger"

        logging.basicConfig(filename="/var/log/cts/cts.log", level=logging.DEBUG,
                            format="%(asctime)s [%(levelname)s] %(name)s: %(message)s")
        self._logger = logging.getLogger(name)

    def log_debug(self, message):
        """
        :type message: str
        """
        self._logger.debug(message)

    def log_info(self, message):
        """
        :type message: str
        """
        self._logger.info(message)

    def log_error(self, message):
        """
        :type message: str
        """
        self._logger.error(message)

    def log_critical(self, message):
        """
        :type message: str
        """
        self._logger.critical(message)
