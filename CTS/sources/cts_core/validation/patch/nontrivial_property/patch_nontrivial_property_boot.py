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
from cts_core.commons.json_comparator import JsonComparator
from cts_core.validation.patch.nontrivial_property.patch_nontrivial_property_base import PatchNonTrivialPropertyBase, \
    ApplicabilityTestResult
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import RequestStatus


class PatchNontrivialPropertyBoot(PatchNonTrivialPropertyBase):
    PROPERTY_NAME = "Boot"
    TYPE = "ComputerSystem.v1_0_0.Boot"

    @staticmethod
    def test_applicability(metadata_container, property_type):
        try:
            if metadata_container.types[PatchNontrivialPropertyBoot.TYPE].is_compatible(property_type):
                return ApplicabilityTestResult.MATCHED
        except KeyError:
            return ApplicabilityTestResult.NOT_MATCHED

    def payload(self):
        yield {
            "BootSourceOverrideEnabled": "Once",
            "BootSourceOverrideTarget": "Pxe",
            "BootSourceOverrideMode": "UEFI"
               }

    def verify(self, api_resource, expected_value):
        try:
            value_post = self.get_property_value()
        except KeyError:
            return ValidationStatus.FAILED

        if JsonComparator.compare_json_to_golden(value_post, expected_value):
            return ValidationStatus.PASSED
        else:
            return ValidationStatus.FAILED

