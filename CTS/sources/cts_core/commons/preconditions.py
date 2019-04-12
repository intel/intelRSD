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

from cts_core.commons.error import cts_error, cts_warning
from cts_core.validation.validation_status import ValidationStatus


class Requirement:
    def __init__(self, base, min=None, max=None, optional=False):
        self.base = base
        self.min = min
        self.max = max
        self.optional = optional


class Preconditions:
    def __init__(self, metadata_container, requirements, skip_check_predicate=None):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type requirements: list(Requirement)
        """
        self._metadata_container = metadata_container
        self.requirements = requirements
        self._skip_check_predicate = skip_check_predicate

    def validate(self, discovery_container):
        """
        :type discovery_container: cts_core.discovery.discovery_container.DiscoveryContainer
        :rtype: cts_core.validation.validation_status.ValidationStatus
        """
        return self._precondition_check(discovery_container)

    def _precondition_check(self, discovery_container):
        """
        :type discovery_container: cts_core.discovery.discovery_container.DiscoveryContainer
        :rtype: cts_core.validation.validation_status.ValidationStatus
        """
        status = ValidationStatus.PASSED

        if self.requirements is None:
            return status

        for requirement in self.requirements:
            if requirement.base not in self._metadata_container.entities:
                cts_error("Internal Error. Type {base} not found in metadata", base=requirement.base)
                status = ValidationStatus.FAILED

        discovered = dict()
        for requirement in self.requirements:
            discovered[requirement.base] = discovery_container.count_resources(requirement.base, any_child_version=True)

        for requirement in self.requirements:
            if self._skip_check_predicate and self._skip_check_predicate(discovery_container, requirement):
                continue

            if (requirement.optional is True and discovered.setdefault(requirement.base, 0) < requirement.min):
                cts_warning("Minimum number of {type} is {min} to make this tests. Current number: {current}. "
                            "Tests will be skipped",
                            type=requirement.base, min=requirement.min, current=discovered[requirement.base])
                status = ValidationStatus.PASSED_WITH_WARNINGS
                continue

            if (requirement.min is not None and discovered.setdefault(requirement.base, 0) < requirement.min):
                cts_error("Minimum number of {type} is {min}. Current number: {current}",
                          type=requirement.base, min=requirement.min, current=discovered[requirement.base])
                status = ValidationStatus.FAILED

            if (requirement.max is not None and discovered.setdefault(requirement.base, 0) > requirement.max):
                cts_error("Maximum number of {type} is {max}. Current number: {current}",
                          type=requirement.base, max=requirement.max, current=discovered[requirement.base])
                status = ValidationStatus.FAILED

        return status
