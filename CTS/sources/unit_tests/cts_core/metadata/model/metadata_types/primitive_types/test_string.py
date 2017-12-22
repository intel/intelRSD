import unittest

from mock import Mock

from cts_core.metadata.model.metadata_types.primitive_types.string import String
from cts_core.validation.validation_status import ValidationStatus


class StringUnitTest(unittest.TestCase):
    def setUp(self):
        self.string = String()

    def test_correct_value(self):
        self.assertEqual(self.string.validate("", ""), ValidationStatus.PASSED)
        self.assertEqual(self.string.validate("example_string", ""), ValidationStatus.PASSED)

    def test_none_value(self):
        self.assertEqual(self.string.validate(None, ""), ValidationStatus.FAILED)

    def test_dict(self):
        self.assertEqual(self.string.validate(dict(), ""), ValidationStatus.FAILED)

    def test_list(self):
        self.assertEqual(self.string.validate(list(), ""), ValidationStatus.FAILED)

    def test_numbers(self):
        self.assertEqual(self.string.validate(1, ""), ValidationStatus.FAILED)

    def test_annotations(self):
        annotations = {"Validation.Pattern": Mock(value="example_string")}

        self.assertEqual(self.string.validate("example_string", "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.string.validate("example#string", "", annotations), ValidationStatus.FAILED)

    def test_generate_value_from_regex(self):
        annotations = {"Validation.Pattern": Mock(value="0[xX](([a-fA-F]|[0-9]){2}){2}")}

        values = self.string.generate_values(annotations)
        self.assertGreaterEqual(len(values), 1)
        for value in values:
            self.assertEqual(self.string.validate(value, "", annotations), ValidationStatus.PASSED)
