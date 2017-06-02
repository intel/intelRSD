/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.container.ResourceContext;
import javax.ws.rs.core.Context;

import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Path("/rest/errors")
@Produces(APPLICATION_JSON)
public class ErrorResource {
    @Context
    private ResourceContext rc;

    @Path("400")
    public MalformedUriErrorResource malformedUriErrorResource() {
        return rc.getResource(MalformedUriErrorResource.class);
    }

    @Path("401")
    public UnauthorizedErrorResource unauthorizedErrorResource() {
        return rc.getResource(UnauthorizedErrorResource.class);
    }

    @Path("403")
    public ForbiddenErrorResource forbiddenErrorResource() {
        return rc.getResource(ForbiddenErrorResource.class);
    }

}
