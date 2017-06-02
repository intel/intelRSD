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
package com.intel.podm.config.base.dto;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.ConnectionParameters;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.config.base.ConfigFile;

import static java.lang.String.format;

@ConfigFile(filename = "service-connection.json")
@JsonIgnoreProperties(ignoreUnknown = true)
public class ServiceConnectionConfig extends BaseConfig {

    @JsonProperty("ConnectionConfiguration")
    private ConnectionConfiguration connectionConfiguration = new ConnectionConfiguration();

    @JsonProperty("ConnectionSecurity")
    private ConnectionSecurity connectionSecurity = new ConnectionSecurity();

    public ConnectionConfiguration getConnectionConfiguration() {
        return connectionConfiguration;
    }

    public ConnectionSecurity getConnectionSecurity() {
        return connectionSecurity;
    }

    @Override
    public boolean configUpdateIsAccepted(BaseConfig updatedConfig) {
        return updatedConfig instanceof ServiceConnectionConfig;
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class ConnectionConfiguration {
        @JsonProperty("ServiceConnectionTimeoutInSeconds")
        private long serviceConnectionTimeout = 2;

        @JsonProperty("ServiceSocketTimeoutInSeconds")
        private long serviceSocketTimeout = 2;

        public ConnectionParameters getConnectionParameters() {
            return new ConnectionParameters(
                serviceConnectionTimeout,
                serviceSocketTimeout
                );
        }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class ConnectionSecurity {
        @JsonProperty("SslEnabledForRmm")
        private boolean sslEnabledForRmm = true;

        @JsonProperty("SslPortForRmm")
        private int sslPortForRmm = 8091;

        @JsonProperty("DefaultPortForRmm")
        private int defaultPortForRmm = 8090;

        @JsonProperty("SslEnabledForPsme")
        private boolean sslEnabledForPsme = true;

        @JsonProperty("SslPortForPsme")
        private int sslPortForPsme = 8443;

        @JsonProperty("DefaultPortForPsme")
        private int defaultPortForPsme = 8888;

        @JsonProperty("SslEnabledForRss")
        private boolean sslEnabledForRss = true;

        @JsonProperty("SslPortForRss")
        private int sslPortForRss = 8443;

        @JsonProperty("DefaultPortForRss")
        private int defaultPortForRss = 8888;

        @JsonProperty("SslEnabledForLui")
        private boolean sslEnabledForLui = true;

        @JsonProperty("SslPortForLui")
        private int sslPortForLui = 8443;

        @JsonProperty("DefaultPortForLui")
        private int defaultPortForLui = 8888;

        public boolean isSslEnabledForServicesOfType(ServiceType type) {
            switch (type) {
                case RMM: return sslEnabledForRmm;
                case PSME: return sslEnabledForPsme;
                case RSS: return sslEnabledForRss;
                case LUI: return sslEnabledForLui;
                default:
                    throw new UnsupportedOperationException(format("Connection configuration for service %s is not supported", type));
            }
        }

        public int getDefaultPortForServicesOfType(ServiceType type) {
            switch (type) {
                case RMM: return defaultPortForRmm;
                case PSME: return defaultPortForPsme;
                case RSS: return defaultPortForRss;
                case LUI: return defaultPortForLui;
                default:
                    throw new UnsupportedOperationException(format("Connection configuration for service %s is not supported", type));
            }
        }

        public int getSslPortForServicesOfType(ServiceType type) {
            switch (type) {
                case RMM: return sslPortForRmm;
                case PSME: return sslPortForPsme;
                case RSS: return sslPortForRss;
                case LUI: return sslPortForLui;
                default:
                    throw new UnsupportedOperationException(format("Connection configuration for service %s is not supported", type));
            }
        }
    }
}
