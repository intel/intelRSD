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
from cts_core.metadata.metadata_constants import MetadataConstants2_2 as MetadataConstants
from cts_core.metadata.required_types import RequiredTypes_PODM_2_2
from cts_core.discovery.discovery_container import equal
from cts_core.validation.get.metadata_get_validator import MetadataGetValidator
from cts_framework.helpers.python.cts_test_script import CtsTestScript


# if all the systems are virtual (PNC), we don't require any memories, processors or vlan collections
def SkipCheckPredicate_PODM_2_2(discovery_container, requirement):
    if requirement.base in [MetadataConstants.MEMORY, MetadataConstants.PROCESSOR,
                            MetadataConstants.VLAN_NETWORK_INTERFACE_COLLECTION]:
        num_systems = discovery_container.count_resources(MetadataConstants.COMPUTER_SYSTEM, any_child_version=True)
        num_virtual_systems = len(discovery_container.get_resources(MetadataConstants.COMPUTER_SYSTEM,
                                                any_child_version=True, constraints=[equal("SystemType", "Virtual")]))
        return (num_systems > 0 and num_systems == num_virtual_systems)

@RegisterService(ServiceTypes.PODM_2_2)
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

        MetadataGetValidator(self.metadata_container, RequiredTypes_PODM_2_2,
                             skip_check_predicate=SkipCheckPredicate_PODM_2_2).validate(discovery_container)

ValidateGetResponses.service_cli()
