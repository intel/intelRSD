/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.redfish.response;

import com.intel.rsd.nodecomposer.types.redfish.ExternalServiceError;
import com.intel.rsd.nodecomposer.types.redfish.ExternalServiceErrorCarryingException;

public class RedfishClientException extends Exception implements ExternalServiceErrorCarryingException {
    private static final long serialVersionUID = -8618892036382737140L;
    private final ExternalServiceError externalServiceError;

    public RedfishClientException(String message, ExternalServiceError externalServiceError) {
        super(message);
        this.externalServiceError = externalServiceError;
    }

    public RedfishClientException(Throwable cause) {
        super(cause);
        externalServiceError = null;
    }

    @Override
    public ExternalServiceError getExternalServiceError() {
        return externalServiceError;
    }
}
