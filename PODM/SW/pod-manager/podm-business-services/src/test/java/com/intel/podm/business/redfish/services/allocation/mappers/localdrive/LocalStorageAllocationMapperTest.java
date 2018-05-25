/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.mappers.localdrive;

import com.intel.podm.business.entities.redfish.base.LocalStorage;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.logger.Logger;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.math.BigDecimal;
import java.util.List;
import java.util.Map;

import static com.intel.podm.business.redfish.services.allocation.templates.assets.LocalDrivesCreation.createAvailableLocalDrive;
import static com.intel.podm.business.redfish.services.allocation.templates.assets.LocalDrivesCreation.createAvailableSimpleStorage;
import static com.intel.podm.business.redfish.services.allocation.templates.assets.LocalDrivesCreation.createRequestedDrive;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.DRIVE;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.MediaType.HDD;
import static com.intel.podm.common.types.Protocol.SAS;
import static java.util.Arrays.asList;
import static java.util.Collections.singletonList;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName"})
public class LocalStorageAllocationMapperTest {
    private static final BigDecimal RPM_VALUE = new BigDecimal(200);

    @Mock
    private Logger logger;

    @InjectMocks
    private LocalStorageAllocationMapper mapper;

    @BeforeClass
    public void setup() {
        MockitoAnnotations.initMocks(this);
    }

    @Test
    public void whenMappingSingleRequestedWithMultipleAvailable_shouldMapToMinimalOne() {
        List<RequestedNode.LocalDrive> requestedLocalDrives = asList(
            createRequestedDrive(HDD, 500, SAS, RPM_VALUE, "ASDFQWER", null)
        );

        List<LocalStorage> availableLocalDrives = asList(
            createAvailableLocalDrive(HDD, new BigDecimal(502), SAS, RPM_VALUE, "ASDFQWER", null),
            createAvailableLocalDrive(HDD, new BigDecimal(501), SAS, RPM_VALUE, "ASDFQWER", null)
        );

        Map<RequestedNode.LocalDrive, LocalStorage> mapped = mapper.map(requestedLocalDrives, availableLocalDrives);

        assertEquals(mapped.size(), 1);
        assertEquals(mapped.get(requestedLocalDrives.get(0)).getCapacityGib(), BigDecimal.valueOf(501));
    }

    @Test
    public void whenMappingMultipleRequestedWithMultipleAvailable_shouldMapToMinimalSubset() {
        List<RequestedNode.LocalDrive> requestedLocalDrives = asList(
            createRequestedDrive(HDD, 500, SAS, RPM_VALUE, "ASDFQWER", contextOf(id(3), DRIVE)),
            createRequestedDrive(HDD, 500, SAS, RPM_VALUE, "ASDFQWER", null)
        );

        List<LocalStorage> availableLocalDrives = asList(
            createAvailableLocalDrive(HDD, new BigDecimal(502), SAS, RPM_VALUE, "ASDFQWER", id(1)),
            createAvailableLocalDrive(HDD, new BigDecimal(503), SAS, RPM_VALUE, "ASDFQWER", id(2)),
            createAvailableLocalDrive(HDD, new BigDecimal(504), SAS, RPM_VALUE, "ASDFQWER", id(3)),
            createAvailableLocalDrive(HDD, new BigDecimal(501), SAS, RPM_VALUE, "ASDFQWER", id(4))
        );

        Map<RequestedNode.LocalDrive, LocalStorage> mapped = mapper.map(requestedLocalDrives, availableLocalDrives);

        assertEquals(mapped.size(), 2);
        assertEquals(mapped.get(requestedLocalDrives.get(0)).getCapacityGib(), BigDecimal.valueOf(504));
        assertEquals(mapped.get(requestedLocalDrives.get(0)).getId(), id(3));

        assertEquals(mapped.get(requestedLocalDrives.get(1)).getCapacityGib(), BigDecimal.valueOf(501));
        assertEquals(mapped.get(requestedLocalDrives.get(1)).getId(), id(4));
    }

    @Test
    public void whenMappingMultipleRequestedWithMultipleAvailableOfDifferentTypes_shouldMap() {
        List<RequestedNode.LocalDrive> requestedLocalDrives = asList(
            createRequestedDrive(HDD, 500, SAS, RPM_VALUE, "ASDFQWER", null),
            createRequestedDrive(null, 502, null, null, null, null)
        );

        List<LocalStorage> availableLocalDrives = asList(
            createAvailableLocalDrive(HDD, new BigDecimal(501), SAS, RPM_VALUE, "ASDFQWER", null),
            createAvailableSimpleStorage(new BigDecimal(502), null)
        );

        Map<RequestedNode.LocalDrive, LocalStorage> mapped = mapper.map(requestedLocalDrives, availableLocalDrives);

        assertEquals(mapped.size(), 2);
        assertEquals(mapped.get(requestedLocalDrives.get(0)).getCapacityGib(), BigDecimal.valueOf(501));
        assertEquals(mapped.get(requestedLocalDrives.get(1)).getCapacityGib(), BigDecimal.valueOf(502));
    }

    @Test
    public void whenMappingRequestedStorageWithoutType_shouldMapToAnyOfAvailableLocalStorage() {
        List<RequestedNode.LocalDrive> requestedLocalDrives = singletonList(
            createRequestedDrive(null, null, null, null, null, null)
        );

        List<LocalStorage> availableLocalDrives = asList(
            createAvailableLocalDrive(HDD, new BigDecimal(502), SAS, RPM_VALUE, "ASDFQWER", id(1)),
            createAvailableLocalDrive(HDD, new BigDecimal(503), SAS, RPM_VALUE, "ASDFQWER", id(2)),
            createAvailableLocalDrive(HDD, new BigDecimal(504), SAS, RPM_VALUE, "ASDFQWER", id(3)),
            createAvailableLocalDrive(HDD, new BigDecimal(501), SAS, RPM_VALUE, "ASDFQWER", id(4)),
            createAvailableLocalDrive(null, new BigDecimal(501), null, RPM_VALUE, "ASDFQWER", id(4)),
            createAvailableLocalDrive(null, null, null, null, null, null)
        );

        Map<RequestedNode.LocalDrive, LocalStorage> mapped = mapper.map(requestedLocalDrives, availableLocalDrives);

        assertEquals(mapped.size(), 1);
    }
}
