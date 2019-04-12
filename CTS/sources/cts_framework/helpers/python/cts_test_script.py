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
import sys
from os import getenv

from cts_core.commons.error import cts_error, cts_warning
from cts_core.commons.replay_controller import ReplayController
from cts_core.commons.services import ServiceTypes
from cts_core.metadata.metadata_manager import MetadataManager
from cts_framework.commons.digest import DirDigest
from cts_framework.helpers.python.test_script import TestScript
from cts_framework.helpers.vars.env_consts import Constants


class CtsTestScript(TestScript):
    """
    base test case class
    """
    TIMEOUT = 60
    """:type: int"""
    DESCRIPTION = "Not specified"
    """:type: str"""
    CONFIGURATION_PARAMETERS = [
        TestScript.ConfigurationParameter(parameter_name="ApiEndpoint",
                                          parameter_description="Endpoint to api that will be tested",
                                          parameter_type=str, is_required=True, parameter_default_value=None),
        TestScript.ConfigurationParameter(parameter_name="UseSSL",
                                          parameter_description="defines if CTS shall use ssl to communicate with api",
                                          parameter_type=str, is_required=True, parameter_default_value="No"),
        TestScript.ConfigurationParameter(parameter_name="CertificateCertFile",
                                          parameter_description="Path to file with certificate in PEM format",
                                          parameter_type=str, is_required=False, parameter_default_value=None),
        TestScript.ConfigurationParameter(parameter_name="CertificateKeyFile",
                                          parameter_description="Path to file with certificate key in PEM format",
                                          parameter_type=str, is_required=False, parameter_default_value=None),
        TestScript.ConfigurationParameter(parameter_name="User", parameter_type=str, is_required=False,
                                          parameter_description="If specified api will use specified user to authorize " +
                                                                "using basic auth",
                                          parameter_default_value=None),
        TestScript.ConfigurationParameter(parameter_name="Password",
                                          parameter_description="If specified api will use specified password to " +
                                                                "authorize using basic auth", parameter_type=str,
                                          is_required=False, parameter_default_value=None),
        TestScript.ConfigurationParameter(parameter_name="IgnoreTypes",
                                          parameter_description="List (comma separated) of fully qualified types. " +
                                                                "Entities, properties of that types will not be validated.",
                                          parameter_type=str, is_required=False, parameter_default_value=""),
        TestScript.ConfigurationParameter(parameter_name="MapTypes",
                                          parameter_description="List (comma separated) of pairs T1:T2, " +
                                                                "where T1 is fully qualified type that is unknown to CTS " +
                                                                "and T2 is fully qualified type that is known to CTS. " +
                                                                "CTS will use T2 to validate instances of T1.",
                                          parameter_type=str, is_required=False, parameter_default_value=""),
        TestScript.ConfigurationParameter(parameter_name="ServiceTypeOverride",
                                          parameter_description="List (comma separated) of REST services that are available on the API endpoint." +
                                                                "Possible values are: %s" % ServiceTypes.list_s(),
                                          parameter_type=str, is_required=False, parameter_default_value=""),
        TestScript.ConfigurationParameter(parameter_name="MetadataDirectory",
                                          parameter_description="Location of alternative .xml files that constitute " +
                                                                "metadata. If set, CTS will load metadata from given "
                                                                "location instead of built-in one." ,
                                          parameter_type=str, is_required=False, parameter_default_value=""),
        TestScript.ConfigurationParameter(parameter_name="EventListenerAddr",
                                          parameter_description="IP (optionally with a port) of the CTS device.",
                                          parameter_type=str, is_required=False, parameter_default_value=""),
        # new since 2.4
        TestScript.ConfigurationParameter(parameter_name="RedfishMetadata",
                                          parameter_description="If present it will try to get Redfish's metadata (2018.1)",
                                          parameter_type=float, is_required=False, parameter_default_value=""),
    ]
    """:type: list[TestCase.ConfigurationParameter]"""

    def get_config(self, item):
        for element in self.CONFIGURATION_PARAMETERS:
            if element.name == item:
                return element

    @classmethod
    def service_cli(cls):
        all_params = cls.CONFIGURATION_PARAMETERS + \
                     super(CtsTestScript, cls).CONFIGURATION_PARAMETERS + \
                     CtsTestScript.CONFIGURATION_PARAMETERS

        #  remove duplicates
        seen = set()
        cls.CONFIGURATION_PARAMETERS = [param for param in all_params if not (param in seen or seen.add(param))]

        super(CtsTestScript, cls).service_cli()

    def before_run(self):
        digest = DirDigest(Constants.DIR_HOME_CTS_TESTS, '.py', DirDigest.LABEL_TESTS)
        if not digest.is_valid() and not getenv('CTS_SKIP', None):
            cts_error("Tests located in {dir} are corrupted or has been tampered. Expected: {expected}, "
                      "Is: {current}", dir=Constants.DIR_HOME_CTS_TESTS, expected=digest.official_digest,
                      current=digest.digest)
            print "MESSAGE::{count} test scripts have been found in {dir}".format(count=len(digest),
                                                                                  dir=Constants.DIR_HOME_CTS_TESTS)

            digest.report_differences()

        self._parse_ignore_types()
        self._parse_map_types()
        self._parse_service_types_override()
        ReplayController.initialize()

    def _parse_service_types_override(self):
        self._service_types_override = {service for service in
                                        (type.strip() for type in self.configuration.ServiceTypeOverride.split(","))
                                        if len(service) > 0
                                        }

        illegal = self._service_types_override - set(ServiceTypes.all())
        if len(illegal) > 0:
            cts_error("Invalid service types specified in ServiceTypeOverride configuration parameter: {illegal}",
                      illegal=", ".join(illegal))
        self._service_types_override -= illegal

        if len(self._service_types_override) > 0:
            cts_warning("Test was designed to be run against metadata for: {registered}, " +
                        "but user declared that service supports: {override}",
                        registered=self.registered_services_s(), override=", ".join(self._service_types_override))

    def _parse_map_types(self):
        if self.configuration.MapTypes:
            try:
                self._map_types = {from_type: to_type for (from_type, to_type) in \
                                   ((pair[0].strip(), pair[1].strip()) for pair in \
                                    (pair.split(':') for pair in self.configuration.MapTypes.split(',')) \
                                    )
                                   if len(from_type) > 0 and len(to_type) > 0
                                   }
                if len(self._map_types) > 0:
                    print "WARNING::CTS is configured to use the following type mappings to " \
                          "validate instances of unknown types:"
                    for from_type in self._map_types:
                        cts_warning("\t- {from_type}:{to_type}",
                                    from_type=from_type,
                                    to_type=self._map_types[from_type])
            except Exception as error:
                cts_error("Error while parsing 'MapTypes' configuration parameter. error = {error}",
                          error=error)
        else:
            self._map_types = dict()

    def _parse_ignore_types(self):
        if len(self.configuration.IgnoreTypes):
            self._ignore_types = set([type.strip() for type in self.configuration.IgnoreTypes.split(",")])
            print "WARNING::CTS is configured to skip properties and entities of the following " \
                  "types:"
            for ignore_type in self._ignore_types:
                cts_warning("\t- {ignore_type}", ignore_type=ignore_type)

    def _load_metadata(self):
        qualifiers, services = self.get_services_and_qualifiers()

        metadata_manager = MetadataManager(qualifiers,
                                           ignore_types=self.ignore_types,
                                           map_types=self.map_types)
        if self.configuration.MetadataDirectory:
            cts_warning("CTS is configured to use custom metadata from: {dir}",
                        dir=self.configuration.MetadataDirectory)
            metadata_container = self._load_custom_metadata(metadata_manager, self.configuration.MetadataDirectory)
        elif self.configuration.RedfishMetadata:
            cts_warning("CTS is configured to use Redfish metadata: {metadata_version}",
                        metadata_version=self.configuration.RedfishMetadata)
            metadata_container = \
                self._load_custom_metadata(metadata_manager, '/'.join((Constants.METADATA_REDFISH_DIR,
                                                                      str(self.configuration.RedfishMetadata))))
        else:
            metadata_container = self._load_built_in_metadata(metadata_manager, services)

        if metadata_container is not None:
            metadata_container.print_types()
        return metadata_container

    @staticmethod
    def _load_custom_metadata(metadata_manager, dir):
        if not metadata_manager.read_metadata_from_dir(dir):
            return None
        return metadata_manager.metadata_container

    @staticmethod
    def _load_built_in_metadata(metadata_manager, services):
        if not metadata_manager.read_metadata_for_services(*services):
            return None
        return metadata_manager.metadata_container

    def get_services_and_qualifiers(self):
        services = self.registered_services if len(self._service_types_override) == 0 \
            else list(self._service_types_override)
        qualifiers = list(services)
        for qualifier in services:
            underscore = qualifier.find('_')
            if -1 != underscore:
                qualifiers.append(qualifier[:underscore])
        print "MESSAGE::Loading metadata for services: %s, using qualifiers: %s" \
              % (", ".join(services), ", ".join(qualifiers))
        return qualifiers, services

    def registered_services_s(self):
        return ', '.join(self.registered_services)

    @property
    def metadata_container(self):
        if not hasattr(self, "_metadata_container"):
            self._metadata_container = self._load_metadata()
        return self._metadata_container

    @property
    def ignore_types(self):
        if not hasattr(self, "_ignore_types"):
            self._ignore_types = set()
        return self._ignore_types

    @property
    def map_types(self):
        if not hasattr(self, "_map_types"):
            self._map_types = dict()
        return self._map_types
