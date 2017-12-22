import unittest

from mock import Mock

from cts_core.metadata.model.metadata_types.primitive_types.double import Double
from cts_core.validation.validation_status import ValidationStatus


class DoubleUnitTest(unittest.TestCase):
    def setUp(self):
        self.double = Double()

    def test_correct_values(self):
        self.assertEqual(self.double.validate(float(0), ""), ValidationStatus.PASSED)
        self.assertEqual(self.double.validate(float(1), ""), ValidationStatus.PASSED)
        self.assertEqual(self.double.validate(float(-1), ""), ValidationStatus.PASSED)

    def test_none_value(self):
        self.assertEqual(self.double.validate(None, ""), ValidationStatus.FAILED)

    def test_str(self):
        self.assertEqual(self.double.validate("", ""), ValidationStatus.FAILED)
        self.assertEqual(self.double.validate("example_value", ""), ValidationStatus.FAILED)

    def test_dict(self):
        self.assertEqual(self.double.validate(dict(), ""), ValidationStatus.FAILED)

    def test_list(self):
        self.assertEqual(self.double.validate(list(), ""), ValidationStatus.FAILED)

    def test_integer(self):
        self.assertEqual(self.double.validate(int(1), ""), ValidationStatus.PASSED)

    def test_minimum_annotation(self):
        annotations = {"Validation.Minimum": Mock(value=10)}
        self.assertEqual(self.double.validate(5.0, "", annotations), ValidationStatus.FAILED)
        self.assertEqual(self.double.validate(15.0, "", annotations), ValidationStatus.PASSED)

    def test_maximum_annotation(self):
        annotations = {"Validation.Maximum": Mock(value=10)}
        self.assertEqual(self.double.validate(5.0, "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.double.validate(15.0, "", annotations), ValidationStatus.FAILED)

    def test_mixed_annotation(self):
        annotations = {"Validation.Minimum": Mock(value=8), "Validation.Maximum": Mock(value=9)}
        self.assertEqual(self.double.validate(7.0, "", annotations), ValidationStatus.FAILED)
        self.assertEqual(self.double.validate(8.0, "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.double.validate(9.0, "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.double.validate(10.0, "", annotations), ValidationStatus.FAILED)
