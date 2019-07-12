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

import ConfigParser
import argparse

import sys
from datetime import datetime

from cts_core.commons.error import ErrorMute
from cts_core.commons.services import ServiceTypes
from cts_core.commons.text_helpers import to_multiline
from cts_core.metadata.comparator import Comparator
from cts_core.metadata.diff.metadata_comparator import MetadataComparator
from cts_framework.actions.action import Action
from cts_framework.build_information import BuildInformation
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.commons.logging_helper import LoggingHelper
from cts_framework.tests_managing.tests_manager import TestsManager

class MetadataDiffAction(Action):
    def __init__(self, *params, **kwargs):
        self._logger = LoggingHelper(__name__)
        self.tests_manager = TestsManager()

        Action.__init__(self, *params, **kwargs)

    ACTION = "diff"
    PARAM_NAME = "EXECUTE_TYPE"

    def fill_parser_arguments(self):
        releases = ", ".join(ServiceTypes.all())

        self.parser.add_argument("METADATA", type=str, nargs=2,
                                 help='can be any of:\n'
                                      '- configuration file with remote endpoint definition\n'
                                      '- RSD release\n'
                                      '  Possible values are:\n'
                                      '  {releases}\n'
                                      '- path to directory that holds metadata xml files\n'.format(
                                        releases=to_multiline(releases, 80, '\n  ')))

        self.parser.add_argument("-q", "--qualifiers", type=str, nargs="*")

    def process_action(self, configuration):
        print "Using CTS in version %s" % (ColorPrinter.format_text(BuildInformation.get_version(), bold=True))
        print "\nComparing...\n\n"

        with ErrorMute():
            report = self.report_name()
            with open(report, 'w') as sys.stdout:
                comparator = MetadataComparator(configuration.METADATA, configuration.qualifiers)
                result = comparator.run()
                if result is not None:
                    if result.result == Comparator.EQUAL:
                        msg = "\n\nMetadata sets {metadata} are equal\n\n".format(metadata=" and ".join(configuration.METADATA))
                        sys.stderr.write(msg)
                        sys.stdout.write(msg)
                    else:
                        msg = "\n\nMetadata sets {metadata} differ\n\n".format(metadata=" and ".join(configuration.METADATA))
                        sys.stderr.write(msg)
                        sys.stdout.write(msg)
                        print result.get_side_by_side(*configuration.METADATA)

            sys.stdout = sys.__stdout__
            print "Report has been written to: {name}".format(name=report)

    def report_name(self):
        return "metadata_diff_{timestamp}.txt".format(timestamp = datetime.now().strftime("%Y-%m-%d-%H%M%S"))
