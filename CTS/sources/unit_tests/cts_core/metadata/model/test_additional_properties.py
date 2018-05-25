import unittest

from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.validation.validation_status import ValidationStatus
from unit_tests.helpers.stdout_capture import StdoutCapture

class AdditionalPropertiesValidationTest(unittest.TestCase):
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
            <Property Name="Name" Nullable="false" Type="Edm.String"/>
          </ComplexType>
        </Schema>
    """

    def setUp(self):
        self.discovery_container = DiscoveryContainer()

        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.\
            read_metadata_from_strings("Unknwon", AdditionalPropertiesValidationTest.METADATA)

    def test_should_raise_an_error_when_complex_type_additional_property_without_odatatype(self):
        """
        when additional property of type 'ComplexType' does not have odata.type specified
         we cannot validate it against metadata
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

        entity_type = EntityJson["@odata.type"]

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED,
                             self.metadata_container.entities[entity_type].validate(EntityJson, ''))
        self.assertIn('ERROR::Property ->Outer->InnerToNotValidate is of complex type without @odata_type',
                      output.raw)


    def test_should_validate_complex_type_additional_typed_property_and_pass(self):
        """
        when additional property of type 'ComplexType' does have odata.type specified
         we can validate it against metadata.
         In this case Inner complex type property is in conformity with metadata.
        """
        EntityJson = {
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

        entity_type = EntityJson["@odata.type"]

        self.assertEqual(ValidationStatus.PASSED, self.metadata_container.entities[entity_type].validate(EntityJson, ''))


    def test_should_validate_complex_type_additional_typed_property_and_fail(self):
        """
        when additional property of type 'ComplexType' does have odata.type specified
         we can validate it against metadata.
         In this case Inner complex type property is not conformant with metadata.
        """
        EntityJson = {
            "@odata.type": "MyNamespace.v1_0_0.MyEntity",
            "Outer": {
                "@odata.type": "MyNamespace.v1_0_0.OuterComplexType",
                "Name": "OuterName",
                "InnerValidate": {
                    "@odata.type": "MyNamespace.v1_0_0.InnerComplexType",
                    "foo": "bar"
                }
            }
        }

        entity_type = EntityJson["@odata.type"]

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.metadata_container.entities[entity_type].validate(EntityJson, ''))

        self.assertIn('ERROR::Property ->Outer->InnerValidate->foo is present, but not defined in metadata. ' + \
                      'Type MyNamespace.v1_0_0.InnerComplexType does not allow additional properties', output.raw)

    def test_should_raise_error_on_complex_additional_property_without_odata_type(self):
        """
        when additional property inside Entity does not have odata.type specified and it is not DynamicProperty,
         CTS should raise an error
        """
        EntityJson = {
            "@odata.type": "MyNamespace.v1_0_0.MyEntity",
            "EntityLevelAdditional": {
                # odatatype intentionally skipped
                "foo": "bar"
            },
            "Outer": {
                "@odata.type": "MyNamespace.v1_0_0.OuterComplexType",
                "Name": "OuterName",
            }
        }

        entity_type = EntityJson["@odata.type"]

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED,
                             self.metadata_container.entities[entity_type].validate(EntityJson, ''))
        self.assertIn('ERROR::Property url=->EntityLevelAdditional is of complex type without '
                      '@odata_type',
                      output.raw)

    def test_should_validate_entity_additional_typed_property_and_pass(self):
        """
        when additional property inside Entity does have odata.type specified
         we can validate it against metadata.
         In this case Additonal complex type property is in conformity with metadata.
        """
        EntityJson = {
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

        entity_type = EntityJson["@odata.type"]

        self.assertEqual(ValidationStatus.PASSED, self.metadata_container.entities[entity_type].validate(EntityJson, ''))


    def test_should_validate_entity_additional_typed_property_and_fail(self):
        """
        when additional property in Entity does have odata.type specified
         we can validate it against metadata.
         In this case additional complex type property is not conformant with metadata.
        """
        EntityJson = {
            "@odata.type": "MyNamespace.v1_0_0.MyEntity",
            "AdditionalPropertyToValidate": {
                "@odata.type": "MyNamespace.v1_0_0.InnerComplexType",
                "foo": "bar"
            },
            "Outer": {
                "@odata.type": "MyNamespace.v1_0_0.OuterComplexType",
                "Name": "OuterName",
            }
        }

        entity_type = EntityJson["@odata.type"]

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED,
                             self.metadata_container.entities[entity_type].validate(EntityJson, ''))

        self.assertIn('ERROR::Property ->AdditionalPropertyToValidate->foo is present, but not defined in metadata. ' \
                      'Type MyNamespace.v1_0_0.InnerComplexType does not allow additional properties;', output.raw)
