import unittest
import re
from cts_core.commons.error import cts_error
from unit_tests.helpers.stdout_capture import StdoutCapture


class CtsErrorUnitTest(unittest.TestCase):
    def test_empty(self):
        with StdoutCapture() as output:
            cts_error("")
        self.assertIsNotNone(re.search("\[#Id=[0-9a-f]{32}\]", output.raw))

    def test_without_additional_arguments(self):
        with StdoutCapture() as output:
            cts_error("kotprezesa")
        self.assertIsNotNone(re.search("kotprezesa.*\[#Id=[0-9a-f]{32}\]", output.raw))

    def test_with_regular_formatting_fields(self):
        with StdoutCapture() as output:
            cts_error('kot{kogo}wazy{ile}', kogo='prezesa', ile='sporo')
        self.assertIsNotNone(re.search("kotprezesawazysporo.*\[#Id=[0-9a-f]{32}\]", output.raw))

        with StdoutCapture() as output:
            cts_error('kot{kogo}wazy{ile}kg', kogo='prezesa', ile=20)
        self.assertIsNotNone(re.search("kotprezesawazy20kg.*\[#Id=[0-9a-f]{32}\]", output.raw))

    def test_cts_error_survives_missing_argument_error(self):
        with StdoutCapture() as output:
            cts_error('a={a}, b={b}, c={c}', a='this is a', B='should be small capital b', c='this is c')
        self.assertIsNotNone(re.search("ERROR::a=this is a, b=\[UNKNOWN\], c=this is c;.*\[#Id=[0-9a-f]{32}\]", output.raw))

    def test_odata_id_field_not_modified_in_text_message(self):
        with StdoutCapture() as output:
            cts_error('{odata_id:id}', odata_id='/redfish/v1/abra/33-th/kadabra/2')
        self.assertIsNotNone(re.search("url=/redfish/v1/abra/33-th/kadabra/2.*\[#Id=[0-9a-f]{"
                                       "32}\]", output.raw))

    def test_identical_error_id_for_different_stacktraces(self):
        with StdoutCapture() as output:
            cts_error('{stacktrace:stacktrace}', stacktrace='one')
        match = re.search("\[#Id=(?P<id>[0-9a-f]{32})\]", output.raw)
        self.assertIsNotNone(match)
        id_1 = match.group('id')

        with StdoutCapture() as output:
            cts_error('{stacktrace:stacktrace}', stacktrace='two')
        match = re.search("\[#Id=(?P<id>[0-9a-f]{32})\]", output.raw)
        self.assertIsNotNone(match)
        id_2 = match.group('id')

        self.assertEqual(id_1, id_2)
