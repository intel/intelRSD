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

package com.intel.rsd.resourcemanager.layers.completer;

import com.google.common.collect.ImmutableList;
import com.google.common.collect.ImmutableSet;
import com.intel.rsd.crawler.Resource;
import lombok.val;
import org.testng.annotations.Test;

import java.util.Set;
import java.util.regex.Pattern;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static java.util.Collections.emptySet;
import static java.util.Collections.singleton;
import static java.util.Collections.singletonList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class CrawledResourcesDistributorTest {

    @Test
    public void whenProvidedWithGenericPattern_thenDistributesAllResources() throws Exception {
        val resourceCompleter = mock(ResourceCompleter.class);
        when(resourceCompleter.interestedInPatterns()).thenReturn(singletonList(Pattern.compile(".*")));
        val sut = new CrawledResourcesDistributor(singleton(resourceCompleter));

        val processorResource = new ResourceImpl(readJsonFromFile(this, "Processor.json"));
        val ethernetInterfaceResource = new ResourceImpl(readJsonFromFile(this, "ComputerSystemEthernetInterface.json"));
        Set<Resource> resources = ImmutableSet.of(processorResource, ethernetInterfaceResource);
        sut.consume(resources);

        verify(resourceCompleter).onResourcesCrawled(resources);
    }

    @Test
    public void whenProvidedWithPatternToComputerSystemEthernetInterface_thenDistributesComputerSystemEthernetInterfaceResourceOnly() throws Exception {
        val resourceCompleter = mock(ResourceCompleter.class);
        val ethernetInterfaceResource = new ResourceImpl(readJsonFromFile(this, "ComputerSystemEthernetInterface.json"));
        when(resourceCompleter.interestedInPatterns()).thenReturn(singletonList(Pattern.compile(ethernetInterfaceResource.get("/@odata.id").asText())));
        val sut = new CrawledResourcesDistributor(singleton(resourceCompleter));

        val processorResource = new ResourceImpl(readJsonFromFile(this, "Processor.json"));

        sut.consume(ImmutableSet.of(processorResource, ethernetInterfaceResource));

        verify(resourceCompleter).onResourcesCrawled(ImmutableSet.of(ethernetInterfaceResource));
    }

    @Test
    public void whenProvidedWithNullPattern_thenDistributesNothing() throws Exception {
        val resourceCompleter = mock(ResourceCompleter.class);
        when(resourceCompleter.interestedInPatterns()).thenReturn(singletonList(null));
        val ethernetInterfaceResource = new ResourceImpl(readJsonFromFile(this, "ComputerSystemEthernetInterface.json"));
        val sut = new CrawledResourcesDistributor(singleton(resourceCompleter));

        sut.consume(singleton(ethernetInterfaceResource));

        verify(resourceCompleter).onResourcesCrawled(emptySet());
    }

    @Test
    public void whenProvidedWithTwoCompletersWithSamePattern_thenDistributesSameResourcesToTwoCompleters() throws Exception {
        val firstResourceCompleter = mock(ResourceCompleter.class);
        val matchAllPattern = Pattern.compile(".*");
        when(firstResourceCompleter.interestedInPatterns()).thenReturn(singletonList(matchAllPattern));
        val secondResourceCompleter = mock(ResourceCompleter.class);
        when(secondResourceCompleter.interestedInPatterns()).thenReturn(singletonList(matchAllPattern));

        val sut = new CrawledResourcesDistributor(ImmutableSet.of(firstResourceCompleter, secondResourceCompleter));
        val anyResource = new ResourceImpl(readJsonFromFile(this, "ComputerSystemEthernetInterface.json"));

        Set<Resource> resourcesToBeDistributed = singleton(anyResource);
        sut.consume(resourcesToBeDistributed);

        verify(firstResourceCompleter).onResourcesCrawled(resourcesToBeDistributed);
        verify(secondResourceCompleter).onResourcesCrawled(resourcesToBeDistributed);
    }

    @Test
    public void whenProvidedWithTwoCompletersWithDifferentPatterns_thenDistributesResourcesProperly() throws Exception {
        val completerInterestedInEthernetInterface = mock(ResourceCompleter.class);
        when(completerInterestedInEthernetInterface.interestedInPatterns()).thenReturn(singletonList(Pattern.compile(".*/EthernetInterfaces/.*")));
        val completerInterestedInSwitchPort = mock(ResourceCompleter.class);
        when(completerInterestedInSwitchPort.interestedInPatterns()).thenReturn(singletonList(Pattern.compile(".*/Ports/.*")));

        val sut = new CrawledResourcesDistributor(ImmutableSet.of(completerInterestedInEthernetInterface, completerInterestedInSwitchPort));
        val ethernetInterfaceResource = new ResourceImpl(readJsonFromFile(this, "ComputerSystemEthernetInterface.json"));
        val switchPortResource = new ResourceImpl(readJsonFromFile(this, "EthernetSwitchPort.json"));

        Set<Resource> resourcesToBeDistributed = ImmutableSet.of(ethernetInterfaceResource, switchPortResource);
        sut.consume(resourcesToBeDistributed);

        verify(completerInterestedInEthernetInterface).onResourcesCrawled(singleton(ethernetInterfaceResource));
        verify(completerInterestedInSwitchPort).onResourcesCrawled(singleton(switchPortResource));
    }

    @Test
    public void whenProvidedWithTwoCompletersWithOverlappingPatterns_thenDistributesResourcesProperly() throws Exception {
        val completerInterestedInEthernetInterfaceAndPort = mock(ResourceCompleter.class);
        Pattern ethernetInterfacePattern = Pattern.compile(".*/EthernetInterfaces/.*");
        Pattern portsPattern = Pattern.compile(".*/Ports/.*");
        when(completerInterestedInEthernetInterfaceAndPort.interestedInPatterns()).thenReturn(ImmutableList.of(ethernetInterfacePattern, portsPattern));
        val completerInterestedInSwitchPort = mock(ResourceCompleter.class);
        when(completerInterestedInSwitchPort.interestedInPatterns()).thenReturn(singletonList(portsPattern));

        val sut = new CrawledResourcesDistributor(ImmutableSet.of(completerInterestedInEthernetInterfaceAndPort, completerInterestedInSwitchPort));
        val ethernetInterfaceResource = new ResourceImpl(readJsonFromFile(this, "ComputerSystemEthernetInterface.json"));
        val switchPortResource = new ResourceImpl(readJsonFromFile(this, "EthernetSwitchPort.json"));

        Set<Resource> resourcesToBeDistributed = ImmutableSet.of(ethernetInterfaceResource, switchPortResource);
        sut.consume(resourcesToBeDistributed);

        verify(completerInterestedInEthernetInterfaceAndPort).onResourcesCrawled(ImmutableSet.of(ethernetInterfaceResource, switchPortResource));
        verify(completerInterestedInSwitchPort).onResourcesCrawled(singleton(switchPortResource));
    }
}
