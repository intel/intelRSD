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

package com.intel.rsd.nodecomposer.types.actions;

public class EndpointUpdateDefinition {
    private Oem oem = new Oem();

    public EndpointUpdateDefinition() {
    }

    public EndpointUpdateDefinition(String username, String password) {
        this.oem.rackScaleOem.authentication.username = username;
        this.oem.rackScaleOem.authentication.password = password;
    }

    public Oem getOem() {
        return oem;
    }

    public static class Oem {
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }
    }

    public static class RackScaleOem {
        private Authentication authentication = new Authentication();

        public Authentication getAuthentication() {
            return authentication;
        }
    }

    public static class Authentication {
        private String username;
        private String password;

        public String getUsername() {
            return username;
        }

        public void setUsername(String username) {
            this.username = username;
        }

        public String getPassword() {
            return password;
        }

        public void setPassword(String password) {
            this.password = password;
        }
    }
}
