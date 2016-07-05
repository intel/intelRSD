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

package com.intel.podm.discovery.internal;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.properties.Console;
import com.intel.podm.business.entities.redfish.properties.GraphicalConsole;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;
import com.intel.podm.config.version.VersionLoader;
import org.testng.annotations.Test;

import static com.google.common.collect.ImmutableList.of;
import static java.util.Collections.emptyList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class PodManagerDiscoveryHandlerTest {

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*Pod.*")
    public void whenCreatingManagerForNullPod_ShouldThrow() {
        PodManagerDiscoveryHandler handler = createPodManagerDiscoveryHandler();
        handler.getManagerForPod(null);
    }

    @Test
    public void whenGettingManagerForPod_ShouldNotCreateManagerWhenManagerExists() {
        PodManagerDiscoveryHandler handler = createPodManagerDiscoveryHandler();
        Chassis mockedPod = mock(Chassis.class);
        when(mockedPod.getManagers()).thenReturn(of(new Manager()));

        handler.getManagerForPod(mockedPod);

        verify(mockedPod).getManagers();
        verify(handler.genericDao, never()).create(Manager.class);
    }

    @Test
    public void whenCreatingManagerForPod_ShouldLinkItToPod() {
        PodManagerDiscoveryHandler handler = createPodManagerDiscoveryHandler();
        Chassis mockedPod = mock(Chassis.class);
        when(mockedPod.getManagers()).thenReturn(emptyList());

        Manager managerForPod = handler.getManagerForPod(mockedPod);

        verify(mockedPod).addManager(managerForPod);
    }

    @Test
    public void whenCreatingManagerForPod_ShouldInitializeNetworking() {
        PodManagerDiscoveryHandler handler = createPodManagerDiscoveryHandler();

        Chassis mockedPod = mock(Chassis.class);
        when(mockedPod.getManagers()).thenReturn(emptyList());

        Manager managerForPod = handler.getManagerForPod(mockedPod);

        verify(handler.podManagerNetworkDiscoveryHandler).initializeNetworking(managerForPod);
    }

    private PodManagerDiscoveryHandler createPodManagerDiscoveryHandler() {
        PodManagerDiscoveryHandler handler = new PodManagerDiscoveryHandler();
        handler.config = mock(Holder.class);
        handler.genericDao = mock(GenericDao.class);

        Manager manager = mock(Manager.class);
        when(manager.getGraphicalConsole()).thenReturn(mock(GraphicalConsole.class));
        when(manager.getSerialConsole()).thenReturn(mock(Console.class));
        when(manager.getCommandShell()).thenReturn(mock(Console.class));

        when(handler.genericDao.create(Manager.class)).thenReturn(manager);
        when(handler.genericDao.create(Console.class)).thenReturn(mock(Console.class));
        when(handler.genericDao.create(GraphicalConsole.class)).thenReturn(mock(GraphicalConsole.class));
        when(handler.config.get()).thenReturn(mock(ServiceConfig.class));

        handler.podManagerNetworkDiscoveryHandler = mock(PodManagerNetworkDiscoveryHandler.class);
        handler.versionLoader = mock(VersionLoader.class);

        return handler;
    }
}
