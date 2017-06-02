/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.redfish.resources;

import com.fasterxml.jackson.annotation.JsonProperty;

import javax.ws.rs.Path;
import javax.ws.rs.Produces;

import static javax.ws.rs.core.MediaType.APPLICATION_JSON;

@Path("/redfish")
@Produces(APPLICATION_JSON)
public class RedfishResource extends BaseResource {

    @Override
    public Object get() {
        return new V1();
    }

    @Path("v1")
    public ServiceRootResource getServiceRoot() {
        return getResource(ServiceRootResource.class);
    }

    static class V1 {
        @JsonProperty("v1")
        String v1 = "/redfish/v1";
    }
}
