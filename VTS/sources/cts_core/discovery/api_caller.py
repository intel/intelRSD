"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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

import urllib3
import requests
from distutils.util import strtobool

from simplejson.scanner import JSONDecodeError

from cts_core.commons.links_factory import LinksFactory
from cts_framework.commons.enums import RequestStatus, ReturnCodes, HttpMethods
from cts_framework.configuration.config_property_reader import ValueNotFound
from cts_framework.tests_helpers.test_case_flags import TestCaseFatalErrorFlag

HTTPS = "https://"
HTTP = "http://"


class ApiCaller:
    def __init__(self, config_property_reader):
        self._links_factory = LinksFactory(config_property_reader.ApiEndpoint)
        self._config_property_reader = config_property_reader

    def perform_call(self, odata_id, http_method=None, acceptable_return_codes=None, payload=None):
        if not acceptable_return_codes:
            acceptable_return_codes = [ReturnCodes.OK]

        if not http_method:
            http_method = HttpMethods.GET

        resource = self._links_factory.get_resource_link(odata_id)

        try:
            url, kwargs = self.build_request_params(resource, payload)
            requests_method = {HttpMethods.GET: requests.get, HttpMethods.PATCH: requests.patch}[http_method]

            print "DEBUG::\n%s %s\n\trequest parameters {\n\t\t%s\n\t}" % (http_method, url, "\n\t\t".join(
                ["%s: %s" % (str(key), str(value)) for key, value in kwargs.iteritems()])
                )

            try:
                # Try to suppress user warnings
                urllib3.disable_warnings()
            except:
                pass

            response = requests_method(url, **kwargs)
            if response.status_code in [ReturnCodes.UNAUTHORIZED, ReturnCodes.FORBIDDEN] and response.status_code not in acceptable_return_codes:
                raise TestCaseFatalErrorFlag("API refuses provided authentication")

            status = RequestStatus.SUCCESS if response.status_code in acceptable_return_codes else RequestStatus.FAILED

            status_code, headers = response.status_code, response.headers

            if response.status_code in [ReturnCodes.NO_CONTENT, ReturnCodes.CREATED]:
                response_body = dict()
            else:
                response_body = response.json()

            pretty_response = json.dumps(response_body, indent=4).replace("\n", "\n\t\t") if response.status_code != ReturnCodes.NO_CONTENT else "No content"

            print "RAW::\n\tresponse\n\t\t%s\n" % (pretty_response)
            print "RAW::\n\tSTATUS_CODE: %d, ACCEPTABLE: %s\n" % (response.status_code, str(acceptable_return_codes))
            if RequestStatus.SUCCESS != status:
                print "RAW::\n\t[WARNING] - %s %s\n" % (http_method, url)


        except requests.RequestException as err:
            raise TestCaseFatalErrorFlag("Error: %s occurred when accessing resource %s" % (err.message, odata_id))
        except JSONDecodeError:
            raise TestCaseFatalErrorFlag("Unable to parse resource %s to json" % odata_id)
        except Exception as err:
            raise TestCaseFatalErrorFlag("Unknown exception %s while accessing resource %s" % (err.message, odata_id))

        return status, status_code, response_body, headers

    def get_resource(self, odata_id):
        return self.perform_call(odata_id)

    def build_request_params(self, resource, payload):
        kwargs = dict(verify=False, headers={"Content-Type": "application/json"})
        try:
            kwargs["cert"] = (self._config_property_reader.CertificateCertFile,
                              self._config_property_reader.CertificateKeyFile)
        except ValueNotFound:
            #not using certificate authorization
            pass

        try:
            kwargs["auth"] = (self._config_property_reader.User, self._config_property_reader.Password)
        except ValueNotFound:
            pass

        try:
            if strtobool(self._config_property_reader.UseSSL):
                url = HTTPS + resource
            else:
                url = HTTP + resource
        except ValueNotFound:
            url = HTTP + resource

        if payload:
            kwargs["data"] = json.dumps(payload)

        return url, kwargs

    def read_certificate(self, certificate_file):
        try:
            with open(certificate_file, "r") as f:
                return f.read()
        except Exception:
            return None
