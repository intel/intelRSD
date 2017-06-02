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

package com.intel.podm.discovery.internal;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.embeddables.CommandShell;
import com.intel.podm.business.entities.redfish.embeddables.GraphicalConsole;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.embeddables.SerialConsole;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;
import com.intel.podm.config.version.VersionLoader;
import org.testng.annotations.Test;

import static java.util.Collections.emptySet;
import static java.util.Collections.singleton;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:ExecutableStatementCount"})
public class PodManagerDiscoveryHandlerTest {
    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenCreatingManagerForNullPod_ShouldThrow() {
        PodManagerDiscoveryHandler handler = createPodManagerDiscoveryHandler();
        handler.getManagerForPod(null);
    }

    @Test
    public void whenGettingManagerForPod_ShouldNotCreateManagerWhenManagerExists() {
        PodManagerDiscoveryHandler handler = createPodManagerDiscoveryHandler();
        Chassis mockedPod = mock(Chassis.class);
        when(mockedPod.getManagers()).thenReturn(singleton(new Manager()));

        handler.getManagerForPod(mockedPod);

        verify(mockedPod).getManagers();
        verify(handler.genericDao, never()).create(Manager.class);
    }

    @Test
    public void whenCreatingManagerForPod_ShouldLinkItToPod() {
        PodManagerDiscoveryHandler handler = createPodManagerDiscoveryHandler();
        Chassis mockedPod = mock(Chassis.class);
        when(mockedPod.getManagers()).thenReturn(emptySet());

        Manager managerForPod = handler.getManagerForPod(mockedPod);

        verify(mockedPod).addManager(managerForPod);
    }

    @Test
    public void whenCreatingManagerForPod_ShouldInitializeNetworking() {
        PodManagerDiscoveryHandler handler = createPodManagerDiscoveryHandler();

        Chassis mockedPod = mock(Chassis.class);
        when(mockedPod.getManagers()).thenReturn(emptySet());

        Manager managerForPod = handler.getManagerForPod(mockedPod);

        verify(handler.podManagerNetworkDiscoveryHandler).initializeNetworking(managerForPod);
    }

    private PodManagerDiscoveryHandler createPodManagerDiscoveryHandler() {
        PodManagerDiscoveryHandler handler = new PodManagerDiscoveryHandler();
        handler.config = mock(Holder.class);
        handler.genericDao = mock(GenericDao.class);

        GraphicalConsole graphicalConsole = mock(GraphicalConsole.class);
        SerialConsole serialConsole = mock(SerialConsole.class);
        CommandShell commandShell = mock(CommandShell.class);

        Manager manager = mock(Manager.class);
        when(manager.getGraphicalConsole()).thenReturn(graphicalConsole);
        when(manager.getSerialConsole()).thenReturn(serialConsole);
        when(manager.getCommandShell()).thenReturn(commandShell);

        when(handler.genericDao.create(Manager.class)).thenReturn(manager);
        when(handler.config.get()).thenReturn(mock(ServiceConfig.class));

        handler.podManagerNetworkDiscoveryHandler = mock(PodManagerNetworkDiscoveryHandler.class);
        handler.versionLoader = mock(VersionLoader.class);

        return handler;
    }
}
