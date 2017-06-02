/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.client.api;

import com.intel.podm.common.types.redfish.RedfishErrorResponse;

import java.net.URI;

public class ExternalServiceApiReaderException extends RedfishApiException {

    public ExternalServiceApiReaderException(String msg, URI resourceUri) {
        super(msg, null, resourceUri, null);
    }

    public ExternalServiceApiReaderException(String msg, URI resourceUri, Throwable cause) {
        super(msg, cause, resourceUri, null);
    }

    public ExternalServiceApiReaderException(String msg, URI resourceUri, Throwable cause, RedfishErrorResponse errorResponse) {
        super(msg, cause, resourceUri, errorResponse);
    }
}
