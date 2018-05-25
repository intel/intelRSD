import unittest

from bs4 import BeautifulSoup

from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.metadata.metadata_container import MetadataContainer
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.metadata.model.property import Property
from cts_core.metadata.model.metadata_types.complex_type import ComplexType
from cts_core.metadata.model.metadata_types.enum_type import EnumType
from cts_core.validation.validation_status import ValidationStatus
from unit_tests.helpers.stdout_capture import StdoutCapture

PROPERTY = "property"


class PropertyReadUnitTest(unittest.TestCase):
    def _get_property_soup_from_property_text(self, property_text):
        return BeautifulSoup(property_text, "lxml").find_all(PROPERTY)[0]

    def test_empty_property(self):
        namespace_name = "namespace_name"
        property_name = "property_name"
        property_type = "property_type"

        property_doc = """
        <Property name="{property_name}" type="{property_type}" />
        """.format(property_name=property_name, property_type=property_type)

        property_soup = self._get_property_soup_from_property_text(property_doc)

        tested_property = Property(MetadataContainer(), namespace_name, property_soup)

        self.assertEqual(tested_property.name, property_name)
        self.assertEqual(tested_property.type, ".".join([namespace_name, property_type]))
        self.assertFalse(tested_property.is_collection)
        self.assertTrue(tested_property.nullable)
        self.assertFalse(tested_property.is_required)
        self.assertFalse(tested_property.annotations)

    def test_property_annotation(self):
        property_doc = """
        <Property name="property_name" type="property_type">
            <Annotation />
        </Property>
        """

        property_soup = self._get_property_soup_from_property_text(property_doc)

        tested_property = Property(MetadataContainer(), "", property_soup)

        self.assertEqual(len(tested_property.annotations), 1)

    def test_property_collection(self):
        property_doc = """
            <Property name="property_name" type="Collection(property_type)">
                <Annotation />
            </Property>
            """

        property_soup = self._get_property_soup_from_property_text(property_doc)

        tested_property = Property(MetadataContainer(), "", property_soup)

        self.assertTrue(tested_property.is_collection)

    def test_property_nullable(self):
        property_doc = """
            <Property name="property_name" type="property_type" nullable="true"/>
            """

        property_soup = self._get_property_soup_from_property_text(property_doc)

        tested_property = Property(MetadataContainer(), "", property_soup)

        self.assertTrue(tested_property.nullable)

    def test_property_required(self):
        property_doc = """
            <Property name="property_name" type="property_type">
                <Annotation Term="Redfish.Required" />
            </Property>
            """

        property_soup = self._get_property_soup_from_property_text(property_doc)

        tested_property = Property(MetadataContainer(), "", property_soup)

        self.assertTrue(tested_property.is_required)

    def test_property_has_value_from_allowable_list(self):
        enum_type_doc = """
        <EnumType Name="ABCEnum">
            <Member Name="A">
            <Member Name="B"/>
            <Member Name="C"/>
        </EnumType>
        """

        complex_type_doc = """
        <ComplexType Name="complex_type">
            <Property Name="abc" type="namespace.ABCEnum" />
        </ComplexType>
        """

        enum_soup = BeautifulSoup(enum_type_doc, "lxml").find_all("enumtype")[0]
        complex_soup = BeautifulSoup(complex_type_doc, "lxml").find_all("complextype")[0]

        metadata_container = MetadataContainer()
        enum_type = EnumType(metadata_container, "namespace", enum_soup)
        complex_type = ComplexType(metadata_container, "namespace", complex_soup)

        metadata_container.types[enum_type.name] = enum_type
        metadata_container.types[complex_type.name] = complex_type


        obj = {'abc' : 'C',
               'abc@Redfish.AllowableValues' : ['C']}

        self.assertEqual(ValidationStatus.PASSED, complex_type.validate(obj, ""))


    def test_property_has_incorrect_value_on_allowable_list(self):
        enum_type_doc = """
        <EnumType Name="ABCEnum">
            <Member Name="A">
            <Member Name="B"/>
            <Member Name="C"/>
        </EnumType>
        """

        complex_type_doc = """
        <ComplexType Name="complex_type">
            <Property Name="abc" type="namespace.ABCEnum" />
        </ComplexType>
        """

        enum_soup = BeautifulSoup(enum_type_doc, "lxml").find_all("enumtype")[0]
        complex_soup = BeautifulSoup(complex_type_doc, "lxml").find_all("complextype")[0]

        metadata_container = MetadataContainer()
        enum_type = EnumType(metadata_container, "namespace", enum_soup)
        complex_type = ComplexType(metadata_container, "namespace", complex_soup)

        metadata_container.types[enum_type.name] = enum_type
        metadata_container.types[complex_type.name] = complex_type


        obj = {'abc' : 'B',
               'abc@Redfish.AllowableValues' : ['A', 'B', 'X']} # X is not element of ABCEnum

        self.assertEqual(ValidationStatus.FAILED, complex_type.validate(obj, ""))



    def test_property_value_not_present_on_allowable_list(self):
        enum_type_doc = """
        <EnumType Name="ABCEnum">
            <Member Name="A">
            <Member Name="B"/>
            <Member Name="C"/>
        </EnumType>
        """

        complex_type_doc = """
        <ComplexType Name="complex_type">
            <Property Name="abc" type="namespace.ABCEnum" />
        </ComplexType>
        """

        enum_soup = BeautifulSoup(enum_type_doc, "lxml").find_all("enumtype")[0]
        complex_soup = BeautifulSoup(complex_type_doc, "lxml").find_all("complextype")[0]

        metadata_container = MetadataContainer()
        enum_type = EnumType(metadata_container, "namespace", enum_soup)
        complex_type = ComplexType(metadata_container, "namespace", complex_soup)

        metadata_container.types[enum_type.name] = enum_type
        metadata_container.types[complex_type.name] = complex_type


        obj = {'abc' : 'C', # C is not on allowable list
               'abc@Redfish.AllowableValues' : ['A', 'B']}

        self.assertEqual(ValidationStatus.PASSED_WITH_WARNINGS, complex_type.validate(obj, ""))


    def test_property_invalid_collection(self):
        METADATA = """
            <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="MyNamespace.v1_0_0">
              <EntityType Name="MyEntity">
                <Property name="property_name" type="Collection(MyComplexType)">
                </Property>
              </EntityType>
              <ComplexType Name="MyComplexType">
                <Property Name="Name" Nullable="false" Type="Edm.String"/>
              </ComplexType>
            </Schema>
        """

        discovery_container = DiscoveryContainer()

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)

        EntityJson = {
            "@odata.type": "MyNamespace.v1_0_0.MyEntity",
            "property_name" : {'a' : {'x' : 1}, 'b' : {'y' : 2}}
        }

        entity_type = EntityJson["@odata.type"]

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, metadata_container.entities[entity_type].validate(EntityJson, ''))

        self.assertIn('ERROR::Property ->property_name is expected to be collection, but it is not'
                      '', output.raw)

