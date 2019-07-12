#!/usr/bin/env python
"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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

from cts_core.commons.services import RegisterService, ServiceTypes
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.metadata.metadata_constants import MetadataConstants2_5 as MetadataConstants
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.helpers.python.cts_test_script import CtsTestScript
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.helpers.python.arch_spec_tests import ArchSpecTests
from os import listdir
from os.path import expanduser
import json


def _skip_pnc_tests_if_resource_is_not_available(test):
    list_of_tests_using_pnc = [
        "6_2_6",
        "6_2_7",
        "6_2_11",
        "6_3_1",
        "6_3_2"
    ]
    if test not in list_of_tests_using_pnc:
        return False
    return True


@RegisterService(ServiceTypes.SS_2_5)
class ArchSpecValidation(CtsTestScript):
    TIMEOUT = 600
    DESCRIPTION = """Test case validating setup with architecture specification"""

    def run(self):
        api_explorer = ApiExplorer(self.metadata_container, self.configuration)
        discovery_container, status = api_explorer.discover(MetadataConstants.SERVICE_ROOT_URI,
                                                            MetadataConstants.SERVICE_ROOT)
        status = ValidationStatus.PASSED

        catalog = expanduser("~") + "/.cts/tests/Rack_Scale_2_5_Storage_Services/required/tests_archeo_ss/"
        for test in listdir(catalog):
            with open(catalog + test) as f:
                data = json.load(f)
                test_name = data["name"]
                print "TEST_CASE::Validating section " + ColorPrinter.format_text(test_name,
                                                                                  color=ColorPrinter.YELLOW_FORE) + \
                      " of architecture specification"
                status_of_test = ArchSpecTests().parse_json(discovery_container, data)
                if _skip_pnc_tests_if_resource_is_not_available(test) and \
                        (status_of_test == ValidationStatus.FAILED):
                    status = ValidationStatus.join_statuses(status, ValidationStatus.PASSED_WITH_WARNINGS)
                    print "MESSAGE:: CTS can not detect a PNC. Skipping this tests"
                else:
                    status = ValidationStatus.join_statuses(status, status_of_test)
                print "STATUS::{status}".format(status=status_of_test)
        print "MESSAGE::All tests finished"
        print "STATUS::{status}".format(status=status)


ArchSpecValidation.service_cli()
