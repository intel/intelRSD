/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.rest.error;

import com.intel.podm.common.types.redfish.ExternalServiceError;
import com.intel.podm.common.types.redfish.RedfishErrorResponse;

import static com.intel.podm.business.errors.ErrorDto.ErrorBuilder.newErrorBuilder;
import static com.intel.podm.business.errors.ExtendedInfoDto.ExtendedInfoBuilder.newExtendedInfoBuilder;

public final class ExternalServiceErrorMessageBuilder {
    private static final String MESSAGE_ID = "Base.1.0.ExternalServiceError";
    private static final String MESSAGE = "Request on dependent service failed.";
    private static final String RESOLUTION = "Consult application log for details.";

    private final ExternalServiceError externalServiceError;

    private ExternalServiceErrorMessageBuilder(ExternalServiceError externalServiceError) {
        this.externalServiceError = externalServiceError;
    }

    public static ExternalServiceErrorMessageBuilder newExternalServiceErrorMessageBuilder(ExternalServiceError externalServiceError) {
        return new ExternalServiceErrorMessageBuilder(externalServiceError);
    }

    public RedfishErrorResponse build() {
        RedfishErrorResponse.Error error = newErrorBuilder(externalServiceError.getRedfishErrorResponse().getError().getMessage())
            .withCode(externalServiceError.getRedfishErrorResponse().getError().getCode())
            .addExtendedInfo(newExtendedInfoBuilder(MESSAGE)
                .withMessageId(MESSAGE_ID)
                .withResolution(RESOLUTION)
                .build()
            ).build();

        return new RedfishErrorResponseWrapper(error);
    }
}
