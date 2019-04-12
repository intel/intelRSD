/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.types.redfish;

import com.intel.rsd.nodecomposer.types.net.HttpMethod;
import com.intel.rsd.nodecomposer.types.net.HttpStatusCode;

import java.net.URI;

public interface ExternalServiceError {
    Request getRequest();
    Response getResponse();
    RedfishErrorResponse getRedfishErrorResponse();

    interface Request {
        HttpMethod getHttpMethod();
        URI getRequestUri();
        Object getRequestBody();
    }

    interface Response {
        HttpStatusCode getHttpStatusCode();
        Object getResponseBody();
    }
}
