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

package com.intel.podm.discovery.external.finalizers.psme;

import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.redfish.Chassis;
import org.testng.annotations.Test;

import java.util.List;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.ChassisType.RACK;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class DrawerLocationGuardTest {

    @Test
    public void whenDrawerHasMultipleParents_unlinkMustBeCalled() {
        DrawerChassisLocationGuard drawerLocationGuard = new DrawerChassisLocationGuard();

        drawerLocationGuard.chassisDao = mock(ChassisDao.class);
        Chassis drawer = createDrawerMock("1", "1");
        Chassis first = createRackChassisMock(singletonList(drawer), "1");
        Chassis second = createRackChassisMock(singletonList(drawer), "2");
        Chassis third = createRackChassisMock(singletonList(drawer), "3");
        when(drawerLocationGuard.chassisDao.getAllByChassisType(RACK)).thenReturn(newArrayList(first, second, third));

        drawerLocationGuard.assureSingleRackParent(drawer);

        verify(first, never()).unlinkContainedChassis(drawer);
        verify(second, times(1)).unlinkContainedChassis(drawer);
        verify(third, times(1)).unlinkContainedChassis(drawer);
    }

    @Test
    public void whenNeitherDrawersNorManagersAreConnectedWithRack_deleteMustBeCalled() {
        DrawerChassisLocationGuard drawerLocationGuard = new DrawerChassisLocationGuard();

        drawerLocationGuard.chassisDao = mock(ChassisDao.class);
        Chassis drawer = createDrawerMock("1", "1");
        Chassis first = createRackChassisMock(singletonList(drawer), "1");
        Chassis second = createRackChassisMock(emptyList(), "2");
        Chassis third = createRackChassisMock(emptyList(), "3");
        when(drawerLocationGuard.chassisDao.getAllByChassisType(RACK)).thenReturn(newArrayList(first, second, third));

        drawerLocationGuard.assureSingleRackParent(drawer);

        verify(drawerLocationGuard.chassisDao, never()).remove(first);
        verify(drawerLocationGuard.chassisDao, times(1)).remove(second);
        verify(drawerLocationGuard.chassisDao, times(1)).remove(third);
    }

    private Chassis createDrawerMock(String drawerLocationId, String parentLocationId) {
        Chassis drawer = mock(Chassis.class);
        when(drawer.getLocationId()).thenReturn(drawerLocationId);
        when(drawer.getLocationParentId()).thenReturn(parentLocationId);
        when(drawer.getChassisType()).thenReturn(DRAWER);
        when(drawer.is(DRAWER)).thenReturn(true);
        return drawer;
    }

    private Chassis createRackChassisMock(List<Chassis> children, String locationString) {
        Chassis rack = mock(Chassis.class);
        when(rack.getLocationParentId()).thenReturn("Pod1");
        when(rack.getChassisType()).thenReturn(RACK);
        when(rack.getLocationId()).thenReturn(locationString);
        when(rack.getContainedChassis()).thenReturn(children);
        when(rack.is(RACK)).thenReturn(true);
        return rack;
    }
}
