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
from cts_core.metadata.metadata_constants import MetadataConstants2_1 as MetadataConstants
from cts_core.metadata.required_types import RequiredTypes_Storage_2_1
from cts_core.validation.patch.metadata_patch_validator import MetadataPatchValidator
from cts_core.validation.patch.patching_strategy_2_1 import PatchingStrategy2_1
from cts_framework.helpers.python.cts_test_script import CtsTestScript


@RegisterService(ServiceTypes.SS_2_1)
class ValidatePatchResponses(CtsTestScript):
    TIMEOUT = 600
    DESCRIPTION = """test case validating api patches against provided metadata"""

    def run(self):
        if self.metadata_container is None:
            return

        print "TEST_CASE::API crawling"
        api_explorer = ApiExplorer(self.metadata_container, self.configuration)

        discovery_container, status = api_explorer.discover(MetadataConstants.SERVICE_ROOT_URI,
                                                            MetadataConstants.SERVICE_ROOT)
        print "STATUS::{status}".format(status=status)

        validator = MetadataPatchValidator(self.metadata_container, self.configuration, PatchingStrategy2_1(), RequiredTypes_Storage_2_1)
        validator.validate(discovery_container)

ValidatePatchResponses.service_cli()
