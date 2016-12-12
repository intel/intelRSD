/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.discovery.external.finalizers.psme;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.types.ChassisType;
import org.testng.annotations.Test;

import java.util.Collection;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.MODULE;
import static com.intel.podm.common.types.ChassisType.RACK;
import static com.intel.podm.common.types.ChassisType.CARD;
import static com.intel.podm.common.types.ChassisType.CARTRIDGE;
import static com.intel.podm.common.types.ChassisType.SLED;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

public class PsmeDiscoveryFinalizerTest {
    private PsmeDiscoveryFinalizer finalizer = new PsmeDiscoveryFinalizer();

    @Test
    public void shouldReturnTopLevelRack_whenThereIsOnlyOneThatSatisfiesNullCondition() {
        ExternalService es = mock(ExternalService.class);
        Chassis rack = create(RACK, null, es);
        Chassis drawer = create(DRAWER, rack, es);
        Chassis module = create(MODULE, drawer, es);

        Collection<Chassis> topLevelChassis = finalizer.getTopLevelChassis(newHashSet(drawer, module, rack));

        assertEquals(topLevelChassis.size(), 1);
        assertTrue(topLevelChassis.iterator().next().is(RACK));
    }

    @Test
    public void shouldReturnEmptyCollection_whenTopLevelChassisIsNeitherRackNorDrawer() {
        ExternalService es = mock(ExternalService.class);
        Chassis card = create(CARD, null, es);
        Chassis module = create(MODULE, card, es);
        Chassis cartridge = create(CARTRIDGE, module, es);

        Collection<Chassis> topLevelChassis = finalizer.getTopLevelChassis(newHashSet(
            card, module, cartridge
        ));

        assertEquals(topLevelChassis.size(), 0);
    }

    @Test
    public void shouldReturnMultipleChassis_whenTheyAreNotContainedByOtherChassis() {
        ExternalService es = mock(ExternalService.class);
        Chassis drawer1 = create(DRAWER, null, es);
        Chassis module1 = create(MODULE, drawer1, es);
        Chassis drawer2 = create(DRAWER, null, es);
        Chassis module2 = create(MODULE, drawer2, es);

        Collection<Chassis> topLevelChassis = finalizer.getTopLevelChassis(newHashSet(drawer1, module1, drawer2, module2));

        assertEquals(topLevelChassis.size(), 2);
        topLevelChassis.forEach(chassis -> assertTrue(chassis.is(DRAWER)));
    }

    @Test
    public void shouldReturnTopLevelChassis_eventThoughItIsLinkedToDomainObjectFromOtherService() {
        ExternalService es = mock(ExternalService.class);
        ExternalService otherEs = mock(ExternalService.class);
        Chassis rack = create(RACK, null, otherEs);
        Chassis drawer = create(DRAWER, rack, es);
        Chassis module = create(MODULE, drawer, es);
        Chassis sled = create(SLED, module, es);

        Collection<Chassis> topLevelChassis = finalizer.getTopLevelChassis(newHashSet(drawer, module, sled));

        assertEquals(topLevelChassis.size(), 1);
        assertTrue(topLevelChassis.iterator().next().is(DRAWER));
    }

    private Chassis create(ChassisType chassisType, Chassis parentChassis, ExternalService externalService) {
        Chassis mock = mock(Chassis.class);
        when(mock.getChassisType()).thenReturn(chassisType);
        when(mock.getContainedBy()).thenReturn(parentChassis);
        when(mock.getService()).thenReturn(externalService);
        when(mock.is(chassisType)).thenReturn(true);

        return mock;
    }
}
