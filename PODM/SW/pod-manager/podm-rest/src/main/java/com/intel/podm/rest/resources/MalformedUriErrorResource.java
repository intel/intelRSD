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
import static com.intel.podm.rest.error.ErrorType.MALFORMED_URI;
import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Produces(APPLICATION_JSON)
public class MalformedUriErrorResource {

    @GET
    public Response malformedUriGet() {
        return getMalformedUriResponse();
    }

    @POST
    public Response malformedUriPost() {
        return getMalformedUriResponse();
    }

    @PUT
    public Response malformedUriPut() {
        return getMalformedUriResponse();
    }

    @PATCH
    public Response malformedUriPatch() {
        return getMalformedUriResponse();
    }

    @DELETE
    public Response malformedUriDelete() {
        return getMalformedUriResponse();
    }

    @HEAD
    public Response malformedUriHead() {
        return getMalformedUriResponse();
    }

    @OPTIONS
    public Response malformedUriOptions() {
        return getMalformedUriResponse();
    }

    private Response getMalformedUriResponse() {
        return newErrorResponseBuilder(MALFORMED_URI).build();
    }

}
