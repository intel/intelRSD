/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.discovery.external.event;

import com.google.common.collect.HashBiMap;
import com.intel.podm.business.entities.dao.EthernetInterfaceDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.EventsConfig;
import com.intel.podm.config.base.dto.ServiceConfig;
import com.intel.podm.config.base.dto.ServiceConnectionConfig;

import javax.annotation.PostConstruct;
import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.ws.rs.core.UriBuilderException;
import java.net.URI;
import java.util.Optional;
import java.util.UUID;

import static com.intel.podm.common.utils.Networking.getIpAddressesForNetworkInterface;
import static java.lang.String.format;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static javax.ws.rs.core.UriBuilder.fromUri;

@ApplicationScoped
public class EventDataProvider {
    public static final String EVENT_ENDPOINT = "/rest/EventListener";
    public static final String PSME = "psme";
    public static final String RSS = "rss";
    public static final String RMM = "rmm";

    private static final String HTTP_SCHEME = "http";
    private static final String HTTPS_SCHEME = "https";

    private static final Integer HTTP_PODM_PORT = 8080;
    private static final Integer HTTPS_PODM_PORT = 8443;

    private final HashBiMap<ServiceType, String> serviceTypeMap = HashBiMap.create();

    @Inject
    private EthernetInterfaceDao ethernetInterfaceDao;

    @Inject @Config
    private Holder<EventsConfig> eventsConfig;

    @Inject @Config
    private Holder<ServiceConfig> serviceConfig;

    @Inject @Config
    private Holder<ServiceConnectionConfig> connectionConfig;

    private Optional<String> ipAddressForPsme;
    private Optional<String> ipAddressForRss;
    private Optional<String> ipAddressForRmm;
    private UUID podmUuid;

    @PostConstruct
    public void initialize() {
        serviceTypeMap.put(ServiceType.PSME, PSME);
        serviceTypeMap.put(ServiceType.RSS, RSS);
        serviceTypeMap.put(ServiceType.RMM, RMM);

        ipAddressForPsme = obtainPodManagerIpForReceivingEventsFromServiceOfType(ServiceType.PSME);
        ipAddressForRss = obtainPodManagerIpForReceivingEventsFromServiceOfType(ServiceType.RSS);
        ipAddressForRmm = obtainPodManagerIpForReceivingEventsFromServiceOfType(ServiceType.RMM);
        podmUuid = serviceConfig.get().getUuid();
    }

    public EventServiceDefinitionImpl createEventServiceDefinition(ExternalService service) {
        URI podmEventServiceDestinationUri = getPodmEventServiceDestination(service.getServiceType(), service.getUuid());
        return new EventServiceDefinitionImpl(podmEventServiceDestinationUri, service.getBaseUri(), podmUuid);
    }

    public boolean isSslEnabledForService(String serviceTypeString) {
        ServiceType serviceType = getValidServiceType(serviceTypeString);
        return connectionConfig.get().isSslEnabledForServicesOfType(serviceType);
    }

    public URI getPodmEventServiceDestination(String serviceTypeString, UUID serviceUuid) {
        ServiceType serviceType = getValidServiceType(serviceTypeString);
        return getPodmEventServiceDestination(serviceType, serviceUuid);
    }

    private URI getPodmEventServiceDestination(ServiceType serviceType, UUID serviceUuid) {
        String validServiceName = getValidServiceTypeString(serviceType);
        String ipAddress = getPodManagerIpForReceivingEventsFromServiceOfType(serviceType);

        boolean sslEnabled = connectionConfig.get().isSslEnabledForServicesOfType(serviceType);

        try {
            return fromUri("{scheme}://{ip}:{port}")
                    .resolveTemplate("scheme", sslEnabled ? HTTPS_SCHEME : HTTP_SCHEME)
                    .resolveTemplate("ip", ipAddress)
                    .resolveTemplate("port", sslEnabled ? HTTPS_PODM_PORT : HTTP_PODM_PORT)
                    .path(EVENT_ENDPOINT + "/" + validServiceName + "/" + serviceUuid.toString())
                    .build();
        } catch (IllegalArgumentException | UriBuilderException e) {
            throw new IllegalArgumentException("Couldn't create valid PodM Event Service destination", e);
        }
    }

    private String getPodManagerIpForReceivingEventsFromServiceOfType(ServiceType serviceType) {
        Optional<String> podmManagerIp;
        switch (serviceType) {
            case PSME:
                podmManagerIp = ipAddressForPsme;
                break;
            case RSS:
                podmManagerIp = ipAddressForRss;
                break;
            case RMM:
                podmManagerIp = ipAddressForRmm;
                break;
            default:
                podmManagerIp = empty();
        }

        return podmManagerIp.orElseThrow(() ->
                new IllegalArgumentException(format("Couldn't get PodM IP address for service type '%s'", serviceType)));
    }

    private Optional<String> obtainPodManagerIpForReceivingEventsFromServiceOfType(ServiceType serviceType) {
        Optional<String> ipAddressOfPodFromConfig = getIpAddressOfPodFromConfig(serviceType);

        if (ipAddressOfPodFromConfig.isPresent()) {
            return ipAddressOfPodFromConfig;
        } else {
            String ethernetInterfaceNameForEvents = getEthernetInterfaceNameForEvents(serviceType);
            return ethernetInterfaceNameForEvents == null ? empty() : getIpAddressOfPodFromNetworkInterface(ethernetInterfaceNameForEvents);
        }
    }

    private Optional<String> getIpAddressOfPodFromConfig(ServiceType serviceType) {
        return ofNullable(getPodIpAddressForEvents(serviceType));
    }

    private Optional<String> getIpAddressOfPodFromNetworkInterface(String ethernetInterfaceNameForEvents) {
        try {
            return getIpAddressesForNetworkInterface(ethernetInterfaceNameForEvents)
                    .stream()
                    .findFirst();
        } catch (IllegalArgumentException e) {
            return empty();
        }
    }

    private String getPodIpAddressForEvents(ServiceType serviceType) {
        switch (serviceType) {
            case PSME:
                return eventsConfig.get().getPodManagerIpAddressForEventsFromPsme();
            case RSS:
                return eventsConfig.get().getPodManagerIpAddressForEventsFromRss();
            case RMM:
                return eventsConfig.get().getPodManagerIpAddressForEventsFromRmm();
            default:
                throw new UnsupportedOperationException(format("Events for service type '%s' are not supported", serviceType));
        }
    }

    private String getEthernetInterfaceNameForEvents(ServiceType serviceType) {
        switch (serviceType) {
            case PSME:
                return eventsConfig.get().getNetworkInterfaceNameForEventsFromPsme();
            case RSS:
                return eventsConfig.get().getNetworkInterfaceNameForEventsFromRss();
            case RMM:
                return eventsConfig.get().getNetworkInterfaceNameForEventsFromRmm();
            default:
                throw new UnsupportedOperationException(format("Events for service type '%s' are not supported", serviceType));
        }
    }

    private String getValidServiceTypeString(ServiceType serviceType) {
        return ofNullable(serviceTypeMap.get(serviceType)).orElseThrow(() ->
                        new UnsupportedOperationException(format("Events for service type '%s' are not supported", serviceType))
        );
    }

    private ServiceType getValidServiceType(String serviceTypeString) {
        return ofNullable(serviceTypeMap.inverse().get(serviceTypeString)).orElseThrow(() ->
                        new UnsupportedOperationException(format("Events for service type '%s' are not supported", serviceTypeString))
        );
    }
}
