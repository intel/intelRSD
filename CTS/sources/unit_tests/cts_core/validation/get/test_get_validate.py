import unittest

import re
from mock import mock

from cts_core.commons.links_factory import Link
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.discovery.api_resource import ApiResource
from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.validation.get.metadata_get_validator import MetadataGetValidator
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import RequestStatus, ReturnCodes
from cts_framework.configuration.configuration import Configuration
from unit_tests.helpers.stdout_capture import StdoutCapture

API_ENDPOINT = "1.2.3.4:567"

METADATA = """
  <Schemas>
    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="N">
        <ComplexType Name="ObjectWithNumericProperty">
            <Annotation Term="OData.AdditionalProperties" Bool="true"/>
            <Annotation Term="Redfish.DynamicPropertyPatterns">
                <Collection>
                    <Record>
                        <PropertyValue Property="Pattern" String="[a]+"/>
                        <PropertyValue Property="Type" String="N.Referenced"/>
                    </Record>
                </Collection>
            </Annotation>

            <Property Name="Number" Type="Edm.Decimal" Nullable="false" />
            <Property Name="OptionalPropertyInComplex1" Type="Edm.Int64"/>
            <Property Name="OptionalPropertyInComplex2" Type="Edm.Int64"/>
            <Property Name="OptionalPropertyInComplex3" Type="Edm.Int64"/>
        </ComplexType>

        <ComplexType Name="Referenced">
            <Property Name="Obligatory" Type="Edm.Decimal" Nullable="false">
                <Annotation Term="Redfish.Required"/>
            </Property>
        </ComplexType>

        <EntityType Name="R" Abstract="false">
            <Annotation Term="OData.AdditionalProperties" Bool="true"/>
            <Annotation Term="Redfish.DynamicPropertyPatterns">
                <Collection>
                    <Record>
                        <PropertyValue Property="Pattern" String="[a]+"/>
                        <PropertyValue Property="Type" String="N.Referenced"/>
                    </Record>
                </Collection>
            </Annotation>

            <Property Name="ObjectWithNumber" Type="N.ObjectWithNumericProperty" Nullable="false">
		        <Annotation Term="Redfish.Required" />
		    </Property>
		    <Property Name="CapacityBytes" Type="Edm.Int64"/>
		    <Property Name="CapacityGiB" Type="Edm.Single" Nullable="false">
            </Property>
            <Property Name="OptionalProperty1" Type="Edm.Int64"/>
            <Property Name="OptionalProperty2" Type="Edm.Int64"/>
            <Property Name="OptionalProperty3" Type="Edm.Int64"/>
        </EntityType>
    </Schema>
  </Schemas>
       """

RESOURCE_CORRECT = \
    {
        "@odata.id": "resource.id",
        "@odata.type": "#N.R",
        "CapacityBytes": 1000000000000,
        "CapacityGiB": 372.6114120483399,
        "ObjectWithNumber": {
            "Number": 667,
            "xOptionalPropertyInComplex": 11,
            "OptionalPropertyInComplex2": 11,
        },
        "xOptionalProperty": 11,
        "OptionalProperty2": 11,
    }

RESOURCE_WITH_UNKNOWN_ENTITY_TYPE = \
    {
        "@odata.id": "resource.id",
        "@odata.type": "unknown.entity.type",
        "CapacityBytes": 1000000000000,
        "ObjectWithNumber": {
            "Number" : 667
        }
    }

RESOURCE_WITH_UNKNOWN_COMPLEX_TYPE = \
    {
        "@odata.id": "resource.id",
        "@odata.type": "#N.R",
        "CapacityBytes": 1000000000000,
        "ObjectWithNumber": {
            "@odata.type" : "unknown.complex.type",
            "Number" : 667
        }
    }


RESOURCE_WITH_INVALID_PROPERTY_VALUE = \
        {
            "@odata.id" : "resource.id",
            "@odata.type": "#N.R",
            "CapacityBytes": 1000000000000,
            "ObjectWithNumber": "Object expected",
        }

RESOURCE_WITH_NULL_ON_NON_NULLABLE = \
        {
            "@odata.id": "resource.id",
            "@odata.type": "#N.R",
            "CapacityBytes": 1000000000000,
            "CapacityGiB": None,
            "ObjectWithNumber": {
                "Number" : None
            }
        }

RESOURCE_WITH_INCOMPLETE_DYNAMIC_PROPERTY = \
    {
        "@odata.id": "resource.id",
        "@odata.type": "#N.R",
        "CapacityBytes": 1000000000000,
        "CapacityGiB": 372.6114120483399,
        "ObjectWithNumber": {
            "Number": 667,
            "xOptionalPropertyInComplex": 11,
            "OptionalPropertyInComplex2": 11,
        },
        "xOptionalProperty": 11,
        "OptionalProperty2": 11,
        "aaaaaaaa": {
            'unexpected_info': 'Obligatory property is missing'
        }
    }

RESOURCE_WITH_INCOMPLETE_DYNAMIC_PROPERTY_AT_COMPLEX_TYPE_LEVEL = \
    {
        "@odata.id": "resource.id",
        "@odata.type": "#N.R",
        "CapacityBytes": 1000000000000,
        "CapacityGiB": 372.6114120483399,
        "ObjectWithNumber": {
            "Number": 667,
            "xOptionalPropertyInComplex": 11,
            "OptionalPropertyInComplex2": 11,
            "aaaaaaaa": {
                'unexpected_info': 'Obligatory property is missing'
            }
        },
        "xOptionalProperty": 11,
        "OptionalProperty2": 11,
    }


class GetValidateTest(unittest.TestCase):
    def test_should_pass_with_correct_resource(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_CORRECT,
                                                                                "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))


    def test_should_fail_bacause_of_unknown_entity_type(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_WITH_UNKNOWN_ENTITY_TYPE,
                                                                                "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, validator.validate(self.discovery_container))

        self.assertIn("ERROR::Unknown entity type 'unknown.entity.type'", output.raw)

    def test_should_pass_when_unknown_entity_type_mapped_to_known_one(self):
        metadata_manager = MetadataManager(["qualifier"], map_types={"A" : "B", "unknown.entity.type" : "N.R"})
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_WITH_UNKNOWN_ENTITY_TYPE,
                                                                                "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))

    def test_should_fail_bacause_of_unknown_complex_type(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_WITH_UNKNOWN_COMPLEX_TYPE,
                                                                                "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, validator.validate(self.discovery_container))

        self.assertIn('Reference to unknown type unknown.complex.type', '\n'.join(output))

    def test_should_pass_when_unknown_complex_type_mapped_on_known_type(self):
        metadata_manager = MetadataManager(["qualifier"], map_types={"A" : "B", "unknown.complex.type" : "N.ObjectWithNumericProperty"})
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_WITH_UNKNOWN_COMPLEX_TYPE,
                                                                                "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))


    def test_should_fail_if_property_of_incorrect_type(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_WITH_INVALID_PROPERTY_VALUE,
                                                                                "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, validator.validate(self.discovery_container))

        self.assertIn('Property resource.id->ObjectWithNumber is expected to be json object', '\n'.join(output))

    def test_should_pass_if_property_of_incorrect_type_declared_on_ignore_list(self):
        metadata_manager = MetadataManager(["qualifier"], ignore_types=["N.ObjectWithNumericProperty"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_WITH_INVALID_PROPERTY_VALUE,
                                                                                "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))


    def test_should_pass_if_entity_type_declared_on_ignore_list(self):
        metadata_manager = MetadataManager(["qualifier"], ignore_types=["N.R"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_WITH_INVALID_PROPERTY_VALUE,
                                                                                "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))

    def test_should_pass_if_entity_type_declared_on_ignore_list_with_asterix(self):
        metadata_manager = MetadataManager(['qualifier'], ignore_types=["N.*"])
        self.metadata_container = metadata_manager.read_metadata_from_strings('Unknown', METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource('/redfish/resource1',
                                                          'netloc',
                                                          RESOURCE_WITH_INVALID_PROPERTY_VALUE,
                                                          '#N.R'))

        requirements = []
        validator = MetadataGetValidator(self.metadata_container, requirements)

        self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))

    def test_should_fail_if_non_nullable_property_is_null(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_WITH_NULL_ON_NON_NULLABLE,
                                                                                "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, validator.validate(self.discovery_container))

        self.assertIn('ERROR::Property resource.id->CapacityGiB cannot be null', '\n'.join(output))
        self.assertIn('ERROR::Property resource.id->ObjectWithNumber->Number cannot be null', '\n'.join(output))

    def test_should_suggest_other_names_for_additionals(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_CORRECT,
                                                                                "#N.R"))

        requirements = []
        validator = MetadataGetValidator(self.metadata_container, requirements)

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))

        self.assertIn('Schema defines other, not used, but similar properties: [OptionalProperty1, OptionalProperty3]',
                      '\n'.join(output))
        self.assertIn(
            'Schema defines other, not used, but similar properties: [OptionalPropertyInComplex1, OptionalPropertyInComplex3]',
            '\n'.join(output))

    def test_should_validate_dynamic_properties_at_entity_level(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(ApiResource("/redfish/resource1",
                                                          'netloc', RESOURCE_WITH_INCOMPLETE_DYNAMIC_PROPERTY,
                                                          "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, validator.validate(self.discovery_container))

        self.assertIn('ERROR::Required property resource.id->aaaaaaaa->Obligatory not present in the resource',
                      '\n'.join(output))

    def test_should_validate_dynamic_properties_at_complex_type_level(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()

        self.discovery_container.add_resource(
            ApiResource("/redfish/resource1",
                        'netloc',
                        RESOURCE_WITH_INCOMPLETE_DYNAMIC_PROPERTY_AT_COMPLEX_TYPE_LEVEL,
                        "#N.R"))

        requirements = [
        ]
        validator = MetadataGetValidator(self.metadata_container, requirements)

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, validator.validate(self.discovery_container))

        self.assertIn(
            'ERROR::Required property resource.id->ObjectWithNumber->aaaaaaaa->Obligatory not present in the resource',
            ';'.join(output))

    @unittest.skip("Temporary change related to bug in Discovery mechanism")
    def test_validate_elements_of_collection_with_elements_referenced_by_out_of_range_json_pointer(self):
        metadata = """
                    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="N">
                        <EntityType Name="Outer" Abstract="false">
                            <NavigationProperty Name="MyCollection" Type="Collection(ReferencedEntity)" ContainsTarget="true">
                                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                            </NavigationProperty>
                        </EntityType>

                        <EntityType Name="ReferencedEntity" Abstract="false">
                            <Property Name="name" Type="Edm.String"/>
                        </EntityType>
                    </Schema>
                    """

        resource = \
            {
                "@odata.id": "resource.id",
                "@odata.type": "N.Outer",
                "MyCollection": [
                    {
                        "@odata.id": "/outer#MyCollection/0"
                    },
                    {
                        "@odata.id": "/outer#MyCollection/2"  # pointer out of range
                    }
                ]
            }

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings("Unknown", metadata)
        discovery_container = DiscoveryContainer()
        configuration = Configuration(**dict(UseSSL='True', ApiEndpoint=API_ENDPOINT))
        api_explorer = ApiExplorer(metadata_container, configuration)

        with mock.patch('cts_core.commons.api_caller.ApiCaller.get_resource') as get_resource:
            get_resource.side_effect = [(Link('url1', 'netloc'), RequestStatus.SUCCESS, ReturnCodes.OK, resource,
                                         Link('url1', 'netloc')),
                                        (Link('url2', 'netloc'), RequestStatus.SUCCESS, ReturnCodes.OK, resource,
                                         Link('url2', 'netloc')),
                                        (Link('url3', 'netloc'), RequestStatus.SUCCESS, ReturnCodes.OK, resource,
                                         Link('url3', 'netloc'))]

            with StdoutCapture() as output:
                api_explorer.discover("/outer", "N.Outer", discovery_container)
            self.assertEqual(3, get_resource.call_count)
            self.assertIn(
                'ERROR::JSON pointer exception while dereferencing /MyCollection/2',
                ';'.join(output))

        requirements = [
        ]
        validator = MetadataGetValidator(metadata_container, requirements)

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.PASSED, validator.validate(discovery_container))

        self.assertEqual(0, len(re.findall('ERROR::', ';'.join(output))))
