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

package com.intel.podm.allocation.mappers.ethernetinterface;

import com.intel.podm.business.dto.redfish.RequestedEthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;
import org.testng.annotations.Test;

import java.util.List;
import java.util.Map;
import java.util.Optional;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.common.types.Id.id;
import static java.util.Collections.singletonList;
import static java.util.Optional.empty;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

public class EthernetInterfacesAllocationMapperTest {

    @Test
    public void whenMappingSingleRequestedInterfaceAndSingleAvailable_shouldMapAvailableToRequested() throws Exception {
        EthernetInterfacesAllocationMapper mapper = new EthernetInterfacesAllocationMapper();
        EthernetInterface availableSlower = mockEthernetInterface(1);
        RequestedEthernetInterface requestedSlower = new RequestedEthernetInterfaceImpl(1);
        List<EthernetInterface> available = singletonList(availableSlower);
        List<RequestedEthernetInterface> requestedEthernetInterfaces = singletonList(requestedSlower);

        Map<EthernetInterface, RequestedEthernetInterface> map = mapper.map(available, requestedEthernetInterfaces);

        assertEquals(map.get(availableSlower), requestedSlower);
    }

    @Test
    public void whenMappingSingleRequestedInterfaceAndMultipleAvailable_shouldMapToFasterOne() throws Exception {
        EthernetInterfacesAllocationMapper mapper = new EthernetInterfacesAllocationMapper();
        EthernetInterface availableSlower = mockEthernetInterface(1);
        EthernetInterface availableFaster = mockEthernetInterface(100);
        RequestedEthernetInterface requestedFaster = new RequestedEthernetInterfaceImpl(100);
        List<EthernetInterface> available = newArrayList(availableSlower, availableFaster);
        List<RequestedEthernetInterface> requestedEthernetInterfaces = newArrayList(requestedFaster);

        Map<EthernetInterface, RequestedEthernetInterface> map = mapper.map(available, requestedEthernetInterfaces);

        assertEquals(map.get(availableFaster), requestedFaster);
    }

    @Test
    public void whenMappingMultipleRequestedWithMultipleAvailable_shouldMapBySpeedRespectively() throws Exception {
        EthernetInterfacesAllocationMapper mapper = new EthernetInterfacesAllocationMapper();
        EthernetInterface availableSlower = mockEthernetInterface(1);
        EthernetInterface availableFaster = mockEthernetInterface(100);
        RequestedEthernetInterface requestedSlower = new RequestedEthernetInterfaceImpl(1);
        RequestedEthernetInterface requestedFaster = new RequestedEthernetInterfaceImpl(100);
        List<EthernetInterface> available = newArrayList(availableSlower, availableFaster);
        List<RequestedEthernetInterface> requestedEthernetInterfaces = newArrayList(requestedSlower, requestedFaster);

        Map<EthernetInterface, RequestedEthernetInterface> map = mapper.map(available, requestedEthernetInterfaces);

        assertEquals(map.get(availableSlower), requestedSlower);
        assertEquals(map.get(availableFaster), requestedFaster);
    }

    @Test
    public void whenMappingMultipleRequestedWithMultipleAvailable_shouldMapByIdRespectively() throws Exception {
        EthernetInterfacesAllocationMapper mapper = new EthernetInterfacesAllocationMapper();
        EthernetInterface availableSlower = mockEthernetInterface(1, id(1));
        EthernetInterface availableFaster = mockEthernetInterface(100, id(2));
        RequestedEthernetInterface requestedSlower = new RequestedEthernetInterfaceImpl(1, contextOf(id(2), ETHERNET_INTERFACE));
        List<EthernetInterface> available = newArrayList(availableSlower, availableFaster);
        List<RequestedEthernetInterface> requestedEthernetInterfaces = newArrayList(requestedSlower);

        Map<EthernetInterface, RequestedEthernetInterface> map = mapper.map(available, requestedEthernetInterfaces);

        assertEquals(map.get(availableFaster), requestedSlower);
    }

    private EthernetInterface mockEthernetInterface(Integer speedMbps) {
        return mockEthernetInterface(speedMbps, null);
    }

    private EthernetInterface mockEthernetInterface(Integer speedMbps, Id resourceId) {
        EthernetInterface ethernetInterface = mock(EthernetInterface.class);
        when(ethernetInterface.getSpeedMbps()).thenReturn(speedMbps);
        when(ethernetInterface.getId()).thenReturn(resourceId);
        return ethernetInterface;
    }

    static class RequestedEthernetInterfaceImpl implements RequestedEthernetInterface {
        final Integer speedMbps;
        final Context resourceContext;

        RequestedEthernetInterfaceImpl(Integer speedMbps) {
            this(speedMbps, null);
        }

        RequestedEthernetInterfaceImpl(Integer speedMbps, Context resourceContext) {
            this.speedMbps = speedMbps;
            this.resourceContext = resourceContext;
        }

        @Override
        public Integer getSpeedMbps() {
            return speedMbps;
        }

        @Override
        public Integer getPrimaryVlan() {
            return null;
        }

        @Override
        public Optional<List<Vlan>> getVlans() {
            return empty();
        }

        @Override
        public Context getResourceContext() {
            return resourceContext;
        }

        @Override
        public Context getChassisContext() {
            return null;
        }
    }

}
