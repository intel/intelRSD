"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2018 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

from nvme_framework.configuration.config_manager import Config
from nvme_framework.helpers.constants import Constants
from nvme_framework.helpers.message import Message


class JsonParse:
    def __init__(self):
        self.traddr, self.trsvcid, self.subnqn = (None, )*3
        self.discovery_response = None

    def discovery_output(self, discovery_output):
        cfg = Config()

        if str(discovery_output).lower() == str(Constants.NVME_DISCOVERY_GENERIC_ERROR).lower():
            for msg in ['Please mount target.', Constants.NVME_DISCOVERY_GENERIC_ERROR]:
                Message.error(msg)

        list_of_discovered = []
        self.discovery_response = self.try_convert_discovery_output_to_dictionary(str(discovery_output))

        for response in self.discovery_response:
            try:
                list_of_discovered.append(
                    dict(
                        addr=response['traddr'],
                        port=response['trsvcid'],
                        name=response['subnqn']
                    ))
                cfg.save_available_target(response['traddr'], response['trsvcid'], response['subnqn'], "discovered")
            except:
                Message.error('Output from nvme discover looks empty.')
                return []
        return list_of_discovered

    def try_convert_discovery_output_to_dictionary(self, discovery_output=None):
        # find =====Discovery
        discovery_output = discovery_output.split("=====Discovery")
        converted_output = []

        for output in discovery_output:
            if output.find('Discovery Log Number of Records') > 0:
                continue
            dictionary = {}
            for i in self.clean_output(output):
                dic = i.split(':')
                dictionary.update({dic[0]: ':'.join(dic[1:])})
            converted_output.append(dictionary)
        return converted_output

    def clean_output(self, discovery_output):
        output = discovery_output.split('\n')
        clean_output = []
        for element in output:
            if element.lower().find('log entry') > 0:
                continue
            clean_output.append(element)
        return self.remove_whitespace(clean_output)

    @staticmethod
    def remove_whitespace(clean_output):
        output_ready_to_parse = []
        for element in clean_output:
            output_ready_to_parse.append(element.replace(' ', ''))
        return filter(None, output_ready_to_parse)[1:-1]
