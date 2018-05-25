/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import javax.ws.rs.ext.ExceptionMapper;
import javax.ws.rs.ext.Provider;

import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.rest.error.ErrorResponseBuilder.newErrorResponseBuilder;
import static com.intel.podm.rest.error.ErrorType.NOT_FOUND;
import static java.lang.String.format;
import static java.util.Collections.singletonList;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@ApplicationScoped
@Provider
@Produces(APPLICATION_JSON)
public class ContextResolvingExceptionMapper implements ExceptionMapper<ContextResolvingException> {
    @Inject
    private Logger logger;

    @Override
    public Response toResponse(ContextResolvingException exception) {
        String detailedMessage;
        if (exception.getContext() != null) {
            detailedMessage = format("Provided URI %s could not be resolved", asOdataId(exception.getContext()));
        } else {
            detailedMessage = exception.getMessage();
        }
        logger.e(detailedMessage, exception);
        return newErrorResponseBuilder(NOT_FOUND)
            .withDetails(singletonList(detailedMessage))
            .build();
    }
}
