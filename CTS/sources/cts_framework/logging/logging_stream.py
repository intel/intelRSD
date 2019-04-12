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
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.helpers.python.result_status import ResultStatus
from cts_framework.logging.logging_messages_stream import LoggingMessagesStream
from cts_framework.logging.indent import Indent, indent


class LoggingStream():
    TABLE_HEADERS = ("Message Level", "Message Datetime", "Message")
    SEPARATOR = "::"
    LOG_PREFIX = "LOG"
    TEST_CASE_TITLE = "TEST_CASE"
    STATUS = "STATUS"
    SCREEN = "SCREEN::"

    def __init__(self, test_run_id, test_script_execution_id, test_script):
        """
        :type test_script_execution_id: int
        :type test_script: cts_framework.tests_managing.test_script.test_script.TestScript
        """
        self.logging_messages_stream = LoggingMessagesStream(test_run_id, test_script_execution_id, test_script)

    def write(self, msg):
        """
        :type msg: str
        :param msg: message to log
        """
        if msg.startswith("%s%s" % (self.LOG_PREFIX, self.SEPARATOR)):
            print >> self.logging_messages_stream, "".join(msg.split(self.SEPARATOR)[1:])
        elif msg.startswith("%s%s" % (self.TEST_CASE_TITLE, self.SEPARATOR)):
            Indent.indent_reset()
            test_case = "".join(msg.split(self.SEPARATOR)[1:]).strip()
            self.logging_messages_stream.set_active_case(test_case)

            msg = '\nTest case: {test_case}'.format(test_case=test_case)
            print "%s%s" % (
                indent(),
                ColorPrinter.format_text(msg, color=ColorPrinter.BLUE_FORE, bold=True))
            Indent.indent_right()
        elif msg.startswith("%s%s" % (self.STATUS, self.SEPARATOR)):
            status = "".join(msg.split(self.SEPARATOR)[1:]).strip()
            self.logging_messages_stream.set_case_status(status)
            print "%s%s" % (
                indent(),
                ColorPrinter.format_status(status, bold=True)
            )
            Indent.indent_reset()
        elif msg.startswith(self.SCREEN):
            print "%s%s" % (
                indent(),
                "".join(msg.split(self.SCREEN)[1:]).strip()
            )
        else:
            print >> self.logging_messages_stream, msg

    def enable_stream(self):
        self.logging_messages_stream.enable_stream()

    def disable_stream(self, configuration=None):
        self.logging_messages_stream.disable_stream(configuration=configuration)

    def raise_timeout(self):
        self.logging_messages_stream.set_case_status(ResultStatus.TIMEOUT)

    def raise_failed(self):
        self.logging_messages_stream.set_case_status(ResultStatus.FAILED)

    def get_overall_execution_status(self):
        return self.logging_messages_stream.overall_status
