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
import re
from cts_framework.db.dao.http_request_dao import HttpRequestDAO


request_id_re = re.compile('request_id=(?P<request_id>\d+)')


class Formatter:
    def __init__(self):
        pass

    def print_results(self, run_id):
        print ""

    @staticmethod
    def request_from_control_channel(metadata_text):
        try:
            m = request_id_re.match(metadata_text)
            if m:
                request_id = int(m.group('request_id'))
                status_code, request, response = HttpRequestDAO.get_code_request_response(request_id)
                if request:
                    yield status_code, request, response
        except:
            pass
