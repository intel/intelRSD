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

import argparse
from abc import abstractmethod


class Action:
    ACTION, PARAM_NAME = None, None

    def __init__(self, subparsers):
        self.parser = argparse.ArgumentParser(add_help=False)
        self.fill_parser_arguments()
        self.parser.set_defaults(**{self.PARAM_NAME: self.ACTION})
        subparsers.add_parser(self.ACTION,
                              parents=[self.parser],
                              formatter_class=argparse.RawTextHelpFormatter)

    def fill_parser_arguments(self):
        pass

    @abstractmethod
    def process_action(self, configuration):
        pass

    def get_config(self):
        self.parser.parse_args()
