/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.redfish;

import lombok.experimental.UtilityClass;
import lombok.val;
import org.springframework.http.HttpStatus;
import org.springframework.validation.BindingResult;

import static com.intel.rsd.redfish.RedfishErrorsArchetype.findArchetype;
import static java.lang.String.format;

@UtilityClass
public class RedfishErrors {

    public static RedfishError createRedfishError(HttpStatus statusCode, BindingResult bindingResult) {
        val archetype = findArchetype(statusCode);
        return new RedfishError(
            archetype.getHttpStatus(),
            archetype.getRedfishErrorcode(),
            archetype.getMessage(),
            bindingResult.getFieldErrors().stream()
                .map(e -> format("%s.%s: %s", e.getObjectName(), e.getField(), e.getDefaultMessage()))
                .toArray(String[]::new)
        );
    }

    public static RedfishError createRedfishError(HttpStatus statusCode, String... extendedInfo) {
        val archetype = findArchetype(statusCode);
        return new RedfishError(archetype.getHttpStatus(), archetype.getRedfishErrorcode(), archetype.getMessage(), extendedInfo);
    }

    public static RedfishError createRedfishErrorWithCustomMessage(HttpStatus statusCode, String message, String... extendedInfo) {
        val archetype = findArchetype(statusCode);
        return new RedfishError(archetype.getHttpStatus(), archetype.getRedfishErrorcode(), message, extendedInfo);
    }
}
