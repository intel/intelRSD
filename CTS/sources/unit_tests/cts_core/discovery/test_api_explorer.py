import re
import unittest

from mock import MagicMock, Mock, ANY, mock

from cts_core.commons.links_factory import Link
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.discovery.api_resource import ApiResource
from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.metadata.metadata_manager import MetadataManager
from cts_framework.commons.enums import RequestStatus, ReturnCodes
from cts_framework.configuration.configuration import Configuration
from unit_tests.helpers.stdout_capture import StdoutCapture

API_ENDPOINT = "1.2.3.4:567"


@unittest.skip("Temporary change related to bug in Discovery mechanism")
class ApiExplorerUnitTest(unittest.TestCase):
    def setUp(self):
        config_property_parser = Mock()
        config_property_parser.ApiEndpoint = ""
        config_property_parser.UseSSL = "Yes"
        self.api_explorer = ApiExplorer(None, config_property_parser)

    def test_visited_endpoint(self):
        discovery_container = DiscoveryContainer()
        discovery_container.add_resource(ApiResource("http://ip:123/something", None, {}, None))
        discovery_container.add_resource = MagicMock()
        returned_discovery_container, _ = self.api_explorer.discover("http://ip:123/something",
                                                                         None, discovery_container)
        discovery_container.add_resource.assert_not_called()

    def test_empty_body(self):
        self.api_explorer._get_resource = MagicMock(return_value=(
            Link('link', 'netloc'),
            RequestStatus.SUCCESS,
            {},
            None))

        with StdoutCapture() as output:
            self.api_explorer.discover("odata_id", None)

        self.assertIn('Empty response body', '\n'.join(output))

    def test_correct_body_processing(self):
        discovery_container = DiscoveryContainer()
        self.api_explorer._metadata_container = MagicMock()
        self.api_explorer._get_resource = MagicMock(return_value=(Link('link', 'netloc'),
                                                                  RequestStatus.SUCCESS,
                                                                  dict(a=1),
                                                                  None))
        self.api_explorer._process_resource = MagicMock(return_value=discovery_container)

        self.api_explorer.discover("url", None)
        self.api_explorer._process_resource.assert_called_once_with(ANY)

    def test_get_resource(self):
        self.api_explorer._api_caller.get_resource = MagicMock(
            return_value=(
                Link('link', 'netloc'),
                RequestStatus.SUCCESS, None, {
                '@odata.type': 'type.of.b',
                'inner': [
                    {'@odata.type': 'type.of.first.inner'},
                    {'@odata.type': 'type.of.second.inner'}
                ]
            }, None))
        link, status, body, _ = self.api_explorer._get_resource("/a/b#inner/0")
        self.assertEqual(RequestStatus.SUCCESS, status)
        self.assertEqual({'@odata.type': 'type.of.first.inner'}, body)

    def test_get_resource_with_odata(self):
        self.api_explorer._api_caller.get_resource = MagicMock(
            return_value=(
                Link('link', 'netloc'),
                RequestStatus.SUCCESS, None, {
                '@odata.type': 'type.of.b',
                'inner': [
                    {'@odata.id': '/redfish/a/b#inner/3'},
                    {'@odata.id': '/redfish/a/b#inner/4'}
                ]
            }, None))
        link, status, body, _ = self.api_explorer._get_resource("/redfish/a/b#inner/3")
        self.assertEqual(RequestStatus.SUCCESS, status)
        self.assertEqual({'@odata.id': '/redfish/a/b#inner/3'}, body)

    def test_get_resource_with_odata_failed(self):
        self.api_explorer._api_caller.get_resource = MagicMock(
            return_value=(
                Link('link', 'netloc'),
                RequestStatus.SUCCESS, None, {
                '@odata.type': 'type.of.b',
                'inner': [
                    {'@odata.id': '/redfish/a/b#inner/3'},
                    {'@odata.id': '/redfish/a/b#inner/4'}
                ]
            }, None))
        with StdoutCapture() as output:
            link, status, body, _ = self.api_explorer._get_resource("/redfish/a/b#inner/543534634")
            self.assertIsNot(RequestStatus.SUCCESS, status)
        self.assertIn(
            "ERROR::Could not find /inner/543534634 from url=/redfish/a/b#inner/543534634 with search by @odata.id;",
            output.raw)

    def test_discovery_from_additional_properties(self):
        metadata = """
            <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="N">
                <EntityType Name="Outer" Abstract="false">
                    <Annotation Term="OData.AdditionalProperties" Bool="true"/>
                </EntityType>

                <ComplexType Name="Inner" Abstract="false">
                    <NavigationProperty Name="Link" Type="N.Referenced" ContainsTarget="true">
                    </NavigationProperty>
                </ComplexType>

                <EntityType Name="Referenced" Abstract="false">
                </EntityType>
            </Schema>
            """

        resource = \
            {
                "@odata.id": "resource.id",
                "@odata.type": "#N.Outer",
                "additional_property": {
                    "@odata.type": "#N.Inner",
                    "Link": {
                        "@odata.id": "/this/is/the/link/to/be/discovered"
                    },
                }
            }

        referenced = \
            {
                "@odata.id": "referenced.id",
                "@odata.type": "#N.Referenced",
            }

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings("Unknown", metadata)
        discovery_container = DiscoveryContainer()
        configuration = Configuration(
            **dict(UseSSL='True', ApiEndpoint=API_ENDPOINT))
        api_explorer = ApiExplorer(metadata_container, configuration)

        with mock.patch(
                'cts_core.discovery.api_explorer.ApiExplorer._get_resource') as get_resource:
            get_resource.side_effect = [(Link('link', 'netloc'), RequestStatus.SUCCESS, resource, None),
                                        (Link('link', 'netloc'), RequestStatus.SUCCESS, referenced, None)]

            with StdoutCapture() as output:
                api_explorer.discover("/outer", "N.Outer", discovery_container)
            self.assertEqual(2, get_resource.call_count)
            self.assertNotIn("ERROR::", '\n'.join(output))

    def test_discovery_error_if_unexpected_additional_properties(self):
        """
        Entity type used in this test does not accept additional properties. As a result, ApiExplorer is expected
        to raise an error about unexpected 'additional_property' property.
        ApiExplorer validate
        """
        metadata = """
            <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="N">
                <EntityType Name="Outer" Abstract="false">
                    <Property Name="@odata.id" Type="Edm.String"/>
                    <Property Name="@odata.type" Type="Edm.String"/>
                    <Property Name="expected" Type="Edm.Int64"/>
                    <Annotation Term="OData.AdditionalProperties" Bool="false"/>
                </EntityType>
            </Schema>
            """

        resource = \
            {
                "@odata.id": "id",
                "@odata.type": "N.Outer",
                "expected": 10,
                "additional_property": 1
            }

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings("Unknown", metadata)
        discovery_container = DiscoveryContainer()
        configuration = Configuration(
            **dict(UseSSL='True', ApiEndpoint=API_ENDPOINT))
        api_explorer = ApiExplorer(metadata_container, configuration)

        with mock.patch(
                'cts_core.discovery.api_explorer.ApiExplorer._get_resource') as get_resource:
            get_resource.side_effect = [(Link('link', 'netloc'), RequestStatus.SUCCESS, resource, None)]

            with StdoutCapture() as output:
                api_explorer.discover("/outer", "N.Outer", discovery_container)
            self.assertEqual(1, get_resource.call_count)
            self.assertIn("unexpected properties: [additional_property]", '\n'.join(output))

    def test_discovery_from_nested_additional_properties(self):
        metadata = """
            <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="N">
                <EntityType Name="Outer" Abstract="false">
                    <Annotation Term="OData.AdditionalProperties" Bool="true"/>
                    <Property Name="Oem" Type="N.Oem">
                    </Property>
                </EntityType>

                <ComplexType Name="Oem" Abstract="false">
                    <Annotation Term="OData.AdditionalProperties" Bool="true"/>
                </ComplexType>

                <ComplexType Name="Inner" Abstract="false">
                    <NavigationProperty Name="Link" Type="N.Referenced" ContainsTarget="true">
                    </NavigationProperty>
                </ComplexType>

                <EntityType Name="Referenced" Abstract="false">
                </EntityType>
            </Schema>
            """

        resource = \
            {
                "@odata.id": "resource.id",
                "@odata.type": "#N.Outer",
                "Oem": {
                    "additional_property": {
                        "@odata.type": "#N.Inner",
                        "Link": {
                            "@odata.id": "/this/is/the/link/to/be/discovered"
                        },
                    }
                }
            }

        referenced = \
            {
                "@odata.id": "referenced.id",
                "@odata.type": "#N.Referenced",
            }

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings("Unknown", metadata)
        discovery_container = DiscoveryContainer()
        configuration = Configuration(
            **dict(UseSSL='True', ApiEndpoint=API_ENDPOINT))
        api_explorer = ApiExplorer(metadata_container, configuration)

        with mock.patch(
                'cts_core.discovery.api_explorer.ApiExplorer._get_resource') as get_resource:
            get_resource.side_effect = [(Link('link', 'netloc'), RequestStatus.SUCCESS, resource, None),
                                        (Link('link', 'netloc'), RequestStatus.SUCCESS, referenced, None)]

            with StdoutCapture() as output:
                api_explorer.discover("/outer", "N.Outer", discovery_container)
            self.assertEqual(2, get_resource.call_count)
            self.assertNotIn("ERROR::", '\n'.join(output))

    @unittest.skip("Temporary change related to bug in Discovery mechanism")
    def test_discovery_can_handle_null_navigation_property(self):
        metadata = """
            <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="N">
                <EntityType Name="Outer" Abstract="false">
                    <NavigationProperty Name="Link" Type="N.Referenced" ContainsTarget="true">
                    </NavigationProperty>
                </EntityType>

                <EntityType Name="Referenced" Abstract="false">
                </EntityType>
            </Schema>
            """

        resource = \
            {
                "@odata.id": "resource.id",
                "@odata.type": "#N.Outer",
                "Link": {
                    "@odata.id": None
                },
            }

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings("Unknown", metadata)
        discovery_container = DiscoveryContainer()
        configuration = Configuration(
            **dict(UseSSL='True', ApiEndpoint=API_ENDPOINT))
        api_explorer = ApiExplorer(metadata_container, configuration)

        with mock.patch(
                'cts_core.discovery.api_explorer.ApiExplorer._get_resource') as get_resource:
            get_resource.side_effect = [(Link('link', 'netloc'), RequestStatus.SUCCESS, resource, None)]

            with StdoutCapture() as output:
                api_explorer.discover("/outer", "N.Outer", discovery_container)
            self.assertEqual(1, get_resource.call_count)
            self.assertNotIn("ERROR::", '\n'.join(output))

    def test_handle_entities_with_dynamic_property_patterns(self):
        metadata = """
                    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="N">
                        <EntityType Name="Outer" Abstract="false">
                            <Annotation Term="OData.AdditionalProperties" Bool="true"/>
                            <Annotation Term="Redfish.DynamicPropertyPatterns">
                                <Collection>
                                    <Record>
                                        <PropertyValue Property="Pattern" String="[a-z0-9]+"/>
                                        <PropertyValue Property="Type" String="N.Referenced"/>
                                    </Record>
                                    <Record>
                                        <PropertyValue Property="Pattern" String="[A-Z0-9]+"/>
                                        <PropertyValue Property="Type" String="N.Referenced"/>
                                    </Record>
                                </Collection>
                            </Annotation>
                        </EntityType>

                        <ComplexType Name="Referenced" Abstract="false">
                            <Property Name="name" Type="Edm.String"/>
                        </ComplexType>
                    </Schema>
                    """

        resource = \
            {
                "@odata.id": "resource.id",
                "@odata.type": "N.Outer",
                "additional1": {
                    "name": "additional#1"
                },
                "additional2": {
                    "name": "additional#2"
                },
                "ADDITIONAL3": "ADDITIONAL3",
                "ADDITIONAL4": "ADDITIONAL4",
                "SiMpLeTyPeIsOk": 22,
                "CoMpLeXInVaLiD": {"x": 1}

            }

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings("Unknown", metadata)
        discovery_container = DiscoveryContainer()
        configuration = Configuration(
            **dict(UseSSL='True', ApiEndpoint=API_ENDPOINT))
        api_explorer = ApiExplorer(metadata_container, configuration)

        with mock.patch(
                'cts_core.discovery.api_explorer.ApiExplorer._get_resource') as get_resource:
            get_resource.side_effect = [
                (Link('https://{API_ENDPOINT}/outer'.format(
                    API_ENDPOINT=API_ENDPOINT), 'netloc'),
                 RequestStatus.SUCCESS, resource, None)]

            with StdoutCapture() as output:
                api_explorer.discover("/outer", "N.Outer", discovery_container)
            self.assertEqual(1, get_resource.call_count)
            self.assertEqual(1, len(re.findall('ERROR::', ';'.join(output))))
            self.assertIn(
                'ERROR::url=https://1.2.3.4:567/outer#CoMpLeXInVaLiD: @odata.type not found in '
                'complex additional property;',
                output.raw)

    def test_handle_complex_types_with_dynamic_property_patterns(self):
        metadata = """
                            <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="N">
                                <EntityType Name="Outer" Abstract="false">
                                    <Property Name="inner" Type="N.TypeWithDynamics"/>
                                </EntityType>

                                <ComplexType Name="TypeWithDynamics" Abstract="false">
                                    <Annotation Term="OData.AdditionalProperties" Bool="true"/>
                                    <Annotation Term="Redfish.DynamicPropertyPatterns">
                                        <Collection>
                                            <Record>
                                                <PropertyValue Property="Pattern" String="[a-z0-9]+"/>
                                                <PropertyValue Property="Type" String="N.Referenced"/>
                                            </Record>
                                            <Record>
                                                <PropertyValue Property="Pattern" String="[A-Z0-9]+"/>
                                                <PropertyValue Property="Type" String="Edm.String"/>
                                            </Record>
                                        </Collection>
                                    </Annotation>
                                </ComplexType>

                                <ComplexType Name="Referenced" Abstract="false">
                                    <Property Name="name" Type="Edm.String"/>
                                </ComplexType>
                            </Schema>
                            """

        resource = \
            {
                "@odata.id": "resource.id",
                "@odata.type": "N.Outer",
                "inner": {
                    "additional1": {
                        "name": "additional#1"
                    },
                    "additional2": {
                        "name": "additional#2"
                    },
                    "ADDITIONAL3": "ADDITIONAL3",
                    "ADDITIONAL4": "ADDITIONAL4",
                    "SiMpLeTyPeIsOk": 22,
                    "CoMpLeXInVaLiD": {"x": 1}
                }
            }

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings("Unknown", metadata)
        discovery_container = DiscoveryContainer()
        configuration = Configuration(
            **dict(UseSSL='True', ApiEndpoint=API_ENDPOINT))
        api_explorer = ApiExplorer(metadata_container, configuration)

        with mock.patch(
                'cts_core.discovery.api_explorer.ApiExplorer._get_resource') as get_resource:
            get_resource.side_effect = [(Link('https://{API_ENDPOINT}/outer'.format(
                API_ENDPOINT=API_ENDPOINT), 'netloc'), RequestStatus.SUCCESS, resource, None)]

            with StdoutCapture() as output:
                api_explorer.discover("/outer", "N.Outer", discovery_container)
            self.assertEqual(1, get_resource.call_count)
            self.assertEqual(1, len(re.findall('ERROR::', ';'.join(output))))
            self.assertIn(
                'ERROR::url=https://1.2.3.4:567/outer#inner.CoMpLeXInVaLiD: @odata.type not found in complex additional property;',
                ';'.join(output))

    def test_handle_collection_with_typeless_elements_referenced_by_json_pointer(self):
        metadata = """
                    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="N">
                        <EntityType Name="Outer" Abstract="false">
                            <NavigationProperty Name="MyCollection" Type="Collection(ReferencedEntity)" ContainsTarget="true">
                                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                            </NavigationProperty>
                        </EntityType>

                        <EntityType Name="ReferencedEntity" Abstract="false">
                            <Property Name="name" Type="Edm.String"/>
                        </EntityType>
                    </Schema>
                    """

        resource = \
            {
                "@odata.id": "resource.id",
                "@odata.type": "N.Outer",
                "MyCollection": [
                    {
                        # implicit odata.type = ReferencedEntity
                        "@odata.id": "/outer#MyCollection/0",
                        "name": "MyName 0"
                    },
                    {
                        # implicit odata_type = ReferencedEntity
                        "@odata.id": "/outer#MyCollection/1",
                        "name": "MyName 1"
                    }
                ]
            }

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings("Unknown", metadata)
        discovery_container = DiscoveryContainer()
        configuration = Configuration(
            **dict(UseSSL='True', ApiEndpoint=API_ENDPOINT))
        api_explorer = ApiExplorer(metadata_container, configuration)

        with mock.patch('cts_core.commons.api_caller.ApiCaller.get_resource') as get_resource:
            get_resource.side_effect = [(Link('link', 'netloc'), RequestStatus.SUCCESS,
                                         ReturnCodes.OK, resource, None),
                                        (Link('link', 'netloc'), RequestStatus.SUCCESS,
                                         ReturnCodes.OK, resource, None),
                                        (Link('link', 'netloc'), RequestStatus.SUCCESS,
                                         ReturnCodes.OK, resource, None)]

            with StdoutCapture() as output:
                api_explorer.discover("/outer", "N.Outer", discovery_container)
            self.assertEqual(3, get_resource.call_count)
            self.assertEqual(0, len(re.findall('ERROR::', ';'.join(output))))
