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

package com.intel.podm.rest.resources;

import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.HEAD;
import javax.ws.rs.OPTIONS;
import javax.ws.rs.PATCH;
import javax.ws.rs.POST;
import javax.ws.rs.PUT;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Response;

import static com.intel.podm.rest.error.ErrorResponseBuilder.newErrorResponseBuilder;
import static com.intel.podm.rest.error.ErrorType.UNAUTHORIZED;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Produces(APPLICATION_JSON)
public class UnauthorizedErrorResource {

    @GET
    public Response unauthorizedGet() {
        return getUnauthorizedResponse();
    }

    @POST
    public Response unauthorizedPost() {
        return getUnauthorizedResponse();
    }

    @PUT
    public Response unauthorizedPut() {
        return getUnauthorizedResponse();
    }

    @PATCH
    public Response unauthorizedPatch() {
        return getUnauthorizedResponse();
    }

    @DELETE
    public Response unauthorizedDelete() {
        return getUnauthorizedResponse();
    }

    @HEAD
    public Response unauthorizedHead() {
        return getUnauthorizedResponse();
    }

    @OPTIONS
    public Response unauthorizedOptions() {
        return getUnauthorizedResponse();
    }

    private Response getUnauthorizedResponse() {
        return newErrorResponseBuilder(UNAUTHORIZED).build();
    }

}
