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
import subprocess
import sys
import threading
import time

from crawling.bot import Bot
from crawling.security_context_initializer import SecurityContextInitializer
from data_model.converters.data_to_json_converter import DataToJsonConverter
from data_model.converters.json_to_data_converter import JsonToDataConverter
from data_model.data import Data
from data_model.service_description import ServiceDescription
from mock_server.mock_handler import MockHandler
from mock_server.mock_server import MockServer
from services_retrieval.parsers.leases_parser import LeasesParser
from services_retrieval.parsers.pod_manager_log_parser import PodManagerLogParser
from services_retrieval.parsers.services_list_parser import ServicesListParser
from services_retrieval.service_retriever import ServiceRetriever


class ScriptController:
    @staticmethod
    def crawl(arguments):
        service_description = ServiceDescription.from_uri(arguments.uri)
        data = ScriptController._crawl_services([service_description])
        print(DataToJsonConverter.dumps(data, False))

    @staticmethod
    def full_crawl(arguments):
        if arguments.remote is not None:
            subprocess.call(['scp', '-r', '.', '{}:crawler'.format(arguments.remote)])

            if arguments.minimal:
                minimal_option = '--minimal'
            else:
                minimal_option = ''

            if arguments.parse_log:
                parse_log_option = '--parse_log'
            else:
                parse_log_option = ''

            subprocess.call(['ssh', '-t', arguments.remote, './crawler/crawler.py --log-file-name crawler/crawler.log crawl {minimal_option} {parse_log_option} crawler/output.json'.format(
                minimal_option=minimal_option,
                parse_log_option=parse_log_option)
            ])
            subprocess.call(['scp', '{}:crawler/output.json'.format(arguments.remote), arguments.output_file_name])
            subprocess.call(['scp', '{}:crawler/crawler.log'.format(arguments.remote), arguments.log_file_name])
            subprocess.call(['ssh', arguments.remote, 'rm -rf crawler'])
            sys.exit()

        ServiceRetriever.add_parser(LeasesParser)
        ServiceRetriever.add_parser(ServicesListParser)

        if arguments.parse_log:
            ServiceRetriever.add_parser(PodManagerLogParser)

        service_descriptions = ServiceRetriever.retrieve_service_descriptions()

        logging.debug('========== Crawling services')

        data = ScriptController._crawl_services(service_descriptions)

        logging.debug('========== Outcome\n{data}'.format(data=data))

        print(DataToJsonConverter.dumps(data, arguments.minimal))

    @staticmethod
    def mock(arguments):
        data = JsonToDataConverter.load(arguments.input_file_name)

        print('SERVICE-URI SERVICE-TYPE => LOCAL-PORT')

        server_threads = []
        port = int(arguments.starting_port)

        for service in data.get_services():
            service_description = service.get_service_description()

            print('{0}:{1} {2} => {3}'.format(
                service_description.get_service_localization().get_host(),
                service_description.get_service_localization().get_port(),
                service_description.get_service_parameters().get_service_type(),
                port
            ))

            server = MockServer(('', port), MockHandler, service)
            server_thread = threading.Thread(target=server.serve_forever)
            server_threads.append(server_thread)
            server_thread.start()

            port += 1

        for thread in server_threads:
            thread.join()

    @staticmethod
    def _crawl_services(service_descriptions):
        security_context = SecurityContextInitializer.init_security_context()
        Bot.set_security_context(security_context)

        data = Data()

        for service_description in service_descriptions:
            logging.debug('Retrieving SERVICE: {service_description}'.format(service_description=service_description))
            bot = Bot(service_description)
            begin = time.time()
            service = bot.crawl()
            end = time.time()
            duration = end - begin
            service.set_retrieval_duration(duration)
            data.add_service(service)

        return data
