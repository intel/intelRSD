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

import datetime
import re
from cts_core.commons.text_helpers import to_multiline
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.commons.enums import LoggingLevel
from cts_framework.db.dao.case_dao import CaseDAO
from cts_framework.db.dao.message_dao import MessageDAO
from cts_framework.helpers.python.message_level import MessageLevel
from cts_framework.helpers.python.result_status import ResultStatus
from cts_framework.logging.indent import indent

INDENT_RE = re.compile('(?P<get_indent>\s*)(?P<therest>.*)')

class LoggingMessagesStream():
    TABLE_HEADERS = ("Test Case", "Message Level", "Message Datetime", "Message")

    def __init__(self, run_id, script_execution_id, script):
        """
        :type script_execution_id: int
        :type script: cts_framework.tests_managing.test_script.test_script.TestScript
        """
        self.run_id = run_id
        self.script_execution_id = script_execution_id
        self.script = script
        self.current_message = None
        self.current_message_level = None
        self.current_date_time = None
        self.active_case = None
        self.active_case_execution_id = None
        self.active_case_execution_status = None
        self.overall_status = True

    def set_active_case(self, case_name):
        if self.active_case_execution_id and self.active_case_execution_status in [None, ResultStatus.WAITING,
                                                                                   ResultStatus.RUNNING]:
            self.set_case_status(ResultStatus.UNKNOWN)

        if self.active_case_execution_id and self.active_case_execution_status in [ResultStatus.TIMEOUT,
                                                                                   ResultStatus.FAILED]:
            self.overall_status = False

        self.save_current_message()
        self.active_case = case_name
        self.active_case_execution_id = CaseDAO.register_case_execution(case_name, self.script_execution_id)

    def set_case_status(self, status):
        if self.active_case is None:
            self.set_active_case("Initialization")

        self.active_case_execution_status = status
        CaseDAO.set_case_execution_status(self.active_case_execution_id, status)

    def write(self, msg):
        """
        :type msg: str
        :param msg: message to log
        """

        if self.active_case is None:
            self.set_active_case("Initialization")

        if not msg.split():
            return
        try:
            message_level = {i: i for i in [LoggingLevel.DEBUG, LoggingLevel.ERROR, LoggingLevel.WARNING,
                                            LoggingLevel.MESSAGE, LoggingLevel.CONTROL]}[msg.split("::")[0].upper()]
            message_text = "".join(msg.split("::")[1:])

            if message_text.startswith(" "):
                message_text = message_text[1:]

            self.current_message = message_text
            self.current_message_level = message_level
            self.current_date_time = datetime.datetime.now()
            self.save_current_message()
        except KeyError:
            if self.current_message is None:
                return

            self.current_message += "\n" + msg
        except IndexError:
            if self.current_message is None:
                return

            self.current_message += "\n" + msg

    def save_current_message(self):
        if self.current_message and self.current_message_level and self.current_date_time:
            self.log_db_message(self.active_case_execution_id, self.current_message, self.current_message_level,
                                self.current_date_time)
            if LoggingLevel.CONTROL != self.current_message_level:
                self.log_screen_message(self.current_message, self.current_message_level, self.current_date_time)
            self.current_message = None
            self.current_date_time = None
            self.current_message_level = None

    def log_db_message(self, case_execution_id, message, message_level, message_datetime):
        if case_execution_id:
            MessageDAO.log_message(case_execution_id, message, message_level, message_datetime)

    def log_screen_message(self, message, message_level, message_datetime):
        if MessageLevel.DEBUG != message_level:
            try:
                color = {MessageLevel.ERROR: ColorPrinter.RED_FORE, MessageLevel.WARNING: ColorPrinter.YELLOW_FORE}[message_level]
            except KeyError:
                color = ColorPrinter.DEFAULT

            m = re.match(INDENT_RE, message)
            if m:
                shift = indent() + m.group('get_indent')
                msg = m.group('therest')

                multiline_log = to_multiline(str(msg), 140, '\n{indent}    '.format(indent=shift))

                print "%s%s" % (
                    shift,
                    ColorPrinter.format_text(multiline_log, color=color))

    def enable_stream(self):
        return

    def disable_stream(self, configuration=None):
        self.save_current_message()
        if self.active_case_execution_status in [ResultStatus.RUNNING, ResultStatus.WAITING]:
            self.set_case_status(ResultStatus.UNKNOWN)

        if self.active_case_execution_status in [ResultStatus.FAILED, ResultStatus.TIMEOUT]:
            self.overall_status = False

    def flush(self):
        pass

    def parse_configuration(self, configuration):
        return [[param.split(" ")[0], " ".join(param.split(" ")[1:])] for param in configuration.split("--")]
