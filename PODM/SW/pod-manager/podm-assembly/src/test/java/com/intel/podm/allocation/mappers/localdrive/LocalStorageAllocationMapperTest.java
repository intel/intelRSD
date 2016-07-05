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

package com.intel.podm.allocation.mappers.localdrive;

import com.intel.podm.business.dto.redfish.RequestedLocalDrive;
import com.intel.podm.business.entities.redfish.base.LocalStorage;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Map;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.DEVICE;
import static com.intel.podm.common.types.DriveType.HDD;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.StorageControllerInterface.SAS;
import static com.intel.podm.templates.assets.LocalDrivesCreation.createAvailableLocalDrive;
import static com.intel.podm.templates.assets.LocalDrivesCreation.createAvailableSimpleStorage;
import static com.intel.podm.templates.assets.LocalDrivesCreation.createRequestedDrive;
import static org.testng.Assert.assertEquals;

public class LocalStorageAllocationMapperTest {
    private LocalStorageAllocationMapper mapper;

    @BeforeClass
    public void setup() {
        this.mapper = new LocalStorageAllocationMapper();
    }

    @Test
    public void whenMappingSingleRequestedWithMultipleAvailable_shouldMapToMinimalOne() {
        ArrayList<RequestedLocalDrive> requestedLocalDrives = newArrayList(
                createRequestedDrive(HDD, 500, SAS, 200, "ASDFQWER", null)
        );

        ArrayList<LocalStorage> availableLocalDrives = newArrayList(
                createAvailableLocalDrive(HDD, new BigDecimal(502), SAS, 200, "ASDFQWER", null),
                createAvailableLocalDrive(HDD, new BigDecimal(501), SAS, 200, "ASDFQWER", null)
        );

        Map<RequestedLocalDrive, LocalStorage> mapped = mapper.map(requestedLocalDrives, availableLocalDrives);

        assertEquals(mapped.size(), 1);
        assertEquals(mapped.get(requestedLocalDrives.get(0)).getCapacityGib(), BigDecimal.valueOf(501));
    }

    @Test
    public void whenMappingMultipleRequestedWithMultipleAvailable_shouldMapToMinimalSubset() {
        ArrayList<RequestedLocalDrive> requestedLocalDrives = newArrayList(
                createRequestedDrive(HDD, 500, SAS, 200, "ASDFQWER", contextOf(id(3), DEVICE)),
                createRequestedDrive(HDD, 500, SAS, 200, "ASDFQWER", null)
        );

        ArrayList<LocalStorage> availableLocalDrives = newArrayList(
                createAvailableLocalDrive(HDD, new BigDecimal(502), SAS, 200, "ASDFQWER", id(1)),
                createAvailableLocalDrive(HDD, new BigDecimal(503), SAS, 200, "ASDFQWER", id(2)),
                createAvailableLocalDrive(HDD, new BigDecimal(504), SAS, 200, "ASDFQWER", id(3)),
                createAvailableLocalDrive(HDD, new BigDecimal(501), SAS, 200, "ASDFQWER", id(4))
        );

        Map<RequestedLocalDrive, LocalStorage> mapped = mapper.map(requestedLocalDrives, availableLocalDrives);

        assertEquals(mapped.size(), 2);
        assertEquals(mapped.get(requestedLocalDrives.get(0)).getCapacityGib(), BigDecimal.valueOf(504));
        assertEquals(mapped.get(requestedLocalDrives.get(0)).getId(), id(3));

        assertEquals(mapped.get(requestedLocalDrives.get(1)).getCapacityGib(), BigDecimal.valueOf(501));
        assertEquals(mapped.get(requestedLocalDrives.get(1)).getId(), id(4));
    }

    @Test
    public void whenMappingMultipleRequestedWithMultipleAvailableOfDifferentTypes_shouldMap() {
        ArrayList<RequestedLocalDrive> requestedLocalDrives = newArrayList(
                createRequestedDrive(HDD, 500, SAS, 200, "ASDFQWER", null),
                createRequestedDrive(null, 502, null, null, null, null)
        );

        ArrayList<LocalStorage> availableLocalDrives = newArrayList(
                createAvailableLocalDrive(HDD, new BigDecimal(501), SAS, 200, "ASDFQWER", null),
                createAvailableSimpleStorage(new BigDecimal(502), null)
        );

        Map<RequestedLocalDrive, LocalStorage> mapped = mapper.map(requestedLocalDrives, availableLocalDrives);

        assertEquals(mapped.size(), 2);
        assertEquals(mapped.get(requestedLocalDrives.get(0)).getCapacityGib(), BigDecimal.valueOf(501));
        assertEquals(mapped.get(requestedLocalDrives.get(1)).getCapacityGib(), BigDecimal.valueOf(502));
    }
}
