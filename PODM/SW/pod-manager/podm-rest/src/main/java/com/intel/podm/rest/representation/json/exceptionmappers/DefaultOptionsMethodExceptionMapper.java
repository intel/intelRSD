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

import org.jboss.resteasy.spi.DefaultOptionsMethodException;

import javax.enterprise.context.ApplicationScoped;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;
import javax.ws.rs.ext.ExceptionMapper;
import javax.ws.rs.ext.Provider;

import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

/**
 * Allows default HTTP OPTIONS method handler from Resteasy to be used.
 * This exception mapper must be provided because we have wider one - RuntimeExceptionMapper
 *
 * @see RuntimeExceptionMapper
 */
@ApplicationScoped
@Provider
@Produces(APPLICATION_JSON)
public class DefaultOptionsMethodExceptionMapper implements ExceptionMapper<DefaultOptionsMethodException> {
    @Override
    public Response toResponse(DefaultOptionsMethodException exception) {
        return exception.getResponse();
    }
}
