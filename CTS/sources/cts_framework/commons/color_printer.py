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

from colorama import init, Fore, Back, Style

from cts_framework.helpers.python.result_status import ResultStatus


class ColorPrinter:
    RED_FORE = Fore.RED
    YELLOW_FORE = Fore.YELLOW
    GREEN_FORE = Fore.GREEN
    BLACK_FORE = Fore.BLACK
    WHITE_FORE = Fore.WHITE
    DEFAULT = Style.RESET_ALL
    WHITE_BACK = Back.WHITE
    BLUE_ON_WHITE = Fore.LIGHTBLUE_EX + Back.WHITE
    BLUE_FORE = Fore.BLUE

    @staticmethod
    def init():
        init()

    @staticmethod
    def format_text(text, color=None, bold=False):
        if color is None:
            color = ""
        return color + (Style.BRIGHT if bold else "") + text + Style.RESET_ALL

    @staticmethod
    def format_status(status, bold=False):
        try:
            color = {ResultStatus.PASSED: ColorPrinter.GREEN_FORE, ResultStatus.FAILED: ColorPrinter.RED_FORE,
                     ResultStatus.BLOCKED: ColorPrinter.YELLOW_FORE, ResultStatus.TIMEOUT: ColorPrinter.RED_FORE,
                     ResultStatus.UNKNOWN: ColorPrinter.YELLOW_FORE}[status]
        except KeyError:
            color = ColorPrinter.DEFAULT
        return ColorPrinter.format_text(status, color=color, bold=bold)
