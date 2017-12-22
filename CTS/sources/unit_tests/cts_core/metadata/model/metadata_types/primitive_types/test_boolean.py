import unittest

from cts_core.metadata.model.metadata_types.primitive_types.boolean import Boolean
from cts_core.validation.validation_status import ValidationStatus


class BooleanUnitTest(unittest.TestCase):
    def setUp(self):
        self.boolean = Boolean()

    def test_correct_values(self):
        self.assertEqual(self.boolean.validate(True, ""), ValidationStatus.PASSED)
        self.assertEqual(self.boolean.validate(False, ""), ValidationStatus.PASSED)

    def test_none_value(self):
        self.assertEqual(self.boolean.validate(None, ""), ValidationStatus.FAILED)

    def test_int(self):
        self.assertEqual(self.boolean.validate(1, ""), ValidationStatus.FAILED)
        self.assertEqual(self.boolean.validate(0, ""), ValidationStatus.FAILED)

    def test_str(self):
        self.assertEqual(self.boolean.validate("", ""), ValidationStatus.FAILED)
        self.assertEqual(self.boolean.validate("example_value", ""), ValidationStatus.FAILED)

    def test_dict(self):
        self.assertEqual(self.boolean.validate(dict(), ""), ValidationStatus.FAILED)

    def test_list(self):
        self.assertEqual(self.boolean.validate(list(), ""), ValidationStatus.FAILED)
