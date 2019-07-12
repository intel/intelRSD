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

import com.google.common.collect.ImmutableList;
import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration;
import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration.Receiver.DynamicProviderMapping;
import lombok.NonNull;
import lombok.val;
import org.apache.commons.net.util.SubnetUtils;
import org.mockito.Mock;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.Arrays;
import java.util.List;
import java.util.UUID;

import static com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration.Receiver.ReceiverType.Dynamic;
import static java.util.Collections.singletonList;
import static org.assertj.core.api.Java6Assertions.assertThat;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;

@SuppressWarnings("checkstyle:MagicNumber")
public class DynamicSubscriptionDestinationProviderTest {

    @Mock
    private RestCallRouteVerifier routeVerifier;

    @BeforeMethod
    public void setUp() {
        initMocks(this);
    }

    @Test
    public void sourceSubnetMatching_firstMatchingSubnetIsUsed() {
        val firstSubnetMapping = createSubnetMapping("0.0.0.0/0", "192.168.1.1");
        val secondSubnetMapping = createSubnetMapping("192.168.1.1/32", "192.168.1.2");
        val mapping = ImmutableList.of(firstSubnetMapping, secondSubnetMapping);
        val eventsConfiguration = createEventsConfigurationWithDynamicReceiver(8080, "https", mapping);

        when(routeVerifier.isReachable(any())).thenReturn(true);
        val sut = new DynamicSubscriptionDestinationProvider(routeVerifier, eventsConfiguration);
        val result = sut.provideUriFor(URI.create("http://192.168.1.2/redfish/v1"));

        assertThat(result.getHost()).isEqualTo("192.168.1.1");
    }

    @Test
    public void sourceSubnetMatching_secondSubnetMatches_secondSubnetIsUsed() {
        val firstSubnetMapping = createSubnetMapping("10.3.0.0/24", "10.3.0.1");
        val secondSubnetMapping = createSubnetMapping("10.2.0.0/24", "10.2.0.1");
        val mapping = ImmutableList.of(firstSubnetMapping, secondSubnetMapping);
        val eventsConfiguration = createEventsConfigurationWithDynamicReceiver(8080, "https", mapping);
        val sut = new DynamicSubscriptionDestinationProvider(routeVerifier, eventsConfiguration);

        when(routeVerifier.isReachable(any())).thenReturn(true);
        val result = sut.provideUriFor(URI.create("http://10.2.0.233/redfish/v1"));

        assertThat(result.getHost()).isEqualTo("10.2.0.1");
    }

    @Test
    public void whenFirstServiceIsUnavailable_secondIsUsed() {
        val subnetMapping = createSubnetMapping("0.0.0.0/0", "192.168.1.1", "192.168.1.2");
        val eventsConfiguration = createEventsConfigurationWithDynamicReceiver(8080, "https", singletonList(subnetMapping));

        when(routeVerifier.isReachable(eq(URI.create("https://192.168.1.1:8080/redfish/v1")))).thenReturn(false);
        when(routeVerifier.isReachable(eq(URI.create("https://192.168.1.2:8080/redfish/v1")))).thenReturn(true);

        val sut = new DynamicSubscriptionDestinationProvider(routeVerifier, eventsConfiguration);

        val result = sut.provideUriFor(URI.create("http://192.168.1.100/redfish/v1"));

        assertThat(result.getHost()).isEqualTo("192.168.1.2");
    }

    @Test(expectedExceptions = SubscriptionDestinationProviderException.class)
    public void whenAllServicesAreUnavailable_thenExceptionIsThrown() {
        val subnetMapping = createSubnetMapping("0.0.0.0/0", "192.168.1.1", "192.168.1.2");
        val eventsConfiguration = createEventsConfigurationWithDynamicReceiver(8080, "https", singletonList(subnetMapping));

        val sut = new DynamicSubscriptionDestinationProvider(routeVerifier, eventsConfiguration);

        sut.provideUriFor(URI.create("http://192.168.1.100/redfish/v1"));
    }

    @Test(expectedExceptions = SubscriptionDestinationProviderException.class)
    public void whenNoMatchingSubnetIsFound_thenExceptionIsThrown() {
        val subnetMapping = createSubnetMapping("192.168.1.1/32", "192.168.1.1");
        val eventsConfiguration = createEventsConfigurationWithDynamicReceiver(8080, "https", singletonList(subnetMapping));

        val sut = new DynamicSubscriptionDestinationProvider(routeVerifier, eventsConfiguration);

        sut.provideUriFor(URI.create("http://192.168.1.100/redfish/v1"));
    }

    @Test(expectedExceptions = SubscriptionDestinationProviderException.class)
    public void whenNonExistingHostIsUsed_thenExceptionIsThrown() {
        val subnetMapping = createSubnetMapping("192.168.1.1/32", "192.168.1.1");
        val eventsConfiguration = createEventsConfigurationWithDynamicReceiver(8080, "https", singletonList(subnetMapping));

        val sut = new DynamicSubscriptionDestinationProvider(routeVerifier, eventsConfiguration);

        sut.provideUriFor(URI.create("http://" + UUID.randomUUID().toString() + "/redfish/v1"));
    }

    @NonNull
    private DynamicProviderMapping createSubnetMapping(String cidrNotation, String... targetIpAddresses) {
        val firstSubnetMapping = new DynamicProviderMapping();
        firstSubnetMapping.setSourceSubnet(new SubnetUtils(cidrNotation));
        firstSubnetMapping.setTargetIpAddresses(Arrays.asList(targetIpAddresses));
        return firstSubnetMapping;
    }

    @NonNull
    private EventsConfiguration createEventsConfigurationWithDynamicReceiver(int port, String protocol, List<DynamicProviderMapping> mapping) {
        val receiver = new EventsConfiguration.Receiver();
        receiver.setType(Dynamic);
        val dynamicProvider = new EventsConfiguration.Receiver.DynamicProvider();
        dynamicProvider.setTargetPort(port);
        dynamicProvider.setTargetProtocol(protocol);
        dynamicProvider.setMapping(mapping);
        receiver.setDynamic(dynamicProvider);
        val eventsConfiguration = new EventsConfiguration();
        eventsConfiguration.setReceiver(receiver);
        return eventsConfiguration;
    }
}
