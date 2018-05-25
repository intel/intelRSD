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

package com.intel.podm.config.base.dto;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.ConnectionParameters;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.config.base.ConfigFile;

import static java.util.Collections.singletonList;

import java.util.List;

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

        @JsonProperty("ConnectionPoolSize")
        private int connectionPoolSize = 200;

        @JsonProperty("MaxPooledPerRoute")
        private int maxPooledPerRoute = 20;

        public ConnectionParameters getConnectionParameters() {
            return new ConnectionParameters(
                serviceConnectionTimeout,
                serviceSocketTimeout,
                connectionPoolSize,
                maxPooledPerRoute
            );
        }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class ConnectionSecurity {
        private static final List<Integer> SSL_PORTS_FOR_SERVICE = singletonList(8443);
        private static final List<Integer> HTTP_PORTS_FOR_SERVICE = singletonList(8888);
        private static final List<Integer> SSL_PORTS_FOR_PSME = singletonList(8443);
        private static final List<Integer> HTTP_PORTS_FOR_PSME = singletonList(8888);
        private static final List<Integer> SSL_PORTS_FOR_RMM = singletonList(8443);
        private static final List<Integer> HTTP_PORTS_FOR_RMM = singletonList(8888);
        private static final List<Integer> SSL_PORTS_FOR_INBAND = singletonList(8448);
        private static final List<Integer> SSL_PORTS_FOR_DISCOVERY_SERVICE = singletonList(8444);

        @JsonProperty("SslEnabledForRmm")
        private boolean sslEnabledForRmm = true;

        @JsonProperty("SslPortsForRmm")
        private List<Integer> sslPortsForRmm = SSL_PORTS_FOR_RMM;

        @JsonProperty("HttpPortsForRmm")
        private List<Integer> httpPortsForRmm = HTTP_PORTS_FOR_RMM;

        @JsonProperty("SslEnabledForPsme")
        private boolean sslEnabledForPsme = true;

        @JsonProperty("SslPortsForPsme")
        private List<Integer> sslPortsForPsme = SSL_PORTS_FOR_PSME;

        @JsonProperty("HttpPortsForPsme")
        private List<Integer> httpPortsForPsme = HTTP_PORTS_FOR_PSME;

        @JsonProperty("SslEnabledForRss")
        private boolean sslEnabledForRss = true;

        @JsonProperty("SslPortsForRss")
        private List<Integer> sslPortsForRss = SSL_PORTS_FOR_SERVICE;

        @JsonProperty("HttpPortsForRss")
        private List<Integer> httpPortsForRss = HTTP_PORTS_FOR_SERVICE;

        @JsonProperty("SslEnabledForLui")
        private boolean sslEnabledForLui = true;

        @JsonProperty("SslPortsForLui")
        private List<Integer> sslPortsForLui = SSL_PORTS_FOR_SERVICE;

        @JsonProperty("HttpPortsForLui")
        private List<Integer> httpPortsForLui = HTTP_PORTS_FOR_SERVICE;

        @JsonProperty("SslPortsForInBand")
        private List<Integer> sslPortsForInBand = SSL_PORTS_FOR_INBAND;

        @JsonProperty("SslPortsForDiscoveryService")
        private List<Integer> sslPortsForDiscoveryService = SSL_PORTS_FOR_DISCOVERY_SERVICE;

        @SuppressWarnings({"checkstyle:CyclomaticComplexity"})
        public boolean isSslEnabledForServicesOfType(ServiceType type) {
            boolean isEnabled;
            switch (type) {
                case RMM:
                    isEnabled = sslEnabledForRmm;
                    break;
                case PSME:
                    isEnabled = sslEnabledForPsme;
                    break;
                case RSS:
                    isEnabled = sslEnabledForRss;
                    break;
                case LUI:
                    isEnabled = sslEnabledForLui;
                    break;
                case INBAND:
                case DISCOVERY_SERVICE:
                    return true;
                default:
                    throw new UnsupportedOperationException(format("Connection configuration for service %s is not supported", type));
            }
            return isEnabled;
        }

        public List<Integer> getDefaultPortsForServicesOfType(ServiceType type) {
            switch (type) {
                case RMM:
                    return httpPortsForRmm;
                case PSME:
                    return httpPortsForPsme;
                case RSS:
                    return httpPortsForRss;
                case LUI:
                    return httpPortsForLui;
                default:
                    throw new UnsupportedOperationException(format("Connection configuration for service %s is not supported", type));
            }
        }

        @SuppressWarnings({"checkstyle:CyclomaticComplexity", "checkstyle:MethodLength"})
        public List<Integer> getSslPortsForServicesOfType(ServiceType type) {
            List<Integer> sslPort;
            switch (type) {
                case RMM:
                    sslPort = sslPortsForRmm;
                    break;
                case PSME:
                    sslPort = sslPortsForPsme;
                    break;
                case RSS:
                    sslPort = sslPortsForRss;
                    break;
                case LUI:
                    sslPort = sslPortsForLui;
                    break;
                case INBAND:
                    sslPort = sslPortsForInBand;
                    break;
                case DISCOVERY_SERVICE:
                    sslPort = sslPortsForDiscoveryService;
                    break;
                default:
                    throw new UnsupportedOperationException(format("Connection configuration for service %s is not supported", type));
            }
            return sslPort;
        }
    }
}
