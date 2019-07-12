import json
import unittest
import requests
from mock import mock, Mock, MagicMock

from cts_core.commons.links_factory import Link
from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.commons.api_caller import ApiCaller, AsyncOperation
from cts_framework.configuration.configuration import Configuration
from cts_framework.commons.enums import ReturnCodes
from unit_tests.helpers.stdout_capture import StdoutCapture

RESOURCE = "/resource"
API_ENDPOINT = "1.2.3.4:567"

class ApiCallerSSLUnitTest(unittest.TestCase):
    def test_ssl_on(self):
        configuration = Configuration(**dict(UseSSL='True', ApiEndpoint=API_ENDPOINT))
        url, kwargs = ApiCaller(configuration)._build_request(RESOURCE)
        self.assertEqual(url, Link("https://%s%s" % (API_ENDPOINT, RESOURCE), API_ENDPOINT))

    def test_ssl_off(self):
        configuration = Configuration(**dict(UseSSL='False', ApiEndpoint=API_ENDPOINT))
        url, kwargs = ApiCaller(configuration)._build_request(RESOURCE)
        self.assertEqual(url, Link("http://%s%s" % (API_ENDPOINT, RESOURCE), API_ENDPOINT))

    def test_ssl_default(self):
        configuration = Configuration(**dict(ApiEndpoint=API_ENDPOINT))
        url, kwargs = ApiCaller(configuration)._build_request(RESOURCE)
        self.assertEqual(url, Link("http://%s%s" % (API_ENDPOINT, RESOURCE), API_ENDPOINT))

CERT_FILE = "cert_file"
CERT_KEY = "cert_key"


class ApiCallerCertsUnitTest(unittest.TestCase):
    def test_certs_given(self):
        configuration = Configuration(**dict(CertificateCertFile=CERT_FILE, CertificateKeyFile=CERT_KEY,
                                                      ApiEndpoint=API_ENDPOINT))
        params, kwargs = ApiCaller(configuration)._build_request(RESOURCE)

        self.assertIn("cert", kwargs)
        self.assertEqual(kwargs["cert"], (CERT_FILE, CERT_KEY))

    def test_cert_given_key_not(self):
        configuration = Configuration(**dict(CertificateCertFile=CERT_FILE, ApiEndpoint=API_ENDPOINT))
        params, kwargs = ApiCaller(configuration)._build_request(RESOURCE)

        self.assertNotIn("cert", kwargs)

    def test_key_given_cert_not(self):
        configuration = Configuration(**dict(CertificateKeyFile=CERT_KEY, ApiEndpoint=API_ENDPOINT))
        params, kwargs = ApiCaller(configuration)._build_request(RESOURCE)

        self.assertNotIn("cert", kwargs)

    def test_certs_not_given(self):
        configuration = Configuration(**dict(ApiEndpoint=API_ENDPOINT))
        params, kwargs = ApiCaller(configuration)._build_request(RESOURCE)

        self.assertNotIn("cert", kwargs)


USER = "user"
PASSWORD = "password"


class ApiCallerBasicAuthUnitTest(unittest.TestCase):
    def test_basic_auth_given(self):
        configuration = Configuration(**dict(User=USER, Password=PASSWORD,
                                                      ApiEndpoint=API_ENDPOINT))
        params, kwargs = ApiCaller(configuration)._build_request(RESOURCE)

        self.assertIn("auth", kwargs)
        self.assertEqual(kwargs["auth"], (USER, PASSWORD))

    def test_user_given_password_not(self):
        configuration = Configuration(**dict(User=USER, ApiEndpoint=API_ENDPOINT))
        params, kwargs = ApiCaller(configuration)._build_request(RESOURCE)

        self.assertNotIn("auth", kwargs)

    def test_password_given_user_not(self):
        configuration = Configuration(**dict(Password=PASSWORD, ApiEndpoint=API_ENDPOINT))
        params, kwargs = ApiCaller(configuration)._build_request(RESOURCE)

        self.assertNotIn("auth", kwargs)

    def test_basic_auth_not_given(self):
        configuration = Configuration(**dict(ApiEndpoint=API_ENDPOINT))
        params, kwargs = ApiCaller(configuration)._build_request(RESOURCE)

        self.assertNotIn("auth", kwargs)


PAYLOAD = dict(a=1, b=2, c=[dict(), 1, "a"])


class ApiCallerPayloadUnitTest(unittest.TestCase):
    def setUp(self):
        configuration = Configuration(**dict(ApiEndpoint=API_ENDPOINT))
        self.api_caller = ApiCaller(configuration)

    def test_payload_given(self):
        params, kwargs = self.api_caller._build_request(RESOURCE, payload=PAYLOAD)
        self.assertIn("data", kwargs)
        self.assertEqual(kwargs["data"], json.dumps(PAYLOAD))

    def test_payload_not_given(self):
        params, kwargs = self.api_caller._build_request(RESOURCE)
        self.assertNotIn("data", kwargs)


class ApiCallerCallsUnitTest(unittest.TestCase):
    def setUp(self):
        configuration = Configuration(**dict(ApiEndpoint=API_ENDPOINT))
        self.api_caller = ApiCaller(configuration)

    @unittest.skip("Temporary change related to bug in Discovery mechanism")
    def test_get_on_empty_resource(self):
        with mock.patch('requests.get') as requests_get_mock:
            response = Mock()
            response.status_code = 200
            response.headers = {}
            response.text = ""
            requests_get_mock.return_value = response
            with StdoutCapture() as output:
                self.api_caller.get_resource("/resource", DiscoveryContainer())
            self.assertIn("ERROR::url=/resource Get failed. Status code: None;", output.raw)

    def test_empty_service_root(self):
        with mock.patch('requests.get') as requests_get_mock:
            requests_get_mock.side_effect = requests.ConnectionError()
            with StdoutCapture() as output:
                self.api_caller.get_resource("/redfish/v1", DiscoveryContainer())
            self.assertIn(
                "ERROR::Get url=http://1.2.3.4:567/redfish/v1 Error <class 'requests.exceptions.ConnectionError'>:;",
                output.raw)

    @unittest.skip("Temporary change related to bug in Discovery mechanism")
    def test_incorrect_status_code(self):
        with mock.patch('requests.get') as requests_get_mock:
            response = Mock()
            response.status_code = 500
            response.headers = {}
            response.text = "{}"
            requests_get_mock.return_value = response
            with StdoutCapture() as output:
                self.api_caller.get_resource("/resource", DiscoveryContainer())
            self.assertIn("ERROR::url=/resource Get failed. Status code: 500;", output.raw)

    def test_request_exception(self):
        with mock.patch('requests.get') as requests_get_mock:
            requests_get_mock.side_effect = requests.RequestException()
            with StdoutCapture() as output:
                self.api_caller.get_resource("/resource", DiscoveryContainer())
            self.assertIn("ERROR::Get url=http://1.2.3.4:567/resource Error <class 'requests.exceptions.RequestException'>:;", output.raw)

    def test_connection_error_exception(self):
        with mock.patch('requests.get') as requests_get_mock:
            requests_get_mock.side_effect = requests.ConnectionError()
            with StdoutCapture() as output:
                self.api_caller.get_resource("/resource", DiscoveryContainer())
            self.assertIn("ERROR::Get url=http://1.2.3.4:567/resource Error <class 'requests.exceptions.ConnectionError'>:;",
                            output.raw)

    def test_http_error_exception(self):
        with mock.patch('requests.get') as requests_get_mock:
            requests_get_mock.side_effect = requests.HTTPError()
            with StdoutCapture() as output:
                self.api_caller.get_resource("/resource", DiscoveryContainer())
            self.assertIn("ERROR::Get url=http://1.2.3.4:567/resource Error <class 'requests.exceptions.HTTPError'>:", output.raw)

    def test_url_required_exception(self):
        with mock.patch('requests.get') as requests_get_mock:
            requests_get_mock.side_effect = requests.URLRequired()
            with StdoutCapture() as output:
                self.api_caller.get_resource("/resource", DiscoveryContainer())
            self.assertIn("ERROR::Get url=http://1.2.3.4:567/resource Error <class "
                          "'requests.exceptions.URLRequired'>:",
                          output.raw)

    def test_to_many_redirects_exception(self):
        with mock.patch('requests.get') as requests_get_mock:
            requests_get_mock.side_effect = requests.TooManyRedirects()
            with StdoutCapture() as output:
                self.api_caller.get_resource("/resource", DiscoveryContainer())
            self.assertIn("ERROR::Get url=http://1.2.3.4:567/resource Error <class 'requests.exceptions.TooManyRedirects'>:;", output.raw)

    def test_timeout_exception(self):
        with mock.patch('requests.get') as requests_get_mock:
            requests_get_mock.side_effect = requests.Timeout()
            with StdoutCapture() as output:
                self.api_caller.get_resource("/resource", DiscoveryContainer())
            self.assertIn("ERROR::Get url=http://1.2.3.4:567/resource Error <class 'requests.exceptions.Timeout'>:;", output.raw)

    def test_incorrect_body(self):
        with mock.patch('requests.get') as requests_get_mock:
            response = Mock()
            response.status_code = 200
            response.headers = {}
            response.text = "not a json"
            requests_get_mock.return_value = response

            with StdoutCapture() as output:
                self.api_caller.get_resource("/resource", DiscoveryContainer())
            self.assertIn(
                "ERROR", output.raw)

    def test_parse_big_ordered_tree_response(self):
        from collections import OrderedDict
        response_payload = OrderedDict(
            [(u'@odata.context', u'/redfish/v1/$metadata#ServiceRoot.ServiceRoot'), (u'@odata.etag', u'W/"1557488360"'),
             (u'@odata.id', \
              u'/redfish/v1/'), (u'@odata.type', u'#ServiceRoot.v1_3_1.ServiceRoot'),
             (u'AccountService', OrderedDict([(u'@odata.id', u'/redfish/v1/AccountService')])),
             (u'Chassis', OrderedDict([(u'@odata.id', u'/redfish/v1/Chassis')])), (u'Description', u'The service root \
                                                                                   for all Redfish requests on this host \
             '), (u'EventService', OrderedDict([(u' @ odata.id', u' / redfish / v1 / EventService')])),
             (u'Id', u'RootService'), (
                 u'Links', OrderedDict(
                     [(u'Oem', OrderedDict([(u'Ami', OrderedDict([(u'@odata.id', u'/redfish/v1/configurations')]))])),
                      (u'Sessions', OrderedDict([(u'@odata.id', u'/redfish/v1/SessionService/Sessions')]))])), (
                 u'Managers', OrderedDict([(u'@odata.id',
                                            u'/redfish/v1/Managers')])), (u'Name', u'PSME Service Root'), (
                 u'Oem', OrderedDict([(u'Ami', OrderedDict([(u'Chassislocation',
                                                             OrderedDict(
                                                                 [(u'@odata.id', u'/redfish/v1/Chassislocation')])),
                                                            (u'Configurations', OrderedDict([(u'@odata.id',
                                                                                              u'/redfish/v1/configurations')])),
                                                            (u'PsmeVersion', u'2.4.181218.tb1')])),
                                      (u'Intel_RackScale', OrderedDict([(u'@odata.type',
                                                                         u'#Intel.Oem.ServiceRoot'),
                                                                        (u'ApiVersion', u'2.4.0'),
                                                                        (
                                                                        u'TelemetryService', OrderedDict([(u'@odata.id',
                                                                                                           u'/redfish/v1/TelemetryService')]))]))])),
             (u'Product', u'AMI Redfish Server'), (u'ProtocolFeaturesSupported',
                                                      OrderedDict([(u'ExpandQuery', OrderedDict(
                                                          [(u'ExpandAll', True), (u'Levels', True), (u'Links', True),
                                                           (u'MaxLevels', 5), (u'NoLinks',
                                                                               True)])), (u'FilterQuery', True),
                                                                   (u'SelectQuery', True)])), (
                 u'RedfishVersion', u'1.5.0'), (u'Registries', OrderedDict([(u'@odata.id',
                                                                             u'/redfish/v1/Registries')])), (
                 u'SessionService', OrderedDict([(u'@odata.id', u'/redfish/v1/SessionService')])), (u'Systems',
                                                                                                    OrderedDict([(
                                                                                                        u'@odata.id',
                                                                                                        u'/redfish/v1/Systems')])),
             (u'Tasks', OrderedDict([(u'@odata.id', u'/redfish/v1/TaskService')])), (u'UUID',
                                                                                        u'ffffffff-ffff-ffff-ffff-ffffffffffff'),
             (u'UpdateService', OrderedDict([(u'@odata.id', u'/redfish/v1/UpdateService')]))])

        with mock.patch('requests.get') as requests_get_mock:
            response = Mock()
            response.status_code = 200
            response.headers = {}
            response.text = response_payload
            requests_get_mock.return_value = response

            with StdoutCapture() as output:
                self.api_caller.get_resource("/resource", DiscoveryContainer())
            self.assertIsNot("ERROR", output.raw)

    def test_no_content(self):
        with mock.patch('requests.get') as requests_get_mock:
            response = Mock()
            response.status_code = 204
            response.headers = {}
            response.text = None
            requests_get_mock.return_value = response

            link, status, status_code, response_body, headers = \
                self.api_caller.get_resource("/resource", DiscoveryContainer(), acceptable_return_codes = [204])
            self.assertTrue(status)
            self.assertEqual(status_code, 204)
            self.assertEqual(response_body, dict())
            response.json.assert_not_called()

    def test_async_create_without_location(self):
        with mock.patch('requests.post') as requests_post_mock:
            response = Mock()
            response.status_code = ReturnCodes.ACCEPTED
            response.headers = {}
            response.text = None
            requests_post_mock.return_value = response

            with mock.patch('requests.get') as requests_get_mock:
                response = Mock()
                response.status_code = 204
                response.headers = {}
                response.text = None
                requests_get_mock.return_value = response

                with StdoutCapture() as output:
                    status, status_code, response_body, headers = self.api_caller.post_resource('odata_id', DiscoveryContainer(), payload={})

            self.assertIn('Location header not found', '\n'.join(output))

    def test_unexpected_async_post_response(self):
        with mock.patch('requests.post') as requests_get_mock:
            response = Mock()
            response.status_code = ReturnCodes.ACCEPTED
            response.headers = {}
            response.text = "{}"
            requests_get_mock.return_value = response

            with self.assertRaises(AsyncOperation):
                self.api_caller.post_resource('odata_id', DiscoveryContainer(), payload={}, wait_if_async=False)

    def test_async_post_response(self):
        with mock.patch('requests.post') as requests_post_mock:
            not_done = Mock()
            not_done.status_code = ReturnCodes.ACCEPTED
            not_done.headers = {'Location' : 'location'}
            not_done.text = None
            requests_post_mock.return_value = not_done

            with mock.patch('requests.get') as requests_get_mock:
                done = Mock()
                done.status_code = ReturnCodes.OK
                done.headers = {'Location' : 'location'}
                done.text = "{ \"done\": true }"

                requests_get_mock.side_effect = [not_done, not_done, done]

                with StdoutCapture() as output:
                    status, status_code, response_body, headers = self.api_caller.post_resource('odata_id',
                                                                                                DiscoveryContainer(),
                                                                                                payload={})
                    self.assertTrue(response_body['done'])

    @unittest.skip("Temporary change related to bug in Discovery mechanism")
    def test_resource_in_discovery_container_after_get_patch_delete(self):
        with mock.patch('requests.get') as requests_get_mock:
            resource = {"@odata.id": "odata.id", "something": "irrelevant"}
            get_response = Mock()
            get_response.status_code = ReturnCodes.OK
            get_response.headers = {}
            get_response.text = json.dumps(resource)
            requests_get_mock.return_value = get_response
            discovery_container = DiscoveryContainer()
            self.api_caller.get_resource("/resource", discovery_container)
            self.assertEqual(discovery_container["http://{API_ENDPOINT}/resource".format(
                API_ENDPOINT=API_ENDPOINT)].body,
                             resource)

            patched_resource = {"@odata.id": "odata.id", "something": "relevant"}
            get_response.text = json.dumps(patched_resource)

            with mock.patch('requests.patch') as requests_patch_mock:
                patch_response = Mock()
                patch_response.status_code = ReturnCodes.OK
                patch_response.headers = {}
                patch_response.text = "{}"
                requests_patch_mock.return_value = patch_response
                _, _, _, _ = self.api_caller.patch_resource("/resource", discovery_container)
                self.assertEqual(discovery_container["http://{API_ENDPOINT}/resource".format(
                API_ENDPOINT=API_ENDPOINT)].body, patched_resource)

                with mock.patch('requests.delete') as requests_delete_mock:
                    delete_response = Mock()
                    delete_response.status_code = ReturnCodes.NO_CONTENT
                    delete_response.headers = {}
                    delete_response.text = ""
                    requests_delete_mock.return_value = delete_response
                    _, _, _, _ = self.api_caller.delete_resource("/resource", discovery_container)
                    self.assertNotIn("/resource", discovery_container)

    def test_get_xml(self):
        with mock.patch('requests.get') as get:
            response = Mock()
            response.status_code = ReturnCodes.OK
            response.headers = {}
            response.text = "<xml></xml>"
            get.return_value = response

            link, status, status_code, response_body, headers = self.api_caller.get_xml("uri")
        self.assertEqual("<xml></xml>", response_body)
