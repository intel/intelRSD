import unittest

from mock import mock, MagicMock

from cts_core.discovery.api_resource import ApiResource
from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.validation.patch.metadata_patch_validator import MetadataPatchValidator
from cts_core.validation.patch.patching_strategy_2_1 import PatchingStrategy2_1
from cts_core.validation.validation_status import ValidationStatus
from unit_tests.helpers.stdout_capture import StdoutCapture

class AdditionalPropertiesPatchTest(unittest.TestCase):
    METADATA = """
        <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="MyNamespace.v1_0_0">
          <EntityType Name="MyEntity">
            <Annotation Term="OData.AdditionalProperties" Bool="true"/>
            <Property Name="Outer" Nullable="false" Type="OuterComplexType"/>
          </EntityType>
          <ComplexType Name="OuterComplexType">
            <Annotation Term="OData.AdditionalProperties" Bool="true"/>
            <Property Name="Name" Nullable="false" Type="Edm.String"/>
          </ComplexType>
          <ComplexType Name="InnerComplexType">
            <Annotation Term="OData.AdditionalProperties" Bool="false"/>
            <Property Name="Name" Nullable="false" Type="Edm.String">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/ReadWrite"/>
            </Property>
            <Property Name="NotRequired" type="Edm.String">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/ReadWrite"/>
            </Property>
          </ComplexType>
          <ComplexType Name="SecondInnerComplexType">
            <Annotation Term="OData.AdditionalProperties" Bool="false"/>
            <Property Name="Name" Nullable="false" Type="Edm.String">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Write"/>
            </Property>
            <Property Name="NotRequired" type="Edm.String">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Write"/>
            </Property>
          </ComplexType>
        </Schema>
    """

    def setUp(self):
        self.discovery_container = DiscoveryContainer()

        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", AdditionalPropertiesPatchTest.METADATA)
        self.discovery_container = DiscoveryContainer()


    def test_should_not_patch_complex_type_level_additional_property_without_odatatype(self):
        """
        when additional property of type 'ComplexType' does not have odata.type specified
         we cannot validate it
        """
        EntityJson = {
            "@odata.type": "MyNamespace.v1_0_0.MyEntity",
            "Outer" : {
                "@odata.type": "MyNamespace.v1_0_0.OuterComplexType",
                "Name" : "OuterName",
                "InnerToNotValidate" : {
                    #odatatype intentionally skipped
                    "foo" : "bar"
                }
            }
        }

        self.discovery_container.add_resource(ApiResource("/redfish/v1/Managers/RMC", "netloc",
                                              EntityJson, "#MyNamespace.v1_0_0.MyEntity"))

        with mock.patch('cts_core.commons.api_caller.ApiCaller.__init__') as api_caller_init_mock:
            api_caller_init_mock.return_value = None

            validator = MetadataPatchValidator(self.metadata_container, None, PatchingStrategy2_1())
            validator._patch_and_verify_property = MagicMock(return_value=ValidationStatus.PASSED)

            with StdoutCapture() as output:
                self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))

            validator._patch_and_verify_property.assert_not_called()

    def test_should_patch_complex_type_level_additional_typed_property(self):
        """
        when additional property of type 'ComplexType' does have odata.type specified
         we can validate it
        """
        EntityJson = {
            "@odata.id": "odata.id",
            "@odata.type": "MyNamespace.v1_0_0.MyEntity",
            "Outer": {
                "@odata.type": "MyNamespace.v1_0_0.OuterComplexType",
                "Name": "OuterName",
                "InnerToValidate": {
                    "@odata.type": "MyNamespace.v1_0_0.InnerComplexType",
                    "Name": "This name property is required"
                }
            }
        }

        self.discovery_container.add_resource(ApiResource("/redfish/v1/Managers/RMC", "netloc",
                                                          EntityJson, "#MyNamespace.v1_0_0.MyEntity"))


        with mock.patch('cts_core.commons.api_caller.ApiCaller.__init__') as api_caller_init_mock:
            api_caller_init_mock.return_value = None

            validator = MetadataPatchValidator(self.metadata_container, None, PatchingStrategy2_1())
            validator._patch_and_verify_property = MagicMock(return_value= (ValidationStatus.PASSED, "new_value"))
            validator._restore_property = MagicMock(return_value=ValidationStatus.PASSED)


            with StdoutCapture() as output:
                self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))
            self.assertGreater(validator._patch_and_verify_property.call_count, 0)



    def test_should_not_patch_entity_level_additional_property_without_odatatype(self):
        """
        when additional property inside Entity does not have odata.type specified
         we cannot validate it
        """
        EntityJson = {
            "@odata.type": "MyNamespace.v1_0_0.MyEntity",
            "EntityLevelAdditionalToNotValidate": {
                # odatatype intentionally skipped
                "foo": "bar"
            },
            "Outer": {
                "@odata.type": "MyNamespace.v1_0_0.OuterComplexType",
                "Name": "OuterName",
            }
        }

        # entity_type = EntityJson["@odata.type"]
        self.discovery_container.add_resource(ApiResource("/redfish/v1/Managers/RMC", "netloc",
                                                          EntityJson, "#MyNamespace.v1_0_0.MyEntity"))

        with mock.patch('cts_core.commons.api_caller.ApiCaller.__init__') as api_caller_init_mock:
            api_caller_init_mock.return_value = None
            validator = MetadataPatchValidator(self.metadata_container, None, PatchingStrategy2_1())
            validator._patch_and_verify_property = MagicMock(return_value=ValidationStatus.PASSED)

            with StdoutCapture() as output:
                self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))

            validator._patch_and_verify_property.assert_not_called()



    def test_should_patch_entity_level_additional_typed_property(self):
        """
        when additional property inside Entity does have odata.type specified
         we can validate it against metadata.
        """
        EntityJson = {
            "@odata.id": "odata.id",
            "@odata.type": "MyNamespace.v1_0_0.MyEntity",
            "AdditionalPropertyToValidate": {
                "@odata.type": "MyNamespace.v1_0_0.InnerComplexType",
                "Name": "This name property is required"
            },
            "Outer": {
                "@odata.type": "MyNamespace.v1_0_0.OuterComplexType",
                "Name": "OuterName",
            }
        }

        self.discovery_container.add_resource(ApiResource("/redfish/v1/Managers/RMC", "netloc",
                                                          EntityJson, "#MyNamespace.v1_0_0.MyEntity"))

        with mock.patch('cts_core.commons.api_caller.ApiCaller.__init__') as api_caller_init_mock:
            api_caller_init_mock.return_value = None
            validator = MetadataPatchValidator(self.metadata_container, None, PatchingStrategy2_1())
            validator._patch_and_verify_property = MagicMock(
                return_value=(ValidationStatus.PASSED, "new_value"))
            validator._restore_property = MagicMock(return_value=ValidationStatus.PASSED)

            with StdoutCapture() as output:
                self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))
            self.assertGreater(validator._patch_and_verify_property.call_count, 0)

    def test_should_pass_and_not_patch_write_entity(self):
        ENTITYJSON = {
            "@odata.type": "MyNamespace.v1_0_0.MyEntity",
            "Outer": {
                "@odata.type": "MyNamespace.v1_0_0.OuterComplexType",
                "Name": "OuterName",
                "InnerToValidate": {
                    "@odata.type": "MyNamespace.v1_0_0.SecondInnerComplexType",
                    "Name": "This name property is required"
                }
            }
        }

        self.discovery_container.add_resource(ApiResource("/redfish/v1/Managers/RMC", "netloc", ENTITYJSON,
                                                          "#MyNamespace.v1_0_0.MyEntity"))

        with mock.patch('cts_core.commons.api_caller.ApiCaller.__init__') as api_caller_init_mock:
            api_caller_init_mock.return_value = None
            validator = MetadataPatchValidator(self.metadata_container, None, PatchingStrategy2_1())

            # return of _verify_property should not block test if OData.Permission is Write
            validator._verify_property = MagicMock(return_value=(ValidationStatus.BLOCKED, None))

            validator._patch_property = MagicMock(return_value=(True, True, ValidationStatus.PASSED))
            validator._restore_property = MagicMock(return_value=ValidationStatus.PASSED)

            with StdoutCapture():
                self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))
            # _verify_property should not be called if OData.Permission is Write
            self.assertEqual(validator._verify_property.call_count, 0)

