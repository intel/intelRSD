/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.common.logger.Logger;
import com.intel.podm.rest.representation.json.errors.ErrorType;
import com.intel.podm.rest.representation.json.providers.JsonProviderException;

import javax.inject.Inject;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import javax.ws.rs.ext.ExceptionMapper;
import javax.ws.rs.ext.Provider;

import static com.intel.podm.rest.error.ErrorResponseCreator.from;
import static com.intel.podm.rest.representation.json.errors.ErrorType.INVALID_PAYLOAD;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Provider
@Produces(APPLICATION_JSON)
public class JsonProviderExceptionMapper implements ExceptionMapper<JsonProviderException> {
    @Inject
    private Logger logger;

    @Override
    public Response toResponse(JsonProviderException exception) {
        logger.e("JSON Processing error", exception);

        ErrorType errorType = INVALID_PAYLOAD;
        return from(errorType).create();
    }
}
