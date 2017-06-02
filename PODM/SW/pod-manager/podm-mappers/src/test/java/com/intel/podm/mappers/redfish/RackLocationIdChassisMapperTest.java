/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.client.api.resources.redfish.ChassisResource;
import com.intel.podm.client.api.resources.redfish.ChassisResource.RackScaleRackChassisOem;
import com.intel.podm.client.api.resources.redfish.LocationObject;
import com.intel.podm.common.types.Ref;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import static com.intel.podm.common.types.Ref.unassigned;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

@SuppressWarnings("checkstyle:MethodName")
public class RackLocationIdChassisMapperTest {
    @InjectMocks
    private ChassisMapper mapper;

    @BeforeTest
    public void initMocks() {
        MockitoAnnotations.initMocks(this);
    }

    @Test
    public void whenRmmHasLocationId_shouldMapRmmLocationIdToRackLocationId() {
        final Ref<String> refRackLocationId = Ref.of("1234");

        ChassisResource source = prepareMockedSource(refRackLocationId, null);
        Chassis target = new Chassis();

        mapper.map(source, target);

        assertEquals(target.getLocationId(), refRackLocationId.get());
    }

    @Test
    public void whenRmmDoesNotHaveLocationId_shouldMapRackPuidToRackLocationId() {
        final Integer rackPuid = 5678;

        ChassisResource source = prepareMockedSource(unassigned(), rackPuid);
        Chassis target = new Chassis();

        mapper.map(source, target);

        assertEquals(target.getLocationId(), rackPuid.toString());
    }

    @Test
    public void whenRmmHasBothRackPuidAndLocationId_shouldMapRmmLocationIdToRackLocationId() {
        final Ref<String> refRackLocationId = Ref.of("1234");
        final Integer rackPuid = 5678;

        ChassisResource source = prepareMockedSource(refRackLocationId, rackPuid);
        Chassis target = new Chassis();

        mapper.map(source, target);

        assertEquals(target.getLocationId(), refRackLocationId.get());
    }

    private ChassisResource prepareMockedSource(Ref<String> rackLocationId, Integer rackPuid) {
        ChassisResource source = mock(ChassisResource.class);
        RackScaleRackChassisOem sourceOem = mock(RackScaleRackChassisOem.class);
        LocationObject sourceLocationObject = mock(LocationObject.class);

        when(source.getRackChassisAttributes()).thenReturn(sourceOem);
        when(sourceOem.getLocation()).thenReturn(sourceLocationObject);
        when(sourceLocationObject.getId()).thenReturn(rackLocationId);
        when(sourceOem.getRackPuid()).thenReturn(rackPuid);

        return source;
    }
}
