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


class MockConstants:
    def __init__(self):
        pass

    @staticmethod
    def delete_resource():
        status = True
        status_code = 204
        response_body = {}
        headers = {'X-Powered-By': 'Undertow/1', 'Expires': '0', 'Server': 'WildFly/9', 'Pragma': 'no-cache',
                   'Cache-Control': 'no-cache, no-store, must-revalidate, max-age=0',
                   'Date': 'Mon, 05 Feb 2018 08:00:00 GMT', 'OData-Version': '4.0', 'Access-Control-Allow-Origin': '*'}
        return status, status_code, response_body, headers
