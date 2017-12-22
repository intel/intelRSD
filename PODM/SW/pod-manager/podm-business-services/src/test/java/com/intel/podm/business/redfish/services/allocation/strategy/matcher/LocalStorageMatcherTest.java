/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.redfish.services.allocation.mappers.localdrive.LocalStorageAllocationMapper;
import com.intel.podm.business.redfish.services.allocation.templates.requestednode.RequestedNodeWithLocalDrives;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.logger.Logger;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.math.BigDecimal;

import static com.intel.podm.business.redfish.services.allocation.templates.assets.LocalDrivesCreation.createAvailableLocalDrive;
import static com.intel.podm.business.redfish.services.allocation.templates.assets.LocalDrivesCreation.createRequestedDrive;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.DRIVE;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.MediaType.HDD;
import static com.intel.podm.common.types.MediaType.SSD;
import static com.intel.podm.common.types.Protocol.NVME;
import static com.intel.podm.common.types.Protocol.SAS;
import static java.util.Collections.singleton;
import static java.util.Collections.singletonList;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName", "checkstyle:MethodCount"})
public class LocalStorageMatcherTest {

    @Mock
    private Logger logger;

    @InjectMocks
    private LocalStorageAllocationMapper localStorageAllocationMapper;

    @BeforeMethod
    public void initMocks() {
        MockitoAnnotations.initMocks(this);
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
            createRequestedDrive(null, null, NVME, null, null, null),
            createAvailableLocalDrive(null, null, NVME, null, null, null)
        );
    }

    @Test
    public void whenExactInterfaceIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
            createRequestedDrive(null, null, NVME, null, null, null),
            createAvailableLocalDrive(null, null, SAS, null, null, null)
        );
    }

    @Test
    public void whenAvailableRpmIsNull_shouldNotMatch() {
        shouldNotMatch(
            createRequestedDrive(null, null, null, new BigDecimal(500), null, null),
            createAvailableLocalDrive(null, null, null, null, null, null)
        );
    }

    @Test
    public void whenAtLeastMinRpmIsNotSatisfied_shouldMatch() {
        shouldMatch(
            createRequestedDrive(null, null, null, new BigDecimal(500), null, null),
            createAvailableLocalDrive(null, null, null, new BigDecimal(501), null, null)
        );
    }

    @Test
    public void whenAtLeastMinRpmIsSatisfied_shouldNotMatch() {
        shouldNotMatch(
            createRequestedDrive(null, null, null, new BigDecimal(501), null, null),
            createAvailableLocalDrive(null, null, null, new BigDecimal(500), null, null)
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
            createRequestedDrive(null, null, null, null, null, contextOf(id(1), DRIVE)),
            createAvailableLocalDrive(null, null, null, null, null, id(1))
        );
    }

    @Test
    public void whenExactIdIsNotSatisfied_shouldNotMatch() {
        shouldNotMatch(
            createRequestedDrive(null, null, null, null, null, contextOf(id(2), DRIVE)),
            createAvailableLocalDrive(null, null, null, null, null, id(1))
        );
    }

    private LocalStorageMatcher createLocalStorageMatcher() {
        LocalStorageMatcher localStorageMatcher = new LocalStorageMatcher();
        localStorageMatcher.mapper = localStorageAllocationMapper;
        return localStorageMatcher;
    }

    private void shouldMatch(RequestedNode.LocalDrive requestedLocalDrive, LocalDrive availableLocalDrive) {
        LocalStorageMatcher localStorageMatcher = createLocalStorageMatcher();

        RequestedNode requestedNode = new RequestedNodeWithLocalDrives(singletonList(requestedLocalDrive));
        assertEquals(localStorageMatcher.matches(requestedNode, singleton(availableLocalDrive)), true);
    }

    private void shouldNotMatch(RequestedNode.LocalDrive requestedLocalDrive, LocalDrive availableLocalDrive) {
        LocalStorageMatcher localStorageMatcher = createLocalStorageMatcher();

        RequestedNode requestedNode = new RequestedNodeWithLocalDrives(singletonList(requestedLocalDrive));
        assertEquals(localStorageMatcher.matches(requestedNode, singleton(availableLocalDrive)), false);
    }
}
