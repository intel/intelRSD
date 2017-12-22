import unittest

from bs4 import BeautifulSoup
from mock import MagicMock, Mock
from cts_core.metadata.metadata_container import MetadataContainer
from cts_core.metadata.model.metadata_types.complex_type import ComplexType
from cts_core.metadata.model.metadata_types.metadata_type_categories import MetadataTypeCategories
from cts_core.validation.validation_status import ValidationStatus



COMPLEX_TYPE = "complextype"


class ComplexTypeReadUnitTest(unittest.TestCase):
    def _get_complex_type_from_complex_type_text(self, complex_type_text):
        return BeautifulSoup(complex_type_text, "lxml").find_all(COMPLEX_TYPE)[0]

    def test_empty_complex_name(self):
        namespace_name = "namespace_name"
        complex_type_name = "complex_type_name"
        complex_type_doc = """
        <ComplexType Name="{complex_type_name}" />
        """.format(complex_type_name=complex_type_name)

        complex_type_soup = self._get_complex_type_from_complex_type_text(complex_type_doc)

        complex_type = ComplexType(MetadataContainer(), namespace_name, complex_type_soup)
        self.assertEqual(complex_type.name, ".".join([namespace_name, complex_type_name]))
        self.assertTrue(complex_type.allow_additional_properties)
        self.assertEqual(complex_type.type_category, MetadataTypeCategories.COMPLEX_TYPE)
        self.assertIsNone(complex_type.base_type)
        self.assertFalse(complex_type.annotations)
        self.assertFalse(complex_type.properties)
        self.assertFalse(complex_type.navigation_properties)

    def test_complex_name_properties(self):
        complex_type_doc = """
        <ComplexType Name="complex_type_name">
            <Property Name="property_name" type="property_type" />
        </ComplexType>
        """

        complex_type_soup = self._get_complex_type_from_complex_type_text(complex_type_doc)

        complex_type = ComplexType(MetadataContainer(), "", complex_type_soup)
        self.assertFalse(complex_type.navigation_properties)
        self.assertEqual(len(complex_type.properties), 1)

    def test_complex_name_navigation_property(self):
        complex_type_doc = """
        <ComplexType Name="complex_type_name">
            <NavigationProperty Name="property_name" type="property_type" />
        </ComplexType>
        """

        complex_type_doc = self._get_complex_type_from_complex_type_text(complex_type_doc)

        complex_type = ComplexType(MetadataContainer(), "", complex_type_doc)
        self.assertFalse(complex_type.properties)
        self.assertEqual(len(complex_type.navigation_properties), 1)

    def test_complex_name_allow_additional_properties(self):
        complex_type_doc = """
        <ComplexType Name="complex_name_name">
            <Annotation Term="OData.AdditionalProperties" Bool="true"/>
        </ComplexType>
        """

        complex_type_soup = self._get_complex_type_from_complex_type_text(complex_type_doc)

        complex_type = ComplexType(MetadataContainer(), "", complex_type_soup)
        self.assertTrue(complex_type.allow_additional_properties)

    def test_base_type(self):
        base_type = "base_type.base_type"
        complex_type_doc = """
        <ComplexType Name="complex_name_name" BaseType="{base_type}"/>
        """.format(base_type=base_type)

        complex_type_soup = self._get_complex_type_from_complex_type_text(complex_type_doc)

        complex_type = ComplexType(MetadataContainer(), "", complex_type_soup)
        self.assertEqual(complex_type.base_type, base_type)


class ComplexTypeValidationUnitTest(unittest.TestCase):
    PROPERTY_TYPE = "base.property_name"
    NAVIGATION_PROPERTY_TYPE = "base.navigation_property_type"
    BASE_COMPLEX_TYPE = "base.complex_type"
    DERIVED_COMPLEX_TYPE = "derived.complex_type"

    def setUp(self):
        base_complex_type_doc = """
        <ComplexType Name="{base_complex_type}">
            <Annotation Term="OData.AdditionalProperties" Bool="false"/>
            <Property Name="property" type="{property_type}" />
            <NavigationProperty Name="navigation_property" type="{navigation_property_type}" />
            <Property Name="RequiredProperty" Nullable="false" Type="Edm.String">
              <Annotation Term="OData.Permissions" EnumMember="OData.Permissions/Read"/>
              <Annotation Term="OData.Description" String="Required property"/>
              <Annotation Term="OData.LongDescription" String="This property is required"/>
              <Annotation Term="Redfish.Required"/>
            </Property>
        </ComplexType>
        """.format(base_complex_type=ComplexTypeValidationUnitTest.BASE_COMPLEX_TYPE,
                   property_type=ComplexTypeValidationUnitTest.PROPERTY_TYPE,
                   navigation_property_type=ComplexTypeValidationUnitTest.NAVIGATION_PROPERTY_TYPE)

        derived_complex_type_doc = """
        <ComplexType Name="{derived_complex_type}" BaseType="{base_complex_type}">
            <Annotation Term="OData.AdditionalProperties" Bool="true"/>
        </ComplexType>
        """.format(derived_complex_type=ComplexTypeValidationUnitTest.DERIVED_COMPLEX_TYPE,
                   base_complex_type=ComplexTypeValidationUnitTest.BASE_COMPLEX_TYPE)

        base_complex_type_soup = BeautifulSoup(base_complex_type_doc, "lxml").find_all(COMPLEX_TYPE)[0]
        derived_complex_type_soup = BeautifulSoup(derived_complex_type_doc, "lxml").find_all(COMPLEX_TYPE)[0]

        self.metadata_container = MetadataContainer()
        base_complex_type = ComplexType(self.metadata_container, "base", base_complex_type_soup)
        derived_complex_type = ComplexType(self.metadata_container, "derived", derived_complex_type_soup)

        self.metadata_container.types[derived_complex_type.name] = derived_complex_type
        self.metadata_container.types[base_complex_type.name] = base_complex_type
        self.metadata_container.types[ComplexTypeValidationUnitTest.PROPERTY_TYPE] = MagicMock()
        self.metadata_container.types[ComplexTypeValidationUnitTest.PROPERTY_TYPE].validate = Mock(
            return_value=ValidationStatus.PASSED)

    def test_correctly_missing_navigation_property(self):
        self.assertEqual(
            self.metadata_container.types[ComplexTypeValidationUnitTest.BASE_COMPLEX_TYPE].validate(dict(property="value", RequiredProperty="value"), ""),
            ValidationStatus.PASSED)

    def test_correctly_missing_property(self):
        self.assertEqual(self.metadata_container.types[ComplexTypeValidationUnitTest.BASE_COMPLEX_TYPE].validate(
            dict(navigation_property="value", RequiredProperty="value"), "", None), ValidationStatus.PASSED)

    def test_incorrect_additional_property(self):
        self.assertEqual(
            self.metadata_container.types[ComplexTypeValidationUnitTest.BASE_COMPLEX_TYPE].validate(dict(add_prop="value"), ""),
            ValidationStatus.FAILED)

    def test_correct_additional_property(self):
        self.assertEqual(
            self.metadata_container.types[ComplexTypeValidationUnitTest.DERIVED_COMPLEX_TYPE].validate(
                dict(add_prop="value", RequiredProperty="value"), ""),
                ValidationStatus.PASSED)

    def test_derived_complex_type(self):
        self.assertEqual(self.metadata_container.types[ComplexTypeValidationUnitTest.BASE_COMPLEX_TYPE].validate(
            {"@odata.type": "#" + ComplexTypeValidationUnitTest.DERIVED_COMPLEX_TYPE,
             "additional_property": "will be marked as passed only if validated as derived type",
             "RequiredProperty" : "Value"}, ""),
            ValidationStatus.PASSED)

    def test_inconsistent_odata_types(self):
        self.assertEqual(self.metadata_container.types[ComplexTypeValidationUnitTest.BASE_COMPLEX_TYPE].validate(
            {"@odata.type": "incosistent_odata_type"}, "", None), ValidationStatus.FAILED)

    def test_incorrect_when_required_property_missing(self):
        self.assertEqual(
            self.metadata_container.types[ComplexTypeValidationUnitTest.DERIVED_COMPLEX_TYPE].validate(
                dict(add_prop="value"), ""),
                ValidationStatus.FAILED)
