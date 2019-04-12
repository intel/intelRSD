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

package com.intel.rsd.nodecomposer.rest.error;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.intel.rsd.nodecomposer.types.redfish.ExternalServiceError;

import static com.intel.rsd.json.JsonUtils.createStringWith;
import static java.lang.String.format;

public final class LogMessageBuilder {
    private final String error;
    private String message;
    private ExternalServiceError externalServiceError;

    private LogMessageBuilder(String error) {
        this.error = error;
    }

    public static LogMessageBuilder newLogMessageBuilder(String error) {
        return new LogMessageBuilder(error);
    }

    public LogMessageBuilder withMessage(String message) {
        this.message = message;
        return this;
    }

    public LogMessageBuilder withExternalServiceError(ExternalServiceError externalServiceError) {
        this.externalServiceError = externalServiceError;
        return this;
    }

    public String build() {
        StringBuilder builder = new StringBuilder(format("[ Error: %s ]", error));
        if (message != null) {
            builder.append(format(" [ Message: %s ]", message));
        }

        // Log using left bottom corner box Unicode character: \u2514
        if (externalServiceError != null) {
            builder.append("\n  \u2514 Request on dependent service failed\n");
            builder.append(format("    \u2514 [Request: %s %s '%s']\n",
                externalServiceError.getRequest().getHttpMethod(),
                externalServiceError.getRequest().getRequestUri(),
                trySerializeToJson(externalServiceError.getRequest().getRequestBody()))
            );
            builder.append(format("    \u2514 [Response: (status %s) '%s']\n",
                externalServiceError.getResponse().getHttpStatusCode(),
                trySerializeToJson(externalServiceError.getResponse().getResponseBody()))
            );
        }

        return builder.toString();
    }

    private String trySerializeToJson(Object object) {
        try {
            return createStringWith(object);
        } catch (JsonProcessingException e) {
            return object.toString();
        }
    }
}
