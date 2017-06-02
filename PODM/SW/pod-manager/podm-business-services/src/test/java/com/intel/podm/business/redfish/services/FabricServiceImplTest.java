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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.FabricDto;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import org.testng.annotations.Test;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.FABRIC;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.State.ENABLED;
import static java.util.Collections.emptyList;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class FabricServiceImplTest {
    public static final Protocol FABRIC_TYPE = Protocol.AHCI;

    public static final String NAME = "name";
    public static final String DESCRIPTION = "description";
    private static final Status STATUS = new Status(ENABLED, OK, null);
    private static final Integer MAX_ZONES = 1;

    @Test
    public void translationTest() throws ContextResolvingException {
        FabricServiceImpl service = new FabricServiceImpl();

        Context fabricContext = contextOf(id(1), FABRIC);
        Fabric mockedManager = createFabricProvider();
        service.traverser = mock(EntityTreeTraverser.class);
        when(service.traverser.traverse(fabricContext)).thenReturn(mockedManager);
        service.unknownOemTranslator = mock(UnknownOemTranslator.class);
        when(service.unknownOemTranslator.translateUnknownOemToDtos(any(), any())).thenReturn(emptyList());

        FabricDto dto = service.getResource(fabricContext);

        assertEquals(dto.getName(), NAME);
        assertEquals(dto.getDescription(), DESCRIPTION);
        assertEquals(dto.getStatus(), STATUS);
        assertEquals(dto.getFabricType(), FABRIC_TYPE);
        assertEquals(dto.getMaxZones(), MAX_ZONES);
    }

    private Fabric createFabricProvider() {
        Fabric manager = mock(Fabric.class);
        when(manager.getId()).thenReturn(id(1));
        when(manager.getName()).thenReturn(NAME);
        when(manager.getStatus()).thenReturn(STATUS);
        when(manager.getMaxZones()).thenReturn(MAX_ZONES);
        when(manager.getFabricType()).thenReturn(FABRIC_TYPE);
        when(manager.getDescription()).thenReturn(DESCRIPTION);
        return manager;
    }
}
