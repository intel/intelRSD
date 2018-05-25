/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.rest.representation.json.exceptionmappers;

import com.intel.podm.business.services.context.UriConversionException;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.rest.representation.json.providers.JsonProviderException;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import javax.ws.rs.ext.ExceptionMapper;
import javax.ws.rs.ext.Provider;
import java.util.Arrays;

import static com.intel.podm.rest.error.ErrorResponseBuilder.newErrorResponseBuilder;
import static com.intel.podm.rest.error.ErrorType.INVALID_PAYLOAD;
import static java.lang.String.format;
import static java.util.Collections.singletonList;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;
import static org.apache.commons.lang3.exception.ExceptionUtils.getRootCause;

@ApplicationScoped
@Provider
@Produces(APPLICATION_JSON)
public class JsonProviderExceptionMapper implements ExceptionMapper<JsonProviderException> {
    @Inject
    private Logger logger;

    @Override
    public Response toResponse(JsonProviderException exception) {
        logger.e("JSON Processing error", exception);

        Throwable cause = getRootCause(exception);
        if (cause instanceof UriConversionException) {
            UriConversionException convertedCause = (UriConversionException) cause;

            String detailedMessage = format("Cannot parse URI [%s] to any type from %s",
                convertedCause.getSourceUri(),
                Arrays.toString(convertedCause.getTargetContextTypes())
            );
            return newErrorResponseBuilder(INVALID_PAYLOAD).withDetails(singletonList(detailedMessage)).build();
        }

        return newErrorResponseBuilder(INVALID_PAYLOAD).build();
    }
}
