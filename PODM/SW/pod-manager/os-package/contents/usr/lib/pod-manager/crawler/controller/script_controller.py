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

import sys
import threading

from subprocess import call

from bot.bot import Bot
from data_model.converters.data_to_json_converter import DataToJsonConverter
from data_model.converters.json_to_data_converter import JsonToDataConverter
from data_model.data import Data
from mock_server.mock_handler import MockHandler
from mock_server.mock_server import MockServer
from parsers.leases_parser import LeasesParser
from parsers.pod_manager_log_parser import PodManagerLogParser
from utils.json_utils import JsonUtils


class ScriptController:
    @staticmethod
    def crawl(arguments):
        if arguments.remote is not None:
            call(['scp', '-r', '.', '{}:crawler'.format(arguments.remote)])

            if arguments.minimal:
                minimal_option = '--minimal'
            else:
                minimal_option = ''

            if arguments.parse_log:
                parse_log_option = '--parse_log'
            else:
                parse_log_option = ''

            call(['ssh', '-t', arguments.remote, 'python3 -B crawler/crawler.py crawl {} {} crawler/output.json'.format(minimal_option, parse_log_option)])
            call(['scp', '{}:crawler/output.json'.format(arguments.remote), arguments.output_file_name])
            call(['ssh', arguments.remote, 'rm -rf crawler'])
            sys.exit()

        if arguments.parse_log:
            service_retrieval_class = PodManagerLogParser
        else:
            service_retrieval_class = LeasesParser

        service_descriptions = service_retrieval_class.retrieve_service_descriptions()

        data = Data()

        for service_description in service_descriptions:
            bot = Bot(service_description)
            service = bot.crawl()
            data.add_service(service)

        json_data_representation = DataToJsonConverter.convert(data, arguments.minimal)

        with open(arguments.output_file_name, 'w') as output_file:
            JsonUtils.dump(json_data_representation, output_file, indent=4)

    @staticmethod
    def mock(arguments):
        with open(arguments.input_file_name) as input_file:
            content = input_file.read()

        json_data_representation = JsonUtils.loads(content)

        data = JsonToDataConverter.convert(json_data_representation)

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
