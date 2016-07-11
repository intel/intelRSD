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

import os
import sys

import datetime

from cts_framework.commons.enums import LoggingLevel
from cts_framework.db.dao.test_result_message_dao import TestResultMessageDAO


class LoggingStream:
    TABLE_HEADERS = ("Message Level", "Message Datetime", "Message")

    def __init__(self, test_case_result_id=None, html_file=None, csv_file=None, tag=None):
        """
        :type tag: str
        :type csv_file: str
        :type html_file: str
        :type test_case_result_id: int
        :param test_case_result_id: test_case_result_id to log under it messages (None if not log)
        :param html_file: file to log messages in html format (None if not log)
        :param csv_file: file to log messages in html format (None if not log)
        :param tag: additional log tag (None if not used)
        """
        if tag is None:
            tag = ""
        else:
            tag = "::" + tag
        self.tag = tag
        self.html_file = html_file
        self.csv_file = csv_file
        self.test_case_result_id = test_case_result_id
        self.html_file_handle = None
        self.csv_file_handle = None
        self.system_stdout = None
        self.current_message = None
        self.current_date_time = None
        self.current_message_level = None

        self.test_result_message_dao = TestResultMessageDAO()

    def write(self, msg):
        """
        :type msg: str
        :param msg: message to log
        """
        if not msg.split():
            return
        try:
            message_level = {i: i for i in [LoggingLevel.DEBUG, LoggingLevel.ERROR, LoggingLevel.WARNING,
                                            LoggingLevel.MESSAGE]}[msg.split("::")[0].upper()]
            message_text = "".join(msg.split("::")[1:])
            self.save_current_message()
            self.current_message = message_text
            self.current_message_level = message_level
            self.current_date_time = datetime.datetime.now()
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
            self.log_csv_message(self.current_message_level, self.current_date_time, self.current_message)
            self.log_html_message(self.current_message_level, self.current_date_time, self.current_message)
            self.log_db_message(self.current_message_level, self.current_date_time, self.current_message)
            self.log_screen_message(self.current_message_level, self.current_date_time, self.current_message)

            self.current_message = None
            self.current_date_time = None
            self.current_message_level = None

    def log_html_message(self, message_level, message_datetime, message):
        """
        :type message_level: str
        :type message_datetime: str
        :type message: str
        :param message_level: message level
        :param message_datetime: message datetime
        :param message: message text
        """
        if self.html_file_handle:
            self.html_file_handle.write("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n" %
                                        (message_datetime, message_level, message.replace("\n", "<br>")))
            self.html_file_handle.flush()

    def log_csv_message(self, message_level, message_datetime, message):
        """
        :type message_level: str
        :type message_datetime: str
        :type message: str
        :param message_level: message level
        :param message_datetime: message datetime
        :param message: message text
        """
        if self.csv_file_handle:
            self.csv_file_handle.write("%s\t%s\t%s\n" % (message_datetime, message_level, message))
            self.csv_file_handle.flush()

    def log_db_message(self, message_level, message_datetime, message):
        """
        :type message_level: str
        :type message_datetime: str
        :type message: str
        :param message_level: message level
        :param message_datetime: message datetime
        :param message: message text
        """
        if self.test_case_result_id:
            self.test_result_message_dao.add_message(message, message_level, message_datetime, self.test_case_result_id)

    def log_screen_message(self, message_level, message_datetime, message):
        """
        :type message_level: str
        :type message_datetime: str
        :type message: str
        :param message_level: message level
        :param message_datetime: message datetime
        :param message: message text
        """
        print >> sys.stderr, "log tag%s [%s] [%s]\n%s\n%s\n%s\n" % (self.tag, message_datetime, message_level, "=" * 25, message,
                                                     "=" * 25)

    def enable_stream(self):
        if not self.system_stdout == self:
            self.create_html_file()
            self.create_csv_file()
            self.system_stdout = sys.stdout
            sys.stdout = self
        else:
            raise RuntimeError("Logging stream already active.")

    def create_html_file(self):
        if self.html_file:
            path = "/".join(self.html_file.split("/")[:-1])
            self._create_path(path)

            self.html_file_handle = open(self.html_file, "w")

            self.html_file_handle.write("<table>")
            self.log_html_message(*self.TABLE_HEADERS)

    def create_csv_file(self):
        if self.csv_file:
            path = "/".join(self.csv_file.split("/")[:-1])
            self._create_path(path)

            self.csv_file_handle = open(self.csv_file, "w")
            self.log_csv_message(*self.TABLE_HEADERS)

    def _create_path(self, path):
        """
        :type path: str
        :param path: path to be created
        """
        try:
            os.makedirs(path)
        except OSError as err:
            if err.errno != os.errno.EEXIST or not os.path.isdir(path):
                raise

    def disable_stream(self):
        if sys.stdout == self:
            self.save_current_message()
            if self.html_file_handle:
                self.html_file_handle.write("</table>")
                self.html_file_handle.close()

            if self.csv_file_handle:
                self.csv_file_handle.write("\n")
                self.csv_file_handle.close()

            sys.stdout = self.system_stdout
            self.system_stdout = None
        else:
            raise RuntimeError("Unable to disable stream that is not active")

    def flush(self):
        pass
