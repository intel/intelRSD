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

package com.intel.podm.allocation.strategy.matcher;

import com.intel.podm.allocation.mappers.ethernetinterface.EthernetInterfacesAllocationMapper;
import com.intel.podm.business.dto.redfish.RequestedEthernetInterface;
import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.templates.requestednode.RequestedNodeWithEthernetInterfaces;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.common.types.Id.id;
import static java.util.Arrays.asList;
import static java.util.Collections.singletonList;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class EthernetInterfaceMatcherTest {
    @InjectMocks
    private EthernetInterfacesAllocationMapper ethernetInterfacesAllocationMapper;

    @Mock
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Mock
    private Logger logger;

    @BeforeMethod
    public void initMocks() {
        MockitoAnnotations.initMocks(this);
    }

    @Test
    public void whenMatchingRequestWithoutVlansWithNotLinkedEthernetInterfaces_shouldMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithNotLinkedInterfaces(100, 1000);
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                singletonList(
                        createRequestedEthernetInterfaceWithoutVlans(100)
                )
        );

        assertTrue(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenMatchingRequestWithoutVlansWithNotLinkedEthernetInterfacesButWithDifferentSpeeds_shouldNotMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithNotLinkedInterfaces(100, 1000);
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                singletonList(
                        createRequestedEthernetInterfaceWithoutVlans(1001)
                )
        );

        assertFalse(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenMatchingTwoRequestedWithSpeedsDifferentButSmallerThanAvailable_shouldMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithNotLinkedInterfaces(100, 1000);
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                asList(
                        createRequestedEthernetInterfaceWithoutVlans(100),
                        createRequestedEthernetInterfaceWithoutVlans(200)
                )
        );

        assertTrue(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenMatchingRequestWithVlansWithNotLinkedEthernetInterfaces_shouldNotMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithNotLinkedInterfaces(100, 1000);
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                singletonList(
                        createRequestedEthernetInterface(
                                100, createRequestedVlan(true, 1)
                        )
                )
        );

        assertFalse(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenMatchingRequestWithVlansWithLinkedEthernetInterfaces_shouldMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithLinkedInterfaces(100, 1000);
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                singletonList(
                        createRequestedEthernetInterface(
                                100, createRequestedVlan(true, 1)
                        )
                )
        );

        assertTrue(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenMatchingRequestWithVlansWithLinkedEthernetInterfacesWithoutSpeeds_shouldNotMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithLinkedInterfaces(null, null);
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                singletonList(
                        createRequestedEthernetInterface(
                                100, createRequestedVlan(true, 1)
                        )
                )
        );

        assertFalse(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenMatchingRequestWithoutSpeedsWithLinkedEthernetInterfaces_shouldMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithLinkedInterfaces(100, 1000);
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                asList(
                        createRequestedEthernetInterface(
                                null, createRequestedVlan(true, 1)
                        ),
                        createRequestedEthernetInterface(
                                null, createRequestedVlan(true, 1)
                        )
                )
        );

        assertTrue(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenRequestingMoreResourcesThanAvailable_shouldNotMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithLinkedInterfaces(100, 1000);
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                asList(
                        createRequestedEthernetInterface(
                                10, createRequestedVlan(true, 1)
                        ),
                        createRequestedEthernetInterface(
                                20, createRequestedVlan(true, 1)
                        ),
                        createRequestedEthernetInterface(
                                30, createRequestedVlan(true, 1)
                        )
                )
        );

        assertFalse(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenRequestingResourcesWithUndefinedSpeeds_shouldNotMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithLinkedInterfaces(100, 1000);
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                asList(
                        createRequestedEthernetInterface(
                                null, createRequestedVlan(true, 1)
                        ),
                        createRequestedEthernetInterface(
                                null, createRequestedVlan(true, 1)
                        )
                )
        );

        assertTrue(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenExactIdIsSatisfied_shouldMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithLinkedInterfaces(null, id(1), null, id(2));
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                asList(
                        createRequestedEthernetInterface(
                                null, contextOf(id(2), ETHERNET_INTERFACE), createRequestedVlan(true, 1)
                        ),
                        createRequestedEthernetInterface(
                                null, contextOf(id(1), ETHERNET_INTERFACE), createRequestedVlan(true, 1)
                        )
                )
        );

        assertTrue(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenExactIdIsNotSatisfied_shouldNotMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithLinkedInterfaces(null, id(1), null, id(2));
        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                asList(
                        createRequestedEthernetInterface(
                                null, contextOf(id(2), ETHERNET_INTERFACE), createRequestedVlan(true, 1)
                        ),
                        createRequestedEthernetInterface(
                                null, contextOf(id(3), ETHERNET_INTERFACE), createRequestedVlan(true, 1)
                        )
                )
        );

        assertFalse(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    @Test
    public void whenEthernetInterfaceConnectedWithSwitchPortWithNotUniqueMacIsUsed_shouldNotMatch() throws NonUniqueResultException {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = createEthernetInterfaceMatcher();

        ComputerSystem computerSystem = createComputerSystemWithEthernetInterfaceConnectedWithSwitchPortWithNotUniqueMac(100, id(1));

        RequestedNodeWithEthernetInterfaces requestedNode = new RequestedNodeWithEthernetInterfaces(
                singletonList(
                        createRequestedEthernetInterface(
                                null, contextOf(id(1), ETHERNET_INTERFACE), createRequestedVlan(true, 1)
                        )
                )
        );

        assertFalse(ethernetInterfaceMatcher.matches(requestedNode, computerSystem.getEthernetInterfaces()));
    }

    private EthernetInterfaceMatcher createEthernetInterfaceMatcher() {
        EthernetInterfaceMatcher ethernetInterfaceMatcher = new EthernetInterfaceMatcher();
        ethernetInterfaceMatcher.ethernetInterfacesAllocationMapper = ethernetInterfacesAllocationMapper;
        return ethernetInterfaceMatcher;
    }

    private ComputerSystem createComputerSystemWithLinkedInterfaces(Integer firstInterfaceSpeed, Integer secondInterfaceSpeed)
            throws NonUniqueResultException {
        return createComputerSystemWithLinkedInterfaces(firstInterfaceSpeed, null, secondInterfaceSpeed, null);
    }

    private ComputerSystem createComputerSystemWithLinkedInterfaces(Integer firstInterfaceSpeed, Id firstId, Integer secondInterfaceSpeed, Id secondId)
            throws NonUniqueResultException {
        EthernetInterface ethernetInterfaceMock1 = createLinkedEthernetInterface(firstInterfaceSpeed, firstId);
        EthernetInterface ethernetInterfaceMock2 = createLinkedEthernetInterface(secondInterfaceSpeed, secondId);

        return createComputerSystemWithEthernetInterfaces(ethernetInterfaceMock1, ethernetInterfaceMock2);
    }

    private ComputerSystem createComputerSystemWithNotLinkedInterfaces(Integer firstInterfaceSpeed, Integer secondInterfaceSpeed)
            throws NonUniqueResultException {
        return createComputerSystemWithNotLinkedInterfaces(firstInterfaceSpeed, null, secondInterfaceSpeed, null);
    }

    private ComputerSystem createComputerSystemWithEthernetInterfaceConnectedWithSwitchPortWithNotUniqueMac(Integer interfaceSpeed, Id id)
            throws NonUniqueResultException {
        EthernetInterface ethernetInterfaceMock = createEthernetInterfaceConnectedWithSwitchPortWithNotUniqueMac(interfaceSpeed, id);

        return createComputerSystemWithEthernetInterfaces(ethernetInterfaceMock);
    }

    private ComputerSystem createComputerSystemWithNotLinkedInterfaces(Integer firstInterfaceSpeed, Id firstId, Integer secondInterfaceSpeed, Id secondId)
            throws NonUniqueResultException {
        EthernetInterface ethernetInterfaceMock1 = createNotLinkedEthernetInterface(firstInterfaceSpeed, firstId);
        EthernetInterface ethernetInterfaceMock2 = createNotLinkedEthernetInterface(secondInterfaceSpeed, secondId);

        return createComputerSystemWithEthernetInterfaces(ethernetInterfaceMock1, ethernetInterfaceMock2);
    }

    private ComputerSystem createComputerSystemWithEthernetInterfaces(EthernetInterface... args) {
        ComputerSystem computerSystemMock = mock(ComputerSystem.class);
        when(computerSystemMock.getEthernetInterfaces()).thenReturn(asList(args));
        return computerSystemMock;
    }

    private EthernetInterface createNotLinkedEthernetInterface(Integer interfaceSpeed, Id id) throws NonUniqueResultException {
        return createEthernetInterface(interfaceSpeed, id, false);
    }

    private EthernetInterface createLinkedEthernetInterface(Integer interfaceSpeed, Id id) throws NonUniqueResultException {
        return createEthernetInterface(interfaceSpeed, id, true);
    }

    private EthernetInterface createEthernetInterface(Integer interfaceSpeed, Id id, boolean isLinked) throws NonUniqueResultException {
        EthernetInterface ethernetInterfaceMock = mock(EthernetInterface.class);
        EthernetSwitchPort associatedPort = isLinked ? mock(EthernetSwitchPort.class) : null;

        when(ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(any())).thenReturn(associatedPort);
        when(ethernetInterfaceMock.getSpeedMbps()).thenReturn(interfaceSpeed);
        when(ethernetInterfaceMock.getId()).thenReturn(id);

        return ethernetInterfaceMock;
    }

    private EthernetInterface createEthernetInterfaceConnectedWithSwitchPortWithNotUniqueMac(Integer interfaceSpeed, Id id) throws NonUniqueResultException {
        EthernetInterface ethernetInterfaceMock = mock(EthernetInterface.class);

        when(ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(any())).thenThrow(new NonUniqueResultException(
                "Couldn't find single EthernetSwitchPort with neighbor MAC Address: 'whatever'. Found '2' EthernetSwitchPorts."));
        when(ethernetInterfaceMock.getSpeedMbps()).thenReturn(interfaceSpeed);
        when(ethernetInterfaceMock.getId()).thenReturn(id);

        return ethernetInterfaceMock;
    }

    private RequestedEthernetInterface createRequestedEthernetInterfaceWithoutVlans(Integer interfaceSpeed) {
        return createRequestedEthernetInterfaceWithoutVlans(interfaceSpeed, null);
    }

    private RequestedEthernetInterface createRequestedEthernetInterfaceWithoutVlans(Integer interfaceSpeed, Context context) {
        return createRequestedEthernetInterface(interfaceSpeed, context, (RequestedEthernetInterface.Vlan[]) null);
    }

    private RequestedEthernetInterface createRequestedEthernetInterface(Integer interfaceSpeed, RequestedEthernetInterface.Vlan... vlans) {
        return createRequestedEthernetInterface(interfaceSpeed, null, vlans);
    }

    private RequestedEthernetInterface createRequestedEthernetInterface(Integer interfaceSpeed, Context context, RequestedEthernetInterface.Vlan... vlans) {
        RequestedEthernetInterface requestedEthernetInterfaceMock = mock(RequestedEthernetInterface.class);

        when(requestedEthernetInterfaceMock.getSpeedMbps()).thenReturn(interfaceSpeed);
        when(requestedEthernetInterfaceMock.getVlans()).thenReturn(vlans == null ? empty() : ofNullable(asList(vlans)));
        when(requestedEthernetInterfaceMock.getResourceContext()).thenReturn(context);

        return requestedEthernetInterfaceMock;
    }

    private RequestedEthernetInterface.Vlan createRequestedVlan(boolean isTagged, int vlanId) {
        RequestedEthernetInterface.Vlan vlanMock = mock(RequestedEthernetInterface.Vlan.class);

        when(vlanMock.getVlanId()).thenReturn(vlanId);
        when(vlanMock.isTagged()).thenReturn(isTagged);

        return vlanMock;
    }
}
