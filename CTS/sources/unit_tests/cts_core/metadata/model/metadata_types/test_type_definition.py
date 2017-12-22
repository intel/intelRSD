import unittest

from bs4 import BeautifulSoup
from mock import Mock

from cts_core.metadata.metadata_container import MetadataContainer
from cts_core.metadata.model.metadata_types.metadata_type_categories import MetadataTypeCategories
from cts_core.metadata.model.metadata_types.type_definition import TypeDefinition
from cts_core.validation.validation_status import ValidationStatus

TYPE_DEFINITION = "typedefinition"


class TypeDefinitionReadUnitTest(unittest.TestCase):
    def _get_type_definition_soup_from_type_definition_text(self, type_definition_text):
        return BeautifulSoup(type_definition_text, "lxml").find_all(TYPE_DEFINITION)[0]

    def test_empty_type_definition(self):
        namespace_name = "namespace_name"
        type_definition_name = "type_definition_name"
        base_type = "base_namespace.base_type"

        type_definition_doc = """
         <TypeDefinition name="{type_definition_name}" UnderlyingType="{base_type}" />
        """.format(type_definition_name=type_definition_name, base_type=base_type)

        type_definition_soup = self._get_type_definition_soup_from_type_definition_text(type_definition_doc)
        type_definition = TypeDefinition(MetadataContainer(), namespace_name, type_definition_soup)

        self.assertEqual(type_definition.name, ".".join([namespace_name, type_definition_name]))
        self.assertFalse(type_definition.annotations)
        self.assertEqual(type_definition.base_type, base_type)
        self.assertEqual(type_definition.type_category, MetadataTypeCategories.TYPE_DEFINITION)

    def test_annotation_given(self):
        base_type = "base_namespace.base_type"
        namespace_name = "namespace_name"
        type_definition_name = "type_definition_name"

        type_definition_doc = """
         <TypeDefinition name="{type_definition_name}" UnderlyingType="{base_type}">
          <Annotation />
         </TypeDefinition>
        """.format(type_definition_name=type_definition_name, base_type=base_type)

        type_definition_soup = self._get_type_definition_soup_from_type_definition_text(type_definition_doc)
        type_definition = TypeDefinition(MetadataContainer(), namespace_name, type_definition_soup)

        self.assertEqual(len(type_definition.annotations), 1)


class TypeDefinitionValidationUnitTest(unittest.TestCase):
    TYPE_DEFINITION_NAME_EXISTING = "type.definition_name"
    TYPE_DEFINITION_NAME_NOT_EXISTING = "type.definition_name2"
    UNDERLYING_TYPE = "underlying.type"

    def setUp(self):
        self.metadata_container = MetadataContainer()
        type_definition_doc = """
         <TypeDefinition name="{type_definition_name}" UnderlyingType="{base_type}" />
        """.format(type_definition_name=TypeDefinitionValidationUnitTest.TYPE_DEFINITION_NAME_EXISTING,
                   base_type=TypeDefinitionValidationUnitTest.UNDERLYING_TYPE)

        self.metadata_container.types[TypeDefinitionValidationUnitTest.TYPE_DEFINITION_NAME_EXISTING] = TypeDefinition(
            self.metadata_container, "type", BeautifulSoup(type_definition_doc, "lxml").find_all(TYPE_DEFINITION)[0])

        type_definition_doc = """
                 <TypeDefinition name="{type_definition_name}" UnderlyingType="not_existing.type" />
                """.format(type_definition_name=TypeDefinitionValidationUnitTest.TYPE_DEFINITION_NAME_EXISTING,
                           base_type=TypeDefinitionValidationUnitTest.UNDERLYING_TYPE)

        self.metadata_container.types[TypeDefinitionValidationUnitTest.TYPE_DEFINITION_NAME_NOT_EXISTING] = TypeDefinition(
            self.metadata_container, "type", BeautifulSoup(type_definition_doc, "lxml").find_all(TYPE_DEFINITION)[0])

    def test_underlying_type_returns_passed(self):
        self.metadata_container.types[TypeDefinitionValidationUnitTest.UNDERLYING_TYPE] = Mock()
        self.metadata_container.types[TypeDefinitionValidationUnitTest.UNDERLYING_TYPE].validate = Mock(
            return_value=ValidationStatus.PASSED)

        self.assertEqual(
            self.metadata_container.types[TypeDefinitionValidationUnitTest.TYPE_DEFINITION_NAME_EXISTING].validate("", ""),
            ValidationStatus.PASSED)

    def test_underlying_type_returns_failed(self):
        self.metadata_container.types[TypeDefinitionValidationUnitTest.UNDERLYING_TYPE] = Mock()
        self.metadata_container.types[TypeDefinitionValidationUnitTest.UNDERLYING_TYPE].validate = Mock(
            return_value=ValidationStatus.FAILED)

        self.assertEqual(
            self.metadata_container.types[TypeDefinitionValidationUnitTest.TYPE_DEFINITION_NAME_EXISTING].validate("", ""),
            ValidationStatus.FAILED)

    def test_underlying_type_not_existing(self):
        self.assertEqual(
            self.metadata_container.types[TypeDefinitionValidationUnitTest.TYPE_DEFINITION_NAME_NOT_EXISTING].validate("", ""),
            ValidationStatus.FAILED)
