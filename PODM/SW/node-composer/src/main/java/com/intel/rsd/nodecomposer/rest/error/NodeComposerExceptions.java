/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.error;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.ViolationsDisclosingException;
import lombok.experimental.UtilityClass;
import lombok.val;

import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishErrorWithCustomMessage;
import static java.util.Optional.ofNullable;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.CONFLICT;
import static org.springframework.http.HttpStatus.METHOD_NOT_ALLOWED;
import static org.springframework.http.HttpStatus.NOT_FOUND;

@UtilityClass
public class NodeComposerExceptions {

    public static NodeComposerWebException notFound() {
        val redfishError = createRedfishError(NOT_FOUND);
        return new NodeComposerWebException(redfishError.getHttpStatus(), redfishError);
    }

    public static NodeComposerWebException unsupportedCreationRequest(Violations violations) {
        val redfishError = createRedfishError(BAD_REQUEST, violations.asStringList().toArray(new String[0]));
        return new NodeComposerWebException(redfishError.getHttpStatus(), redfishError);
    }

    public static NodeComposerWebException resourcesStateMismatch(String message, BusinessApiException e) {
        val extendedInfo = ofNullable(e.getCause())
            .filter(ViolationsDisclosingException.class::isInstance)
            .map(ViolationsDisclosingException.class::cast)
            .map(ViolationsDisclosingException::getViolations)
            .map(violations -> violations.asStringList().toArray(new String[0]))
            .orElseGet(() -> new String[]{e.getMessage()});

        val redfishError = createRedfishErrorWithCustomMessage(CONFLICT, message, extendedInfo);
        return new NodeComposerWebException(redfishError.getHttpStatus(), redfishError);
    }

    public static NodeComposerWebException invalidHttpMethod() {
        val redfishError = createRedfishError(METHOD_NOT_ALLOWED);
        return new NodeComposerWebException(redfishError.getHttpStatus(), redfishError);
    }
}
