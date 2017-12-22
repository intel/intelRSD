# Copyright (c) 2016-2017 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import logging

from data_model.service_description import ServiceDescription
from data_model.service_localization import ServiceLocalization
from data_model.service_parameters import ServiceParameters
from pod_manager_config.pod_manager_defaults import PodManagerDefaults


class ServiceRetriever:
    _parsers = []

    @staticmethod
    def retrieve_service_descriptions():
        logging.debug('========== Loading service descriptions')

        service_descriptions = []
        pod_manager_service_description = ServiceRetriever._create_pod_manager_service_description()
        service_descriptions.append(pod_manager_service_description)
        for parser in ServiceRetriever._parsers:
            loaded_service_descriptions = ServiceRetriever._retrieve_service_descriptions_from_parser(parser)
            service_descriptions += loaded_service_descriptions
        return service_descriptions

    @staticmethod
    def add_parser(parser_class):
        ServiceRetriever._parsers.append(parser_class)

    @staticmethod
    def _retrieve_service_descriptions_from_parser(parser_class):
        logging.debug('---------- Loading service descriptions from {services_register_location}'.format(services_register_location=parser_class.services_register_location))

        service_descriptions = []

        try:
            with open(parser_class.services_register_location) as input_file:
                lines = input_file.readlines()

            for line in lines:
                service_descriptions += parser_class.parse_line(line)

            for service_description in service_descriptions:
                logging.debug('Loaded service: {service_description}'.format(service_description=service_description))
        except FileNotFoundError:
            logging.debug('There is no {services_register_location}'.format(services_register_location=parser_class.services_register_location))

        return service_descriptions

    @staticmethod
    def _create_pod_manager_service_description():
        service_localization = ServiceLocalization(PodManagerDefaults.host, PodManagerDefaults.port)
        service_parameters = ServiceParameters(PodManagerDefaults.service_type, PodManagerDefaults.root_path, PodManagerDefaults.protocol)
        return ServiceDescription(service_localization, service_parameters)
