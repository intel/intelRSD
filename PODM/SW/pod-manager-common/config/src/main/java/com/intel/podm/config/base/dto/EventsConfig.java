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

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.config.base.ConfigFile;

import javax.ws.rs.core.UriBuilderException;
import java.net.URI;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

import static com.intel.podm.common.utils.Networking.getIpAddressesForNetworkInterface;
import static java.lang.String.format;
import static java.util.Optional.ofNullable;
import static javax.ws.rs.core.UriBuilder.fromUri;

@ConfigFile(filename = "events.json")
@SuppressWarnings({"checkstyle:MethodCount"})
public class EventsConfig extends BaseConfig {
    @JsonIgnore
    public static final String EVENT_RECEIVING_ENDPOINT = "/rest/EventListener";

    @JsonIgnore
    private static final Map<ServiceType, EventConfiguration> EVENT_CONFIGURATION_CACHE = new ConcurrentHashMap<>();

    @JsonIgnore
    private static final String HTTP_SCHEME = "http";

    @JsonIgnore
    private static final String HTTPS_SCHEME = "https";

    @JsonIgnore
    private static final Integer HTTP_POD_MANAGER_PORT = 8080;

    @JsonIgnore
    private static final Integer HTTPS_POD_MANAGER_PORT = 8443;

    @JsonProperty("EventSubscriptionIntervalSeconds")
    private long eventSubscriptionIntervalSeconds = 90;

    @JsonProperty("PodManagerIpAddressForEventsFromPsme")
    private String podManagerIpAddressForEventsFromPsme;

    @JsonProperty("NetworkInterfaceNameForEventsFromPsme")
    private String networkInterfaceNameForEventsFromPsme;

    @JsonProperty("PodManagerIpAddressForEventsFromRss")
    private String podManagerIpAddressForEventsFromRss;

    @JsonProperty("NetworkInterfaceNameForEventsFromRss")
    private String networkInterfaceNameForEventsFromRss;

    @JsonProperty("PodManagerIpAddressForEventsFromRmm")
    private String podManagerIpAddressForEventsFromRmm;

    @JsonProperty("NetworkInterfaceNameForEventsFromRmm")
    private String networkInterfaceNameForEventsFromRmm;

    @JsonProperty("Northbound")
    private NorthboundConfiguration northboundConfiguration = new NorthboundConfiguration();

    public long getEventSubscriptionIntervalSeconds() {
        return eventSubscriptionIntervalSeconds;
    }

    public String getPodManagerIpAddressForEventsFromPsme() {
        return podManagerIpAddressForEventsFromPsme;
    }

    public String getNetworkInterfaceNameForEventsFromPsme() {
        return networkInterfaceNameForEventsFromPsme;
    }

    public String getPodManagerIpAddressForEventsFromRss() {
        return podManagerIpAddressForEventsFromRss;
    }

    public String getNetworkInterfaceNameForEventsFromRss() {
        return networkInterfaceNameForEventsFromRss;
    }

    public String getPodManagerIpAddressForEventsFromRmm() {
        return podManagerIpAddressForEventsFromRmm;
    }

    public String getNetworkInterfaceNameForEventsFromRmm() {
        return networkInterfaceNameForEventsFromRmm;
    }

    public NorthboundConfiguration getNorthboundConfiguration() {
        return northboundConfiguration;
    }

    public EventConfiguration getEventConfigForServiceType(ServiceType serviceType) {
        if (EVENT_CONFIGURATION_CACHE.containsKey(serviceType)) {
            return EVENT_CONFIGURATION_CACHE.get(serviceType);
        } else {
            EventConfiguration configuration = new EventConfiguration(serviceType, buildEventServiceDestinationUri(serviceType, false),
                buildEventServiceDestinationUri(serviceType, true));
            EVENT_CONFIGURATION_CACHE.putIfAbsent(serviceType, configuration);
            return configuration;
        }
    }

    private URI buildEventServiceDestinationUri(ServiceType serviceType, Boolean sslEnabled) {
        try {
            String ipAddress = getPodManagerIpForReceivingEventsFromServiceOfType(serviceType);
            String path = EVENT_RECEIVING_ENDPOINT + "/" + serviceType.name().toLowerCase();
            return fromUri("{scheme}://{ip}:{port}")
                .resolveTemplate("scheme", sslEnabled ? HTTPS_SCHEME : HTTP_SCHEME)
                .resolveTemplate("ip", ipAddress)
                .resolveTemplate("port", sslEnabled ? HTTPS_POD_MANAGER_PORT : HTTP_POD_MANAGER_PORT)
                .path(path)
                .build();
        } catch (IllegalArgumentException | UriBuilderException e) {
            throw new IllegalArgumentException("Couldn't create valid PodM Event Service destination", e);
        }
    }

    private String getPodManagerIpForReceivingEventsFromServiceOfType(ServiceType serviceType) {
        switch (serviceType) {
            case PSME:
                return ofNullable(getPodManagerIpAddressForEventsFromPsme())
                    .orElseGet(() -> getIpAddressByNetworkInterface(getNetworkInterfaceNameForEventsFromPsme()));
            case RSS:
                return ofNullable(getPodManagerIpAddressForEventsFromRss())
                    .orElseGet(() -> getIpAddressByNetworkInterface(getNetworkInterfaceNameForEventsFromRss()));
            case RMM:
                return ofNullable(getPodManagerIpAddressForEventsFromRmm())
                    .orElseGet(() -> getIpAddressByNetworkInterface(getNetworkInterfaceNameForEventsFromRmm()));
            default:
                throw new IllegalArgumentException(format("Couldn't get PodM IP address for service type '%s'", serviceType));
        }
    }

    private String getIpAddressByNetworkInterface(String ifaceName) {
        return getIpAddressesForNetworkInterface(ifaceName)
            .stream()
            .findFirst()
            .orElseThrow(() -> new IllegalArgumentException(format("Couldn't get PodM IP address for network interface type '%s'", ifaceName)));
    }

    @Override
    public boolean configUpdateIsAccepted(BaseConfig updatedConfig) {
        if (!(updatedConfig instanceof EventsConfig)) {
            return false;
        }

        EventsConfig newConfig = (EventsConfig) updatedConfig;
        return samePsmeConfig(newConfig) && sameRssConfig(newConfig) && sameRmmConfig(newConfig);
    }

    private boolean samePsmeConfig(EventsConfig newConfig) {
        return Objects.equals(newConfig.getPodManagerIpAddressForEventsFromPsme(), getPodManagerIpAddressForEventsFromPsme())
            && Objects.equals(newConfig.getNetworkInterfaceNameForEventsFromPsme(), getNetworkInterfaceNameForEventsFromPsme());
    }

    private boolean sameRssConfig(EventsConfig newConfig) {
        return Objects.equals(newConfig.getPodManagerIpAddressForEventsFromRss(), getPodManagerIpAddressForEventsFromRss())
            && Objects.equals(newConfig.getNetworkInterfaceNameForEventsFromRss(), getNetworkInterfaceNameForEventsFromRss());
    }

    private boolean sameRmmConfig(EventsConfig newConfig) {
        return Objects.equals(newConfig.getPodManagerIpAddressForEventsFromRmm(), getPodManagerIpAddressForEventsFromRmm())
            && Objects.equals(newConfig.getNetworkInterfaceNameForEventsFromRmm(), getNetworkInterfaceNameForEventsFromRmm());
    }

    public static final class EventConfiguration {
        private final ServiceType serviceType;
        private final URI defaultPodManagerEventReceivingEndpoint;
        private final URI securePodManagerEventReceivingEndpoint;

        public EventConfiguration(ServiceType serviceType, URI defaultPodManagerEventReceivingEndpoint,
                                  URI securePodManagerEventReceivingEndpoint) {
            this.serviceType = serviceType;
            this.defaultPodManagerEventReceivingEndpoint = defaultPodManagerEventReceivingEndpoint;
            this.securePodManagerEventReceivingEndpoint = securePodManagerEventReceivingEndpoint;
        }

        public ServiceType getServiceType() {
            return serviceType;
        }

        public URI getDefaultPodManagerEventReceivingEndpoint() {
            return defaultPodManagerEventReceivingEndpoint;
        }

        public URI getSecurePodManagerEventReceivingEndpoint() {
            return securePodManagerEventReceivingEndpoint;
        }
    }

    public static class NorthboundConfiguration {
        @JsonProperty("DeliveryRetryAttempts")
        private long deliveryRetryAttempts = 3;

        @JsonProperty("DeliveryRetryIntervalSeconds")
        private long deliveryRetryIntervalSeconds = 3;

        public long getDeliveryRetryAttempts() {
            return deliveryRetryAttempts;
        }

        public long getDeliveryRetryIntervalSeconds() {
            return deliveryRetryIntervalSeconds;
        }
    }
}
