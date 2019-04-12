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

package com.intel.rsd.service.detector.ssdp.transport.impl;

import com.google.common.collect.ImmutableList;
import com.intel.rsd.service.detector.config.SsdpConfig;
import lombok.val;
import org.apache.commons.net.util.SubnetUtils;
import org.testng.annotations.Test;

import java.util.List;

import static java.util.Arrays.asList;
import static java.util.Collections.singletonList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class FilteredDatagramProcessorImplTest {

    @Test
    public void anyAddressShouldNotBeWhiteListedWhenNoConfigurationIsGiven() {
        val ssdpConfig = prepareSsdpConfig(noExclusions(), noKnownSubnets());

        val sut = new FilteredDatagramProcessorImpl(ssdpConfig);
        assertFalse(sut.isWhiteListed("1.1.1.1"));
    }

    @Test
    public void anyAddressShouldBeWhiteListedWhenThereAreNoExclusionsAndKnownSubnetsCoverWholeAvailableRange() {
        val wholeRangeSubnet = singletonList(new SubnetUtils("0.0.0.0/0"));
        val ssdpConfig = prepareSsdpConfig(noExclusions(), wholeRangeSubnet);

        val sut = new FilteredDatagramProcessorImpl(ssdpConfig);
        assertTrue(sut.isWhiteListed("1.1.1.1"));
    }

    @Test
    public void givenAddressShouldNotBeWhiteListedWhenItIsExcluded() {
        val wholeRangeSubnet = singletonList(new SubnetUtils("0.0.0.0/0"));
        val exclusions = singletonList(new SubnetUtils("1.1.1.1/32"));
        val ssdpConfig = prepareSsdpConfig(exclusions, wholeRangeSubnet);

        val sut = new FilteredDatagramProcessorImpl(ssdpConfig);
        assertFalse(sut.isWhiteListed("1.1.1.1"));
    }

    @Test
    public void givenAddressShouldBeWhiteListedWhenItIsNotExcluded() {
        val wholeRangeSubnet = singletonList(new SubnetUtils("0.0.0.0/0"));
        val exclusions = singletonList(new SubnetUtils("1.1.1.1/32"));
        val ssdpConfig = prepareSsdpConfig(exclusions, wholeRangeSubnet);

        val sut = new FilteredDatagramProcessorImpl(ssdpConfig);
        assertTrue(sut.isWhiteListed("1.1.1.2"));
    }

    @Test
    public void givenAddressShouldBeWhiteListedWhenItIsWithingOfDefinedKnownSubnetRange() {
        //subnet below covers range: 1.1.1.1-1.1.1.254
        val knownSubnets = singletonList(new SubnetUtils("1.1.1.1/24"));
        val ssdpConfig = prepareSsdpConfig(noExclusions(), knownSubnets);

        val sut = new FilteredDatagramProcessorImpl(ssdpConfig);
        assertTrue(sut.isWhiteListed("1.1.1.2"));
    }

    @Test
    public void givenAddressShouldNotBeWhiteListedWhenItIsNotWithingOfDefinedKnownSubnetRange() {
        //subnet below covers range: 1.1.1.1-1.1.1.254
        val knownSubnets = singletonList(new SubnetUtils("1.1.1.1/24"));
        val ssdpConfig = prepareSsdpConfig(noExclusions(), knownSubnets);

        val sut = new FilteredDatagramProcessorImpl(ssdpConfig);
        assertFalse(sut.isWhiteListed("2.2.2.2"));
    }

    @Test
    public void eachAddressRelatedWithKnownSubnetsShouldBeWhitelisted() {
        //subnets below covers range: 1.1.1.1-1.1.1.254 and 2.2.2.2-2.2.2.254
        val knownSubnets = asList(new SubnetUtils("1.1.1.1/24"), new SubnetUtils("2.2.2.2/24"));
        val ssdpConfig = prepareSsdpConfig(noExclusions(), knownSubnets);

        val sut = new FilteredDatagramProcessorImpl(ssdpConfig);
        assertTrue(sut.isWhiteListed("1.1.1.1"));
        assertTrue(sut.isWhiteListed("2.2.2.2"));
    }

    private List<SubnetUtils> noExclusions() {
        return ImmutableList.of();
    }

    private List<SubnetUtils> noKnownSubnets() {
        return ImmutableList.of();
    }

    private SsdpConfig prepareSsdpConfig(List<SubnetUtils> exclusions, List<SubnetUtils> subnets) {
        val ssdpConfig = mock(SsdpConfig.class);
        when(ssdpConfig.getSubnets()).thenReturn(subnets);
        when(ssdpConfig.getExclusions()).thenReturn(exclusions);
        return ssdpConfig;
    }
}
