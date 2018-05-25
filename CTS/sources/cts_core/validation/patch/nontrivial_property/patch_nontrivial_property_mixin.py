from cts_core.commons.error import cts_warning
from cts_core.validation.patch.nontrivial_property.patch_nontrivial_property_boot import PatchNontrivialPropertyBoot
from cts_core.validation.patch.nontrivial_property.patch_nontrivial_property_chap import PatchNontrivialPropertyChap
from cts_core.validation.validation_status import ValidationStatus
from patch_nontrivial_property_base import ApplicabilityTestResult

NONTRIVIAL_PATCH_IMPLEMENTATIONS = [PatchNontrivialPropertyBoot]


class PatchNontrivialPropertyMixin:
    def patch_nontrivial(self, api_resource, variable_path, property_description):
        for implementation in NONTRIVIAL_PATCH_IMPLEMENTATIONS:
            applicability = implementation.test_applicability(self._metadata_container, property_description.type)

            # matched but will not be handled by specialization
            if applicability in [ApplicabilityTestResult.SKIP_PROPERTY, ApplicabilityTestResult.PATCH_AS_TRIVIAL]:
                return applicability, None

            # specialized handler matched
            if applicability == ApplicabilityTestResult.MATCHED:
                instance = implementation(self._metadata_container, self.discovery_container, self._api_caller,
                                          self._strategy, api_resource, variable_path, property_description)
                return applicability, instance.handle_nontrivial_property()

        # handler for non-trivial not found
        return ApplicabilityTestResult.NOT_MATCHED, None

    def handle_nontrivial(self, context, variable_path, property_description):
        """
        :param context:
        :type context:
        :param variable_path:
        :type variable_path:
        :param property_description:
        :type property_description:
        :return:
        :rtype:
        """
        applicability, validation_result = self.patch_nontrivial(context.api_resource,
                                                                 variable_path,
                                                                 property_description)

        if applicability in [ApplicabilityTestResult.SKIP_PROPERTY, ApplicabilityTestResult.NOT_MATCHED]:
            cts_warning("Skipping non-trivial property {name}", name=property_description)
            return ApplicabilityTestResult.SKIP_PROPERTY, ValidationStatus.PASSED_WITH_WARNINGS

        context.register_patch_attempt(self, property_description.name)
        return applicability, validation_result
