import unittest

from cts_core.metadata.commons import Commons


class CommonsUnitTest(unittest.TestCase):
    def test_namespace_given(self):
        self.assertEqual(Commons.parse_type_name("a", "b"), "b.a")

    def test_namespace_none(self):
        self.assertEqual(Commons.parse_type_name("a", None), "a")

    def test_namespace_included(self):
        self.assertEqual(Commons.parse_type_name("b.a", "c"), "b.a")
