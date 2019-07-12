/*
 * Copyright (c) 2016-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


class RedfishError {
    constructor(redfish_error_code, redfish_error_message) {
        this._template = {
            'error': {
                'code': redfish_error_code,
                'message': redfish_error_message,
                '@Message.ExtendedInfo': []
            }
        }
    }

    addExtendedInfo(message) {
        this._template.error['@Message.ExtendedInfo'].push({'Message': message});
        return this;
    }

    build() {
        return JSON.parse(JSON.stringify(this._template));
    }
}

var mockError = (mock, uri, code, response) => {
    mock.mockGet(uri, code, response);
    mock.mockPost(uri, code, response);
    mock.mockPatch(uri, code, response);
    mock.mockDelete(uri, code, response);
};

module.exports = {
    RedfishError: RedfishError,
    MockError: mockError
}