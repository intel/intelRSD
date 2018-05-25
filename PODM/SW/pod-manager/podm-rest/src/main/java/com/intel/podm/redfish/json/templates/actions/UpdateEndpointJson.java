/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.redfish.RedfishEndpoint;

@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class UpdateEndpointJson implements RedfishEndpoint {
    @JsonProperty("Oem")
    public Oem oem;

    @Override
    public Oem getOem() {
        return oem;
    }

    public static class Oem implements RedfishEndpoint.Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem;

        @Override
        public RedfishEndpoint.RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }
    }

    public static class RackScaleOem implements RedfishEndpoint.RackScaleOem {
        @JsonProperty("@odata.type")
        private String oDataType;

        @JsonProperty("Authentication")
        private Authentication authentication;

        @Override
        public Authentication getAuthentication() {
            return authentication;
        }

        public String getOdataType() {
            return oDataType;
        }
    }

    public static class Authentication implements RedfishEndpoint.Authentication {
        @JsonProperty("Username")
        private String username;

        @JsonProperty("Password")
        private String password;

        @Override
        public String getUsername() {
            return username;
        }

        @Override
        public String getPassword() {
            return password;
        }
    }
}
