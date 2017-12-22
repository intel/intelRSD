import unittest

from mock import Mock

from cts_core.metadata.model.metadata_types.primitive_types.decimal import Decimal
from cts_core.validation.validation_status import ValidationStatus


class DecimalUnitTest(unittest.TestCase):
    def setUp(self):
        self.decimal = Decimal()

    def test_correct_values(self):
        self.assertEqual(self.decimal.validate(float(0), ""), ValidationStatus.PASSED)
        self.assertEqual(self.decimal.validate(float(1), ""), ValidationStatus.PASSED)
        self.assertEqual(self.decimal.validate(float(-1), ""), ValidationStatus.PASSED)

    def test_none_value(self):
        self.assertEqual(self.decimal.validate(None, ""), ValidationStatus.FAILED)

    def test_str(self):
        self.assertEqual(self.decimal.validate("", ""), ValidationStatus.FAILED)
        self.assertEqual(self.decimal.validate("example_value", ""), ValidationStatus.FAILED)

    def test_dict(self):
        self.assertEqual(self.decimal.validate(dict(), ""), ValidationStatus.FAILED)

    def test_list(self):
        self.assertEqual(self.decimal.validate(list(), ""), ValidationStatus.FAILED)

    def test_integer(self):
        self.assertEqual(self.decimal.validate(int(1), ""), ValidationStatus.PASSED)

    def test_minimum_annotation(self):
        annotations = {"Validation.Minimum": Mock(value=10)}
        self.assertEqual(self.decimal.validate(5.0, "", annotations), ValidationStatus.FAILED)
        self.assertEqual(self.decimal.validate(15.0, "", annotations), ValidationStatus.PASSED)

    def test_maximum_annotation(self):
        annotations = {"Validation.Maximum": Mock(value=10)}
        self.assertEqual(self.decimal.validate(5.0, "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.decimal.validate(15.0, "", annotations), ValidationStatus.FAILED)

    def test_mixed_annotation(self):
        annotations = {"Validation.Minimum": Mock(value=8), "Validation.Maximum": Mock(value=9)}
        self.assertEqual(self.decimal.validate(7.0, "", annotations), ValidationStatus.FAILED)
        self.assertEqual(self.decimal.validate(8.0, "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.decimal.validate(9.0, "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.decimal.validate(10.0, "", annotations), ValidationStatus.FAILED)
