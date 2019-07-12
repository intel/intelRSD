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

from cts_framework.actions.action import Action
from cts_framework.build_information import BuildInformation
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.commons.logging_helper import LoggingHelper

import pkg_resources
import subprocess


class SosAction(Action):
    ACTION = "sos"
    PARAM_NAME = "ACTION"

    def __init__(self, *params, **kwargs):
        Action.__init__(self, *params, **kwargs)
        self._logger = LoggingHelper(__name__)

    def process_action(self, configuration):
        """
        The SOS process collects the information needed for easier debugging
        :return:
        """
        print "Using SOS CTS in version %s" % (ColorPrinter.format_text(BuildInformation.get_version(), bold=True))
        SOS_SCRIPT_PATH = ""
        SOS_SCRIPT_NAME = "cts-sos.sh"

        resource_package = __name__
        resource_path = '/'.join((SOS_SCRIPT_PATH, SOS_SCRIPT_NAME))

        script_path = pkg_resources.resource_filename(resource_package, resource_path)
        subprocess.call(['sh', script_path])
