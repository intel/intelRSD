import unittest

from cts_core.commons.links_factory import LinksFactory

ENDPOINT = "my-address:1234"

# All relative URIs used by the service shall start with a double forward slash ("//") and
# include the authority (e.g., //mgmt.vendor.com/redfish/v1/Systems) or a single forward
# slash ("/") and include the absolute-path (e.g., /redfish/v1/Systems).
# https://www.dmtf.org/sites/default/files/standards/documents/DSP0266_1.2.0.pdf
RELATIVE_WITH_ABSOLUTE_PATH_ODATA_ID = "/redfish/v1/EthernetSwitches/1-es-1/Ports/1-es-1-pt-7"
RELATIVE_ODATA_ID_WITH_AUTHORITY = "//bmcip/redfish/v1/Managers"
RELATIVE_ODATA_ID_WITH_AUTHORITY_WITH_PORT = "//bmcip:1234/redfish/v1/Managers"
ABSOLUTE_ODATA_ID = "http://my-funny-endpoint.com/sun/is/the/fun"
ABSOLUTE_ODATA_ID_WITH_TRAILING_SLASH = "http://my-funny-endpoint.com/sun/is/the/fun/"
OVERRIDE = "override:1234"


class LinksFactoryUnitTest(unittest.TestCase):
    def setUp(self):
        self.links_factory = LinksFactory(ENDPOINT, ssl=True)

    def test_relative_with_absolute_path(self):
        self.assertEqual("https://{enpoint}{odata_id}".format(enpoint=ENDPOINT,
                                                              odata_id=RELATIVE_WITH_ABSOLUTE_PATH_ODATA_ID),
                         self.links_factory.get_resource_link(
                             RELATIVE_WITH_ABSOLUTE_PATH_ODATA_ID).link)

    def test_relative(self):
        self.assertEqual("https://" + RELATIVE_ODATA_ID_WITH_AUTHORITY[2:],
                         self.links_factory.get_resource_link(
                             RELATIVE_ODATA_ID_WITH_AUTHORITY).link)
        self.assertEqual("https://" + RELATIVE_ODATA_ID_WITH_AUTHORITY_WITH_PORT[2:],
                         self.links_factory.get_resource_link(
                             RELATIVE_ODATA_ID_WITH_AUTHORITY_WITH_PORT).link)

    def test_absolute(self):
        self.assertEqual(ABSOLUTE_ODATA_ID, self.links_factory.get_resource_link(
            ABSOLUTE_ODATA_ID).link)
        self.assertNotEqual(ABSOLUTE_ODATA_ID_WITH_TRAILING_SLASH[:-1],
                            self.links_factory.get_resource_link(
                                ABSOLUTE_ODATA_ID_WITH_TRAILING_SLASH).link)

    def test_with_override(self):
        self.assertEqual("https://{enpoint}{odata_id}".format(enpoint=OVERRIDE,
                                                              odata_id=RELATIVE_WITH_ABSOLUTE_PATH_ODATA_ID),
                         self.links_factory.get_resource_link(
                             RELATIVE_WITH_ABSOLUTE_PATH_ODATA_ID,
                             api_endpoint_override=OVERRIDE).link)

        self.assertEqual("https://" + RELATIVE_ODATA_ID_WITH_AUTHORITY[2:],
                         self.links_factory.get_resource_link(
                             RELATIVE_ODATA_ID_WITH_AUTHORITY,
                             api_endpoint_override=OVERRIDE).link)
        self.assertEqual("https://" + RELATIVE_ODATA_ID_WITH_AUTHORITY_WITH_PORT[2:],
                         self.links_factory.get_resource_link(
                             RELATIVE_ODATA_ID_WITH_AUTHORITY_WITH_PORT,
                             api_endpoint_override=OVERRIDE).link)

        self.assertEqual(ABSOLUTE_ODATA_ID, self.links_factory.get_resource_link(
            ABSOLUTE_ODATA_ID,
            api_endpoint_override=OVERRIDE).link)
        self.assertNotEqual(ABSOLUTE_ODATA_ID_WITH_TRAILING_SLASH[:-1],
                            self.links_factory.get_resource_link(
                                ABSOLUTE_ODATA_ID_WITH_TRAILING_SLASH,
                                api_endpoint_override=OVERRIDE).link)
