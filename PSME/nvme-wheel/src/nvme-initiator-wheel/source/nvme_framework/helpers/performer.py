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

from os import path
from nvme_framework.helpers.constants import Constants


class Performer:
    @staticmethod
    def safe_deletions(nvme_id=None):
        if not nvme_id:
            return False
        target_nvme = Constants.NVME_PATH_DELETE_CONTROLLER.format(node_id=nvme_id)
        if not path.exists(target_nvme):
            return False

        with open(target_nvme, 'w') as f:
            f.write('1')
            return True
