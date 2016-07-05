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

import com.intel.podm.business.dto.redfish.RequestedLocalDrive;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.entities.redfish.Adapter;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Device;
import com.intel.podm.templates.requestednode.RequestedNodeWithLocalDrives;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.math.BigDecimal;
import java.util.List;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.DEVICE;
import static com.intel.podm.common.types.DriveType.HDD;
import static com.intel.podm.common.types.DriveType.SSD;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.StorageControllerInterface.PCIE;
import static com.intel.podm.common.types.StorageControllerInterface.SAS;
import static com.intel.podm.templates.assets.LocalDrivesCreation.createAvailableLocalDrive;
import static com.intel.podm.templates.assets.LocalDrivesCreation.createRequestedDrive;
import static java.util.Collections.singletonList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

public class LocalStorageMatcherTest {
    private LocalStorageMatcher matcher;

    @BeforeClass
    public void setup() {
        this.matcher = new LocalStorageMatcher();
    }

    @Test
    public void whenExactDriveTypeIsSatisfied_shouldMatch() {
        shouldMatch(
                createRequestedDrive(HDD, null, null, null, null, null),
                createAvailableLocalDrive(HDD, null, null, null, null, null)
        );
    }

    @Test
    public void whenExactDriveTypeIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
                createRequestedDrive(HDD, null, null, null, null, null),
                createAvailableLocalDrive(SSD, null, null, null, null, null)
        );
    }

    @Test
    public void whenAvailableCapacityGibIsNull_shouldNotMatch() {
        shouldNotMatch(
                createRequestedDrive(null, 500, null, null, null, null),
                createAvailableLocalDrive(null, null, null, null, null, null)
        );
    }

    @Test
    public void whenAtLeastCapacityGibIsSatisfied_shouldMatch() {
        shouldMatch(
                createRequestedDrive(null, 500, null, null, null, null),
                createAvailableLocalDrive(null, new BigDecimal(501), null, null, null, null)
        );
    }

    @Test
    public void whenAtLeastCapacityGibIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
                createRequestedDrive(null, 501, null, null, null, null),
                createAvailableLocalDrive(null, new BigDecimal(500), null, null, null, null)
        );
    }

    @Test
    public void whenExactInterfaceIsSatisfied_shouldMatch() {
        shouldMatch(
                createRequestedDrive(null, null, PCIE, null, null, null),
                createAvailableLocalDrive(null, null, PCIE, null, null, null)
        );
    }

    @Test
    public void whenExactInterfaceIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
                createRequestedDrive(null, null, PCIE, null, null, null),
                createAvailableLocalDrive(null, null, SAS, null, null, null)
        );
    }

    @Test
    public void whenAvailableRpmIsNull_shouldNotMatch() {
        shouldNotMatch(
                createRequestedDrive(null, null, null, 500, null, null),
                createAvailableLocalDrive(null, null, null, null, null, null)
        );
    }

    @Test
    public void whenAtLeastMinRpmIsNotSatisfied_shouldMatch() {
        shouldMatch(
                createRequestedDrive(null, null, null, 500, null, null),
                createAvailableLocalDrive(null, null, null, 501, null, null)
        );
    }

    @Test
    public void whenAtLeastMinRpmIsSatisfied_shouldNotMatch() {
        shouldNotMatch(
                createRequestedDrive(null, null, null, 501, null, null),
                createAvailableLocalDrive(null, null, null, 500, null, null)
        );
    }

    @Test
    public void whenExactSerialIsSatisfied_shouldMatch() {
        shouldMatch(
                createRequestedDrive(null, null, null, null, "AnySerialNumber", null),
                createAvailableLocalDrive(null, null, null, null, "AnySerialNumber", null)
        );
    }

    @Test
    public void whenExactSerialIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
                createRequestedDrive(null, null, null, null, "AnySerialNumber", null),
                createAvailableLocalDrive(null, null, null, null, "SomeSerialNumber", null)
        );
    }

    @Test
    public void whenExactIdIsSatisfied_shouldMatch() {
        shouldMatch(
                createRequestedDrive(null, null, null, null, null, contextOf(id(1), DEVICE)),
                createAvailableLocalDrive(null, null, null, null, null, id(1))
        );
    }

    @Test
    public void whenExactIdIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
                createRequestedDrive(null, null, null, null, null, contextOf(id(2), DEVICE)),
                createAvailableLocalDrive(null, null, null, null, null, id(1))
        );
    }

    private void shouldMatch(RequestedLocalDrive requestedLocalDrive, Device availableLocalDrive) {
        RequestedNode requestedNode = new RequestedNodeWithLocalDrives(singletonList(requestedLocalDrive));
        ComputerSystem computerSystemWithDrives = createComputerSystemWithDrives(singletonList(availableLocalDrive));
        assertEquals(matcher.matches(requestedNode, computerSystemWithDrives), true);
    }

    private void shouldNotMatch(RequestedLocalDrive requestedLocalDrive, Device availableLocalDrive) {
        RequestedNode requestedNode = new RequestedNodeWithLocalDrives(singletonList(requestedLocalDrive));
        ComputerSystem computerSystemWithDrives = createComputerSystemWithDrives(singletonList(availableLocalDrive));
        assertEquals(matcher.matches(requestedNode, computerSystemWithDrives), false);
    }

    private static ComputerSystem createComputerSystemWithDrives(List<Device> drives) {
        ComputerSystem computerSystemMock = mock(ComputerSystem.class);
        Adapter adapterMock = mock(Adapter.class);

        when(adapterMock.getDevices()).thenReturn(drives);
        when(computerSystemMock.getAdapters()).thenReturn(singletonList(adapterMock));

        return computerSystemMock;
    }
}
