/*
 * Copyright (c) 2016-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version . (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalLink;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.types.ChassisType;
import org.mockito.internal.util.reflection.Whitebox;
import org.testng.annotations.Test;

import java.util.Collection;
import java.util.HashSet;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.MODULE;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.types.ChassisType.SLED;
import static java.util.stream.Collectors.toList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName"})
public class ChassisHierarchyMaintainerTest {

    private ChassisHierarchyMaintainer hierarchyMaintainer = new ChassisHierarchyMaintainer();

    @Test
    public void shouldReturnTopLevelRack_whenThereIsOnlyOneWithNullParent() {
        ExternalLink externalLink = createExternalLink(2);
        Chassis rack = create(RACK, null, externalLink);
        Chassis drawer = create(DRAWER, rack, externalLink);
        Chassis module = create(MODULE, drawer, externalLink);

        Collection<Chassis> topLevelChassis = hierarchyMaintainer.getTopLevelChassis(newHashSet(drawer, rack, module));

        assertEquals(topLevelChassis.size(), 1);
        assertTrue(topLevelChassis.iterator().next().is(RACK));
    }

    @Test
    public void shouldReturnMultipleChassis_whenTheyAreNotContainedByOtherChassis() {
        ExternalLink chassisOnPsme = createExternalLink(2);
        Chassis drawer1 = create(DRAWER, null, chassisOnPsme);
        Chassis module1 = create(MODULE, drawer1, chassisOnPsme);
        Chassis drawer2 = create(DRAWER, null, chassisOnPsme);
        Chassis module2 = create(MODULE, drawer2, chassisOnPsme);

        Collection<Chassis> topLevelChassis = hierarchyMaintainer.getTopLevelChassis(newHashSet(module1, module2, drawer2, drawer1));

        assertEquals(topLevelChassis.size(), 2);
        topLevelChassis.forEach(chassis -> assertTrue(chassis.is(DRAWER)));
    }

    @Test
    public void shouldReturnTopLevelChassis_evenThoughtItIsLinkedToEntityFromOtherService() {
        ExternalLink rackOnRmm = createExternalLink(1);
        ExternalLink chassisOnPsme = createExternalLink(2);

        Chassis rack = create(RACK, null, rackOnRmm);
        Chassis drawer = create(DRAWER, rack, chassisOnPsme);
        Chassis module = create(MODULE, drawer, chassisOnPsme);
        Chassis sled = create(SLED, module, chassisOnPsme);

        Collection<Chassis> topLevelChassis = hierarchyMaintainer.getTopLevelChassis(newHashSet(drawer, module, sled));

        assertEquals(topLevelChassis.size(), 1);
        assertTrue(topLevelChassis.iterator().next().is(DRAWER));
    }

    private ExternalLink createExternalLink(long id) {
        ExternalService externalService = new ExternalService();
        Whitebox.setInternalState(externalService, "id", id);

        ExternalLink externalLink = new ExternalLink();
        externalLink.setExternalService(externalService);
        return externalLink;
    }

    private Chassis create(ChassisType chassisType, Chassis parentChassis, ExternalLink... links) {
        HashSet<ExternalLink> externalLinks = newHashSet(links);
        Chassis mock = mock(Chassis.class);
        when(mock.getChassisType()).thenReturn(chassisType);
        when(mock.is(chassisType)).thenReturn(true);
        when(mock.getExternalLinks()).thenReturn(externalLinks);
        when(mock.getContainedByChassis()).thenReturn(parentChassis);
        for (ExternalLink externalLink : links) {
            externalLink.setDiscoverableEntity(mock);
        }
        when(mock.getExternalServices()).thenReturn(externalLinks.stream().map(el -> el.getExternalService()).collect(toList()));
        return mock;
    }
}
