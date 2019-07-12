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
from cts_core.commons.services import ServiceTypes, RegisterService
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.measure.performance_information import PerformanceInformation
from cts_core.metadata.metadata_constants import MetadataConstants2_4 as MetadataConstants
from cts_core.metadata.required_types import RequiredTypes_Storage_2_4
from cts_core.validation.get.metadata_get_validator import MetadataGetValidator
from cts_framework.helpers.python.cts_test_script import CtsTestScript


@RegisterService(ServiceTypes.SS_2_5)
class ValidateGetResponses(CtsTestScript):
    TIMEOUT = 600
    DESCRIPTION = """test case validating api get responses against provided metadata"""

    def run(self):
        if self.metadata_container is None:
            return

        print "TEST_CASE::API crawling"
        api_explorer = ApiExplorer(self.metadata_container, self.configuration)
        discovery_container, status = api_explorer.discover(MetadataConstants.SERVICE_ROOT_URI,
                                                            MetadataConstants.SERVICE_ROOT)
        print "STATUS::{status}".format(status=status)

        MetadataGetValidator(self.metadata_container, RequiredTypes_Storage_2_4).validate(discovery_container)
        performance_status = self._measuring_request_responses(api_explorer)
        print("STATUS::{status}".format(status=performance_status))

    @staticmethod
    def _measuring_request_responses(api_explorer):
        print("TEST_CASE::API Response Time")
        pi = PerformanceInformation(api_explorer.get_performance_information())
        pi.generate_csv_performance_report()
        pi.show_performance_data()

        return pi.get_status


ValidateGetResponses.service_cli()
