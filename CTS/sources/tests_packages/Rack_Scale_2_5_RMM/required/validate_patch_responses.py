#!/usr/bin/env python
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
from cts_core.commons.preconditions import Requirement
from cts_core.commons.services import ServiceTypes, RegisterService
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.measure.performance_information import PerformanceInformation
from cts_core.metadata.metadata_constants import MetadataConstants_RMM_2_5 as MetadataConstants
from cts_core.validation.patch.metadata_patch_validator import MetadataPatchValidator
from cts_core.validation.patch.patching_strategy_2_5 import PatchingStrategy2_5
from cts_framework.helpers.python.cts_test_script import CtsTestScript
from cts_framework.helpers.python.test_script import TestScript


@RegisterService(ServiceTypes.RMM_2_5)
class ValidatePatchResponses(CtsTestScript):
    TIMEOUT = 600
    DESCRIPTION = """test case validating api patches against provided metadata"""

    CONFIGURATION_PARAMETERS = [
        TestScript.ConfigurationParameter(parameter_name="IgnoredElements",
                                          parameter_description="Provide a list of elements there will be ignored in patch",
                                          parameter_type=str, is_required=False, parameter_default_value="")]

    def run(self):
        if self.metadata_container is None:
            return

        print "TEST_CASE::API crawling"
        api_explorer = ApiExplorer(self.metadata_container, self.configuration)

        discovery_container, status = api_explorer.discover(MetadataConstants.SERVICE_ROOT_URI,
                                                            MetadataConstants.SERVICE_ROOT)
        print "STATUS::{status}".format(status=status)

        requirements = [
            Requirement(MetadataConstants.SERVICE_ROOT, min=1, max=1)
        ]
        validator = MetadataPatchValidator(self.metadata_container, self.configuration,
                                           PatchingStrategy2_5(), requirements,
                                           skip_list=self.configuration.IgnoredElements)
        validator.validate(discovery_container)
        performance_status = self._measuring_request_responses(api_explorer)
        print("STATUS::{status}".format(status=performance_status))

    @staticmethod
    def _measuring_request_responses(api_explorer):
        print("TEST_CASE::API Response Time")
        pi = PerformanceInformation(api_explorer.get_performance_information())
        pi.generate_csv_performance_report()
        pi.show_performance_data()

        return pi.get_status
ValidatePatchResponses.service_cli()
