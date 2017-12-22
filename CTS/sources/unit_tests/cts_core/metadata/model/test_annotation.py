import unittest

from bs4 import BeautifulSoup

from cts_core.metadata.model.annotation import Annotation
ANNOTATION = "annotation"


class AnnotationReadUnitTest(unittest.TestCase):
    def _get_annotation_soup_from_annotation_text(self, annotation_text):
        return BeautifulSoup(annotation_text, "lxml").find_all(ANNOTATION)[0]

    def test_empty_annotation(self):
        annotation_doc = """
         <Annotation/>
        """
        annotation_soup = self._get_annotation_soup_from_annotation_text(annotation_doc)
        annotation = Annotation(annotation_soup)

        self.assertIsNone(annotation.term)
        self.assertIsNone(annotation.value)
        self.assertIsNone(annotation.value_type)

    def test_term_given(self):
        term = "term"
        annotation_doc = """
         <Annotation term="{term}" />
        """.format(term=term)

        annotation_soup = self._get_annotation_soup_from_annotation_text(annotation_doc)
        annotation = Annotation(annotation_soup)

        self.assertEqual(annotation.term, term)
        self.assertIsNone(annotation.value)
        self.assertIsNone(annotation.value_type)

    def test_value_given(self):
        value = "value"
        value_type = "value_type"
        annotation_doc = """
         <Annotation {value_type}="{value}" />
        """.format(value=value, value_type=value_type)

        annotation_soup = self._get_annotation_soup_from_annotation_text(annotation_doc)
        annotation = Annotation(annotation_soup)

        self.assertIsNone(annotation.term)
        self.assertEqual(annotation.value, value)
        self.assertEqual(annotation.value_type, value_type)

    def test_all_values_given(self):
        term = "term"
        value = "value"
        value_type = "value_type"
        annotation_doc = """
              <Annotation term="{term}" {value_type}="{value}" />
             """.format(term=term, value=value, value_type=value_type)

        annotation_soup = self._get_annotation_soup_from_annotation_text(annotation_doc)
        annotation = Annotation(annotation_soup)

        self.assertEqual(annotation.term, term)
        self.assertEqual(annotation.value, value)
        self.assertEqual(annotation.value_type, value_type)
