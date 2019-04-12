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
from time import time

COLOR_RED = '\033[91m'
COLOR_ORANGE = '\033[93m'
COLOR_END = '\033[0m'


class Message:
    @staticmethod
    def error(message):
        print('ERRO::{timestamp}:: {message}'.
              format(timestamp=int(time()),
                     message=message))

    @staticmethod
    def error_color(message):
        print('{color_start}ERRO::{timestamp}:: {message}{color_end}'.
              format(timestamp=int(time()),
                     message=message,
                     color_start=COLOR_RED, color_end=COLOR_END))

    @staticmethod
    def warning(message):
        print('WARN::{timestamp}:: {message}'.
              format(timestamp=int(time()),
                     message=message))

    @staticmethod
    def warning_color(message):
        print('{color_start}WARN::{timestamp}:: {message}{color_end}'.
              format(timestamp=int(time()),
                     message=message,
                     color_start=COLOR_ORANGE, color_end=COLOR_END))

    @staticmethod
    def info(message):
        print('INFO::{timestamp}:: {message}'.
              format(timestamp=int(time()),
                     message=message))

    @staticmethod
    def debug(message):
        print('DEBUG::{timestamp}:: {message}'.
              format(timestamp=int(time()),
                     message=message))

    def found_message(self, normal_text, highlighted_text):
        print('INFO::{timestamp}:: {message}'.
              format(timestamp=int(time()),
                     message=self.found_message_builder(normal_text, highlighted_text, COLOR_ORANGE)))

    def not_found_message(self, normal_text, highlighted_text):
        print('WARN::{timestamp}:: {message}'.
              format(timestamp=int(time()),
                     message=self.found_message_builder(normal_text, highlighted_text, COLOR_RED)))

    @staticmethod
    def found_message_builder(normal_text, highlighted_text, highlight_color):
        return '{normal_text} \t{color_start}{highlighted_text}{color_end}'. \
            format(normal_text=normal_text,
                   color_start=highlight_color,
                   highlighted_text=highlighted_text,
                   color_end=COLOR_END)
