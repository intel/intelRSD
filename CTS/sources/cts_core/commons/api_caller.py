"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

import json
import pickle
import time
import re
import requests
import socket
import OpenSSL
import sys
from distutils.util import strtobool
from os import getenv, getppid
from traceback import format_exc
from collections import OrderedDict
from simplejson.scanner import JSONDecodeError
from cts_core.commons.error import cts_error, cts_warning, cts_message
from cts_core.commons.links_factory import LinksFactory
from cts_core.commons.replay_controller import ReplayController
from cts_core.discovery.api_resource import ApiResource
from cts_framework.commons.enums import RequestStatus, ReturnCodes, HttpMethods, LoggingLevel, HTTPServerErrors
from cts_framework.configuration.configuration import ValueNotFound
from cts_framework.helpers.vars.pickle_mock_consts import MockConstants
from cts_framework.db.dao.http_request_dao import HttpRequestDAO
from cts_framework.db.dao.script_dao import ScriptDAO

TASK_TIMEOUT = 200


class AsyncOperation(Exception):
    pass


class ApiCaller:
    DEFAULT_RETURN_CODES = {
        HttpMethods.PATCH: [ReturnCodes.NO_CONTENT, ReturnCodes.ACCEPTED, ReturnCodes.OK],
        HttpMethods.POST: [ReturnCodes.CREATED, ReturnCodes.ACCEPTED],
        HttpMethods.DELETE: [ReturnCodes.NO_CONTENT, ReturnCodes.ACCEPTED],
        HttpMethods.GET: [ReturnCodes.OK]}

    FORMAT_JSON = "application/json"
    FORMAT_XML = "application/xml"

    def __init__(self, configuration):
        try:
            ssl = strtobool(configuration.UseSSL)
        except ValueNotFound:
            ssl = False

        self._links_factory = LinksFactory(configuration.ApiEndpoint, ssl=ssl)
        self._config_property_reader = configuration
        try:
            self.script_execution_id = ScriptDAO.get_last_script_execution_id()
            if self.script_execution_id != None:
                script_execution = ScriptDAO.get_script_execution_details(self.script_execution_id)
                if getppid() != script_execution.pid:
                    # this is probably replay or running test script without framework
                    # do not register requests
                    self.script_execution_id = None
        except:
            self.script_execution_id = None

        self.request_registration = self.script_execution_id is not None

    @property
    def links_factory(self):
        return self._links_factory

    @property
    def _is_controlled_by_framework(self):
        """
        Used to determine if test that executes api caller is controlled by CTS framework
        or has been executed as independent python script. This is used to determine if request/response should
        be written to database.

        :rtype: bool
        """
        return self.script_execution_id is not None

    def _perform_call(self, url, http_method=None, acceptable_return_codes=None,
                      payload=None, api_endpoint_override=None, format=None):
        """
        Helper method that executes http request and returns result. Internally it may talk to remote API or
        retrieve recorded data from database.
        :rtype: (Link, RequestStatus, int, json|String, dict)
        :type http_method: str
        :type acceptable_return_codes: list
        :type payload: object
        """
        if url is None:
            return None, RequestStatus.FAILED, None, None, None

        if format is None:
            format = ApiCaller.FORMAT_JSON

        if not http_method:
            http_method = HttpMethods.GET
        if not acceptable_return_codes:
            acceptable_return_codes = ApiCaller.DEFAULT_RETURN_CODES[http_method]

        try:
            link, kwargs = self._build_request(url, payload,
                                               api_endpoint_override=api_endpoint_override,
                                               format=format)
            url = link.link

            print "MESSAGE::-%s %s" % (http_method, url)
            if not self._is_controlled_by_framework:
                self._log_request(kwargs)

            response, status_code = self._do_request(kwargs, url, http_method)

            if response is None:
                return None, RequestStatus.FAILED, status_code, None, None

            self._register_request(http_method, kwargs, response, url)

            status = RequestStatus.SUCCESS if response.status_code in acceptable_return_codes else RequestStatus.FAILED
            status_code, headers = response.status_code, response.headers

            if status_code in range(500, 600):
                custom_response_text = HTTPServerErrors.ERROR.get(status_code, "Unknown server error: %s" % status_code)
                return None, RequestStatus.FAILED, status_code, custom_response_text, None

            if format == ApiCaller.FORMAT_JSON:
                response_body = self._decode_json(url, response)
                if response_body is None:
                    return None, RequestStatus.FAILED, None, None, None
            else:
                response_body = response.text

        except Exception as err:
            cts_error(
                "Unknown exception '{err:exception}' on {http_method} resource {url:id} : {"
                "stack:stacktrace}",
                stack=format_exc(), **locals())
            return None, RequestStatus.FAILED, None, None, None

        return link, status, status_code, response_body, headers

    def _decode_json(self, url, response):
        if response.text:
            if response.status_code in [ReturnCodes.NO_CONTENT]:
                cts_error(
                    "{method} {url:id} Non-empty response despite NO_CONTENT {code} return "
                    "code",
                    method=response.request.method, url=url,
                    code=ReturnCodes.NO_CONTENT)
            try:
                if isinstance(response.text, OrderedDict):
                    response_without_any_ordered_dict_collection = json.dumps(response.text)
                else:
                    response_without_any_ordered_dict_collection = response.text
                response_body = json.loads(response_without_any_ordered_dict_collection,
                                           object_pairs_hook=OrderedDict)
            except (JSONDecodeError, ValueError) as err:
                method = response.request.method if response.request else ''
                cts_error("{method} {url:id} Unable to parse. Error {err:exception}", method=method, url=url, err=err)
                return None
        else:
            if response.status_code in [ReturnCodes.NO_CONTENT, ReturnCodes.CREATED,
                                        ReturnCodes.ACCEPTED]:
                response_body = dict()
            else:
                cts_error("{url:id} Unexpected empty response", url=url)
                response_body = None

        if not self._is_controlled_by_framework:
            self._log_response(response, response_body)

        return response_body

    def _register_request(self, http_method, kwargs, response, url):
        # register request/response in the database
        if self._is_controlled_by_framework:
            # Remove stuff to make the serializers work:
            lkwargs = kwargs.copy()
            del lkwargs['hooks']
            response.request = None

            request_id = HttpRequestDAO.register_request(
                self.script_execution_id, http_method, url,
                json.dumps(lkwargs), pickle.dumps(response),
                response.status_code)
            print "%s::request_id=%d" % (LoggingLevel.CONTROL, request_id)

    def _do_request(self, kwargs, url, http_method):
        response = None
        if ReplayController.replay_mode_on():
            response = ReplayController.request(http_method, url, **kwargs)

        if response is None:
            requests_method = {HttpMethods.GET: requests.get,
                               HttpMethods.PATCH: requests.patch,
                               HttpMethods.POST: requests.post,
                               HttpMethods.DELETE: requests.delete}[http_method]

            try:
                self._suppress_urllib3_error()
                response = requests_method(url, **kwargs)
            except OpenSSL.SSL.Error as ose:
                cts_error("There is a problem with CertFile or KeyFile: {err}",
                          err=' '.join(ose.message[0]))
                return None, ReturnCodes.INVALID_CERTS
            except requests.HTTPError as err:
                cts_error("{method} {url:id} Error {err:exception}",
                          method=http_method, url=url, err=err)
                return None, ReturnCodes.METHOD_NOT_ALLOWED
            except requests.TooManyRedirects as err:
                cts_error("{method} {url:id} Error {err:exception}",
                          method=http_method, url=url, err=err)
                return None, ReturnCodes.INVALID_FORWARDING
            except requests.Timeout as err:
                cts_error("{method} {url:id} Error {err:exception}",
                          method=http_method, url=url, err=err)
                return None, ReturnCodes.TIMEOUT
            except requests.ConnectionError as err:
                cts_error("{method} {url:id} Error {err:exception}",
                          method=http_method, url=url, err=err)
                return None, ReturnCodes.NOT_FOUND
            except requests.RequestException as err:
                cts_error("{method} {url:id} Error {err:exception}",
                          method=http_method, url=url, err=err)
                return None, ReturnCodes.BAD_REQUEST
        return response, response.status_code

    @staticmethod
    def _suppress_urllib3_error():
        try:
            # Try to suppress user warnings
            from requests.packages.urllib3.exceptions import InsecureRequestWarning
            from requests.packages.urllib3 import disable_warnings
            disable_warnings(category=InsecureRequestWarning)
        except:
            pass

    @staticmethod
    def _log_request(kwargs):
        cts_message(" request parameters:")
        for key, value in kwargs.iteritems():
            cts_message("     {key}:{value}", key=str(key), value=str(value))

    @staticmethod
    def _log_response(response, response_body):
        pretty_response = json.dumps(response_body, indent=4) \
            if response.status_code != ReturnCodes.NO_CONTENT else "No content"
        cts_message("status code: %d" % response.status_code)
        cts_message("response:")
        for r in pretty_response.split('\n'):
            cts_message("{line}\n", line=r)

    @staticmethod
    def _skip_refresh_after_request(url):
        # we can not do GET on Actions/* without additional errors in logs
        return True if re.search(r"(redfish\/v1\/Nodes\/|Systems\/+)(.*Actions\/)", url) else False

    def get_xml(self, uri):
        return self._perform_call(uri, format=ApiCaller.FORMAT_XML)

    def get_resource(self, url, discovery_container, acceptable_return_codes=None,
                     api_endpoint_override=None, check_resource_against_metadata=None):
        """
        Sends http GET request to remote endpoint and retrieves resource odata_id.

        :type url: str
        :type discovery_container: DiscoveryContainer
        :type acceptable_return_codes: list(int)

        :rtype link
        :rtype status
        :rtype status_code
        :rtype response_body
        :rtype headers
        """

        link, status, status_code, response_body, headers = \
            self._perform_call(url,
                               acceptable_return_codes=acceptable_return_codes,
                               api_endpoint_override=api_endpoint_override)
        if not discovery_container:
            return link, status, status_code, response_body, headers

        if status == RequestStatus.SUCCESS:
            if response_body and status_code != ReturnCodes.NOT_FOUND:
                status = discovery_container.add_resource(
                    ApiResource(link.link,
                                link.netloc,
                                response_body,
                                discovery_container.get_expected_odata_type_for_url(link.link)),
                                    check_resource_against_metadata=check_resource_against_metadata)
        else:
            cts_error("{url:id} Get failed. Status code: {code}", url=url, code=status_code)

        return link, status, status_code, response_body, headers

    def post_resource(self, url, discovery_container, payload=None, acceptable_return_codes=None,
                      wait_if_async=True, expect_location=None, api_endpoint_override=None):
        """
        Sends http POST request to remote endpoint.
        Throws AsyncOperation if service created task in response to this request.

        :type url: str
        :type discovery_container: DiscoveryContainer
        :type payload: dict
        :type acceptable_return_codes: list(int)
        :type wait_if_async: bool
        """
        from cts_core.discovery.api_explorer import ApiExplorer

        if expect_location is None:
            expect_location = True

        _, status, status_code, response_body, headers = \
            self._perform_call(url,
                               http_method=HttpMethods.POST,
                               payload=payload,
                               acceptable_return_codes=acceptable_return_codes,
                               api_endpoint_override=api_endpoint_override)
        # determine if the operation was completed synchronously or asynchronously
        if status_code == ReturnCodes.ACCEPTED:
            if not wait_if_async:
                raise AsyncOperation()
            status, status_code, response_body, headers = self._wait_for_task(headers,
                                                                              discovery_container,
                                                                              acceptable_ret_codes=acceptable_return_codes)

        if expect_location and status and not self._skip_refresh_after_request(url):
            try:
                # add the created element and discover all its children
                new_url = self.links_factory.get_resource_link(headers["Location"],
                                                               api_endpoint_override=api_endpoint_override).link
                print "MESSAGE::Refreshing {} info".format(new_url)
                # refresh the collection info
                _, get_status, get_status_code, _, _ = self.get_resource(url, discovery_container,
                                                                         api_endpoint_override=api_endpoint_override,
                                                                         check_resource_against_metadata=True)
                _, rediscovery_status = ApiExplorer(discovery_container.metadata_container,
                                                    self._config_property_reader).discover(new_url,
                                                                                           discovery_container.get_expected_odata_type_for_url(new_url),
                                                                                           discovery_container)
                if get_status and rediscovery_status:
                    print "MESSAGE::Refreshed %s and its children info" % new_url
                else:
                    cts_warning("Refreshing {odata_id:id} after POST generated errors. Get status code: {code}",
                                odata_id=new_url, code=get_status_code)

            except (KeyError, TypeError) as err:
                cts_warning(
                    "POST {odata_id:id} Response has no 'Location' header; Error: {err:exception}",
                    odata_id=url, err=err)

        return status, status_code, response_body, headers

    def patch_resource(self, url, discovery_container, payload=None,
                       acceptable_return_codes=None, wait_if_async=True, api_endpoint_override=None):
        """
        Sends http PATCH request to remote endpoint.
        Throws AsyncOperation if service created task in response to this request.

        :type url: str
        :type discovery_container: DiscoveryContainer
        :type payload: dict
        :type acceptable_return_codes: list(int)
        :type wait_if_async: bool
        """

        _, patch_status, patch_status_code, response_body, headers = \
            self._perform_call(url,
                               http_method=HttpMethods.PATCH,
                               payload=payload,
                               acceptable_return_codes=acceptable_return_codes,
                               api_endpoint_override=api_endpoint_override)

        # determine if the operation was completed synchronously or asynchronously
        if patch_status_code == ReturnCodes.ACCEPTED:
            if not wait_if_async:
                raise AsyncOperation()
            patch_status, patch_status_code, response_body, headers = self._wait_for_task(headers,
                                                                                          discovery_container,
                                                                                          acceptable_ret_codes=acceptable_return_codes)

        if patch_status != RequestStatus.SUCCESS:
            cts_error("{url:id} Patch failed. Status code: {code}", url=url, code=patch_status_code)

        print "MESSAGE::Refreshing {} info".format(url)

        _, get_status, get_status_code, _, _ = self.get_resource(url, discovery_container,
                                                                 api_endpoint_override=api_endpoint_override,
                                                                 check_resource_against_metadata=True)
        if not get_status:
            cts_warning("Refreshing {url:id} after PATCH generated errors. Get status code: {code}",
                        url=url, code=get_status_code)

        return patch_status, patch_status_code, response_body, headers

    def delete_resource(self, url, discovery_container, acceptable_return_codes=None,
                        wait_if_async=True, api_endpoint_override=None):
        """
        Sends http DELETE request to remote endpoint.
        Throws AsyncOperation if service created task in response to this request.

        :type url: str
        :type discovery_container: DiscoveryContainer
        :type acceptable_return_codes: list(int)
        :type wait_if_async: bool
        """
        link, status, status_code, response_body, headers = \
            self._perform_call(url,
                               http_method=HttpMethods.DELETE,
                               acceptable_return_codes=acceptable_return_codes,
                               api_endpoint_override=api_endpoint_override)

        # determine if the operation was completed synchronously or asynchronously
        if status_code == ReturnCodes.ACCEPTED:
            if not wait_if_async:
                raise AsyncOperation()
            status, status_code, response_body, headers = self._wait_for_task(headers,
                                                                              discovery_container,
                                                                              acceptable_ret_codes=acceptable_return_codes)

        # In UNPICKLE mode, CTS can't find a parent resource, so return mocked success
        if getenv('CTS_UNPICKLE', None):
            return MockConstants.delete_resource()

        if status == RequestStatus.SUCCESS:
            if discovery_container[link.link].parent_url:
                self.get_resource(discovery_container[link.link].parent_url,
                                  discovery_container,
                                  api_endpoint_override=api_endpoint_override)
            else:
                cts_warning("Unable to update the parent of {url:id} - parent unknown",
                            url=link.link)
            discovery_container.delete_resource(link.link)
        else:
            # not deleted. refresh
            self.get_resource(url, discovery_container,
                              api_endpoint_override=api_endpoint_override)

        return status, status_code, response_body, headers

    def _wait_for_task(self, headers, discovery_container, api_endpoint_override=None, acceptable_ret_codes=None):
        if acceptable_ret_codes is None:
            acceptable_ret_codes = [ReturnCodes.OK,
                                    ReturnCodes.CREATED,
                                    ReturnCodes.NO_CONTENT
                                    ]
        task_monitor = headers.get('Location')
        print "MESSAGE::Waiting for task completion. Monitor = %s" % task_monitor
        if task_monitor:
            for _ in range(0, TASK_TIMEOUT):
                _, status, status_code, task, headers = \
                    self.get_resource(task_monitor,
                                      discovery_container,
                                      acceptable_ret_codes +
                                      [
                                          ReturnCodes.ACCEPTED
                                      ],
                                      api_endpoint_override)

                # The client may also cancel the operation by performing a DELETE on the Task resource. Deleting the
                # Task resource object may invalidate the associated Task Monitor and subsequent GET on the Task
                # Monitor URL returns either 410 (Gone) or 404 (Not Found)
                if status_code in [ReturnCodes.NOT_FOUND, ReturnCodes.GONE]:
                    cts_warning("Task was cancelled unexpectedly")
                    return RequestStatus.FAILED, None, None, None

                # Once the operation has completed, the Task Monitor shall return a status code of OK (200) or
                # CREATED (201) for POST and include the headers and response body of the initial operation, as if it
                #  had completed synchronously
                if status_code in acceptable_ret_codes:
                    return status, status_code, task, headers

                # As long as the operation is in process, the service shall continue to return a status code of
                # 202 (Accepted) when querying the Task Monitor returned in the location header
                if status_code not in [ReturnCodes.ACCEPTED]:
                    cts_error("{odata_id:id} Task monitor returned unexpected status code: {code}",
                              odata_id=task_monitor, code=status_code)
                    return RequestStatus.FAILED, None, None, None

                print "MESSAGE::Task in progress. Waiting for completion"
                time.sleep(1)
        else:
            cts_error("{odata_id:id} Task has been created but Location header not found",
                      odata_id=task_monitor)
            return RequestStatus.FAILED, None, None, None

    def _build_request(self, url, payload=None, api_endpoint_override=None, format=None):
        if format is None:
            format = ApiCaller.FORMAT_JSON

        link = self._links_factory.get_resource_link(url,
                                                     api_endpoint_override=api_endpoint_override)

        kwargs = {'verify': False,
                  'headers': {"Content-Type": format,
                              "Accept": format},
                  'hooks': {'response': self._save_address_hook},
                  }
        try:
            kwargs["cert"] = (self._config_property_reader.CertificateCertFile,
                              self._config_property_reader.CertificateKeyFile)
        except ValueNotFound:
            # not using certificate authorization
            pass
        try:
            kwargs["auth"] = (
                self._config_property_reader.User, self._config_property_reader.Password)
        except ValueNotFound:
            pass

        if payload:
            kwargs["data"] = json.dumps(payload)

        return link, kwargs

    @staticmethod
    def read_certificate(certificate_file):
        try:
            with open(certificate_file, "r") as f:
                return f.read()
        except IOError:
            pass

        return None

    def _save_address_hook(self, r, *args, **kwargs):
        s = socket.fromfd(r.raw.fileno(), socket.AF_INET, socket.SOCK_STREAM)
        self.remote_address = s.getpeername()
        self.local_address = s.getsockname()
