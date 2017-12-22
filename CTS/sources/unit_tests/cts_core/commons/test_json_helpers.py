import unittest
from cts_core.commons.json_helpers import split_path_into_segments


class JsonHelpersUnitTest(unittest.TestCase):

    def test_segment_splitting(self):
        self.assertEqual(
            split_path_into_segments("////redfish/v1/EthernetSwitches/x/Ports/y/VLANs/z///",),
            ['redfish', 'v1', 'EthernetSwitches', 'x', 'Ports', 'y', 'VLANs', 'z'])
        self.assertEqual(
            split_path_into_segments("/////",),
            [])
        self.assertEqual(
            split_path_into_segments("redfish/v1/EthernetSwitches/x/Ports/y/VLANs/z",),
            ['redfish', 'v1', 'EthernetSwitches', 'x', 'Ports', 'y', 'VLANs', 'z'])

