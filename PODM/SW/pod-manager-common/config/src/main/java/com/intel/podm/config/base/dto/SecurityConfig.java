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

package com.intel.podm.config.base.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.config.base.ConfigFile;

import static java.lang.String.format;

@ConfigFile(filename = "security.json")
public class SecurityConfig extends BaseConfig {
    public enum CertificateType {
        CLIENT,
        SERVER
    }

    @JsonProperty("ClientKeystorePath")
    private String clientKeystorePath = "/var/lib/pod-manager/client.jks";

    @JsonProperty("ServerKeystorePath")
    private String serverKeystorePath = "/var/lib/pod-manager/server.jks";

    @JsonProperty("ServerCertificateVerificationEnabled")
    private boolean serverCertificateVerificationEnabled;

    public String getKeystorePath(CertificateType type) {
        switch (type) {
            case CLIENT:
                return clientKeystorePath;
            case SERVER:
                return serverKeystorePath;
            default:
                throw new UnsupportedOperationException(format("Certificate of type %s keystore does not exist.", type));
        }
    }

    public boolean isServerCertificateVerificationEnabled() {
        return serverCertificateVerificationEnabled;
    }
}
