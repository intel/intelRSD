/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.runner.events;

import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration;
import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration.Receiver.DynamicProvider;
import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration.Receiver.DynamicProviderMapping;
import com.intel.rsd.resourcemanager.runner.requiredlayer.RestClient;
import com.intel.rsd.resourcemanager.runner.requiredlayer.ServiceAccessException;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty;
import org.springframework.http.HttpHeaders;
import org.springframework.stereotype.Component;
import org.springframework.web.util.UriComponentsBuilder;

import java.net.Inet4Address;
import java.net.URI;
import java.net.UnknownHostException;
import java.util.List;

import static com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration.Receiver.ReceiverType.Dynamic;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static org.springframework.http.HttpMethod.GET;

@ConditionalOnProperty(name = "events.receiver.type", havingValue = "Dynamic")
@Component
@Slf4j
public class DynamicSubscriptionDestinationProvider implements SubscriptionDestinationProvider {
    private final RestClient restClient;
    private final DynamicProvider dynamicProviderConfig;

    @Autowired
    public DynamicSubscriptionDestinationProvider(RestClient restClient, EventsConfiguration eventsConfiguration) {
        this.restClient = restClient;
        this.dynamicProviderConfig = eventsConfiguration.getReceiver().getDynamic();
        log.info("Starting Receiver with {} configuration: {}", Dynamic, eventsConfiguration.getReceiver());
    }

    @Override
    public URI provideUriFor(@NonNull URI serviceUri) {
        val targetHost = getHostAddress(serviceUri);
        val targetIpAddresses = determineTargetIpAddresses(targetHost);

        for (String targetIpAddress : targetIpAddresses) {
            val uriToCall = createServiceRootUri(targetIpAddress);
            try {
                Response response = restClient.call(uriToCall, GET, new HttpHeaders(), null);
                if (response.getHttpStatus().is2xxSuccessful()) {
                    return createUriForHost(targetIpAddress, "");
                } else {
                    String responseBody = response.getBody() != null ? response.getBody().asText() : null;
                    log.warn("{} returned non 2xx status code: {} with body: {}", uriToCall, response.getHttpStatus().value(), responseBody);
                }
            } catch (ServiceAccessException e) {
                log.warn("{} is unavailable due to {}", uriToCall, e.getMessage());
            }
        }

        throw new SubscriptionDestinationProviderException(
            format("Could not provide destination Uri for %s - none of %s addresses is responding", serviceUri, ipAddressesToServiceRootUris(targetIpAddresses))
        );
    }

    private String getHostAddress(URI serviceUri) {
        try {
            return Inet4Address.getByName(serviceUri.getHost()).getHostAddress();
        } catch (UnknownHostException e) {
            throw new SubscriptionDestinationProviderException(
                String.format("Could not resolve host address for service uri %s due to %s", serviceUri, e.getMessage())
            );
        }
    }

    private List<String> determineTargetIpAddresses(String targetHost) {
        return dynamicProviderConfig.getMapping().stream()
            .filter(mapping -> mapping.getSourceSubnet().getInfo().isInRange(targetHost))
            .findFirst()
            .map(DynamicProviderMapping::getTargetIpAddresses)
            .orElseThrow(() -> new SubscriptionDestinationProviderException("Could not find matching subnet for target host: " + targetHost));
    }

    private URI createServiceRootUri(String targetIpAddress) {
        return createUriForHost(targetIpAddress, "/redfish/v1");
    }

    private URI createUriForHost(String targetIpAddress, String path) {
        return UriComponentsBuilder.newInstance()
            .scheme(dynamicProviderConfig.getTargetProtocol())
            .host(targetIpAddress)
            .port(dynamicProviderConfig.getTargetPort())
            .path(path)
            .build().toUri();
    }

    private List<URI> ipAddressesToServiceRootUris(List<String> targetIpAddresses) {
        return targetIpAddresses.stream()
            .map(this::createServiceRootUri)
            .collect(toList());
    }
}
