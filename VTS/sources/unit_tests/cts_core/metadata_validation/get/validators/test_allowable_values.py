import unittest
from bs4 import BeautifulSoup
from cts_core.metadata.model.metadata_types import EnumType, ComplexType
from cts_core.metadata.metadata_containers import MetadataContainer
from cts_core.metadata_validation.get.validators.type_validator import ComplexTypeValidator

class AllowableValuesTest(unittest.TestCase):

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
        enum_type = EnumType("namespace", enum_soup)
        complex_type = ComplexType("namespace", complex_soup)

        metadata_container.types[enum_type.name] = enum_type
        metadata_container.types[complex_type.name] = complex_type


        obj = { '@odata.type' : 'namespace.complex_type',
                'abc' : 'C',
               'abc@Redfish.AllowableValues' : ['C']}

        validator = ComplexTypeValidator(None, metadata_container)
        self.assertTrue(validator.validate('namespace.complex_type', obj))


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
        enum_type = EnumType("namespace", enum_soup)
        complex_type = ComplexType("namespace", complex_soup)

        metadata_container.types[enum_type.name] = enum_type
        metadata_container.types[complex_type.name] = complex_type

        obj = {'abc': 'B',
               'abc@Redfish.AllowableValues': ['A', 'B', 'X']}  # X is not element of ABCEnum

        validator = ComplexTypeValidator(None, metadata_container)
        self.assertFalse(validator.validate('namespace.complex_type', obj))

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
        enum_type = EnumType("namespace", enum_soup)
        complex_type = ComplexType("namespace", complex_soup)

        metadata_container.types[enum_type.name] = enum_type
        metadata_container.types[complex_type.name] = complex_type

        obj = {'abc': 'C',  # C is not on allowable list
               'abc@Redfish.AllowableValues': ['A', 'B']}

        validator = ComplexTypeValidator(None, metadata_container)
        # used to be assertFalse, but there was a decision to change error to warning
        self.assertTrue(validator.validate('namespace.complex_type', obj))