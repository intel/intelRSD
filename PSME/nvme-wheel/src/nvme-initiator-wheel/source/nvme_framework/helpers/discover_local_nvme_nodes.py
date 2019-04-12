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

from os import listdir
from nvme_framework.helpers.message import Message


class DiscoverLocal:
    def __init__(self):
        self._nvme_list = self.get_local_nvme_nodes()
        self._last_found = None

    def __str__(self):
        return self._last_found

    @staticmethod
    def get_local_nvme_nodes():
        nvme_list = [x for x in listdir('/sys/class/nvme/') if x.find('nvme') != -1]
        return nvme_list

    def get_recently_added_nvme_node(self):
        try:
            self._last_found = self.__compare(self.get_local_nvme_nodes())[0]
        except:
            Message.warning('No new devices connected')
            return None
        return self._last_found

    def __compare(self, actual_nvme_devices):
        for previously_discovered in self._nvme_list:
            if previously_discovered in actual_nvme_devices:
                actual_nvme_devices.remove(previously_discovered)

        if len(actual_nvme_devices) != 1:
            Message.warning('More devices connected {connected}'.format(connected=actual_nvme_devices))
        return actual_nvme_devices
