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
import com.intel.podm.business.dto.ManagerDto;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.embeddables.CommandShell;
import com.intel.podm.business.entities.redfish.embeddables.GraphicalConsole;
import com.intel.podm.business.entities.redfish.embeddables.SerialConsole;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.GeneralConnectType;
import com.intel.podm.common.types.GraphicalConnectType;
import com.intel.podm.common.types.ManagerType;
import com.intel.podm.common.types.Status;
import org.testng.annotations.Test;

import static com.intel.podm.business.redfish.services.mappers.MockedMapperFactory.createManagerMapperWithMockedOem;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.MANAGER;
import static com.intel.podm.common.types.GeneralConnectType.OEM;
import static com.intel.podm.common.types.GraphicalConnectType.KVMIP;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.ManagerType.BMC;
import static com.intel.podm.common.types.State.ENABLED;
import static java.util.Collections.singletonList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:ExecutableStatementCount"})
public class ManagerReaderServiceImplTest {
    public static final ManagerType TYPE = BMC;

    public static final String FIRMWARE_VERSION = "1";
    public static final String MODEL = "model";
    public static final String NAME = "name";

    private static final Status STATUS = new Status(ENABLED, OK, null);
    private static final Boolean CONSOLE_ENABLED = true;
    private static final Integer CONSOLE_MAX_CONCURRENT_SESSIONS = 1;
    private static final GeneralConnectType CONSOLE_CONNECT_TYPE_SUPPORTED = OEM;
    private static final GraphicalConnectType GRAPHICAL_CONSOLE_CONNECT_TYPE_SUPPORTED = KVMIP;

    @Test
    public void translationTest() throws ContextResolvingException {
        ManagerReaderServiceImpl service = new ManagerReaderServiceImpl();

        Context managerContext = contextOf(id(1), MANAGER);
        Manager mockedManager = createManagerProvider();
        service.traverser = mock(EntityTreeTraverser.class);
        when(service.traverser.traverse(managerContext)).thenReturn(mockedManager);

        service.entityToDtoMapper = createManagerMapperWithMockedOem();
        ManagerDto dto = service.getResource(managerContext);

        assertEquals(dto.getName(), NAME);
        assertEquals(dto.getFirmwareVersion(), FIRMWARE_VERSION);
        assertEquals(dto.getStatus(), STATUS);
        assertEquals(dto.getManagerType(), TYPE);
        assertEquals(dto.getModel(), MODEL);

        checkConsole(dto.getGraphicalConsole());
        checkConsole(dto.getSerialConsole());
        checkConsole(dto.getCommandShell());
    }

    private void checkConsole(ManagerDto.ConsoleDto consoleDto) {
        assertEquals(consoleDto.getServiceEnabled(), CONSOLE_ENABLED);
        assertEquals(consoleDto.getMaxConcurrentSessions(), CONSOLE_MAX_CONCURRENT_SESSIONS);
        assertEquals(consoleDto.getConnectTypesSupported(), singletonList(CONSOLE_CONNECT_TYPE_SUPPORTED));
    }

    private void checkConsole(ManagerDto.GraphicalConsoleDto consoleDto) {
        assertEquals(consoleDto.getServiceEnabled(), CONSOLE_ENABLED);
        assertEquals(consoleDto.getMaxConcurrentSessions(), CONSOLE_MAX_CONCURRENT_SESSIONS);
        assertEquals(consoleDto.getConnectTypesSupported(), singletonList(GRAPHICAL_CONSOLE_CONNECT_TYPE_SUPPORTED));
    }

    private Manager createManagerProvider() {
        Chassis chassis = mock(Chassis.class);
        when(chassis.getId()).thenReturn(id(1));

        Manager manager = mock(Manager.class);
        when(manager.getId()).thenReturn(id(1));
        when(manager.getName()).thenReturn(NAME);
        when(manager.getFirmwareVersion()).thenReturn(FIRMWARE_VERSION);
        when(manager.getStatus()).thenReturn(STATUS);
        when(manager.getManagerType()).thenReturn(TYPE);
        when(manager.getModel()).thenReturn(MODEL);
        when(manager.getInChassisManager()).thenReturn(chassis);

        GraphicalConsole graphicalConsole = createGraphicalConsole();
        SerialConsole serialConsole = createSerialConsole();
        CommandShell commandShell = createCommandShell();

        when(manager.getGraphicalConsole()).thenReturn(graphicalConsole);
        when(manager.getSerialConsole()).thenReturn(serialConsole);
        when(manager.getCommandShell()).thenReturn(commandShell);

        return manager;
    }

    private GraphicalConsole createGraphicalConsole() {
        GraphicalConsole graphicalConsole = mock(GraphicalConsole.class);
        when(graphicalConsole.isServiceEnabled()).thenReturn(CONSOLE_ENABLED);
        when(graphicalConsole.getMaxConcurrentSessions()).thenReturn(CONSOLE_MAX_CONCURRENT_SESSIONS);
        when(graphicalConsole.getConnectTypesSupported()).thenReturn(singletonList(GRAPHICAL_CONSOLE_CONNECT_TYPE_SUPPORTED));

        return graphicalConsole;
    }

    private SerialConsole createSerialConsole() {
        SerialConsole serialConsole = mock(SerialConsole.class);
        when(serialConsole.isServiceEnabled()).thenReturn(CONSOLE_ENABLED);
        when(serialConsole.getMaxConcurrentSessions()).thenReturn(CONSOLE_MAX_CONCURRENT_SESSIONS);
        when(serialConsole.getConnectTypesSupported()).thenReturn(singletonList(CONSOLE_CONNECT_TYPE_SUPPORTED));

        return serialConsole;
    }

    private CommandShell createCommandShell() {
        CommandShell commandShell = mock(CommandShell.class);
        when(commandShell.isServiceEnabled()).thenReturn(CONSOLE_ENABLED);
        when(commandShell.getMaxConcurrentSessions()).thenReturn(CONSOLE_MAX_CONCURRENT_SESSIONS);
        when(commandShell.getConnectTypesSupported()).thenReturn(singletonList(CONSOLE_CONNECT_TYPE_SUPPORTED));

        return commandShell;
    }
}
