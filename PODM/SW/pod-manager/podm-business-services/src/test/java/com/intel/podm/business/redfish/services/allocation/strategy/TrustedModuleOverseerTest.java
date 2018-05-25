/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.strategy;

import com.intel.podm.business.entities.redfish.embeddables.TrustedModule;
import com.intel.podm.business.services.redfish.requests.RequestedNode.Security;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.actions.ChangeTpmStatusUpdateDefinition;
import org.testng.annotations.Test;

import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.InterfaceType.TPM1_2;
import static com.intel.podm.common.types.InterfaceType.TPM2_0;
import static com.intel.podm.common.types.State.DISABLED;
import static com.intel.podm.common.types.State.ENABLED;
import static java.lang.Boolean.TRUE;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

public class TrustedModuleOverseerTest {
    private TrustedModuleOverseer trustedModuleOverseer = new TrustedModuleOverseer();

    @Test
    public void requestedTpmPresentTrue() {
        TrustedModule firstTrustedModule = mock(TrustedModule.class);
        when(firstTrustedModule.getInterfaceType()).thenReturn(TPM1_2);
        when(firstTrustedModule.getStatus()).thenReturn(new Status(ENABLED, null, null));

        TrustedModule secondTrustedModule = mock(TrustedModule.class);
        when(secondTrustedModule.getInterfaceType()).thenReturn(TPM2_0);
        when(secondTrustedModule.getStatus()).thenReturn(new Status(DISABLED, null, null));

        List<TrustedModule> trustedModules = new ArrayList<TrustedModule>() {{
            add(firstTrustedModule);
            add(secondTrustedModule);
        }};

        Security security = mock(Security.class);
        when(security.getTpmPresent()).thenReturn(true);

        ChangeTpmStatusUpdateDefinition changeTpmStatusUpdateDefinition =
            trustedModuleOverseer.prepareChangeTpmStateUpdateDefinition(security, trustedModules);
        assertEquals(changeTpmStatusUpdateDefinition.getInterfaceType(), TPM2_0);
    }

    @Test
    public void requestedTpmInterface() {
        TrustedModule firstTrustedModule = mock(TrustedModule.class);
        when(firstTrustedModule.getInterfaceType()).thenReturn(TPM1_2);
        when(firstTrustedModule.getStatus()).thenReturn(new Status(DISABLED, null, null));

        TrustedModule secondTrustedModule = mock(TrustedModule.class);
        when(secondTrustedModule.getInterfaceType()).thenReturn(TPM2_0);
        when(secondTrustedModule.getStatus()).thenReturn(new Status(ENABLED, null, null));

        List<TrustedModule> trustedModules = new ArrayList<TrustedModule>() {{
            add(firstTrustedModule);
            add(secondTrustedModule);
        }};

        Security security = mock(Security.class);
        when(security.getTpmInterfaceType()).thenReturn(TPM1_2);

        ChangeTpmStatusUpdateDefinition changeTpmStatusUpdateDefinition =
            trustedModuleOverseer.prepareChangeTpmStateUpdateDefinition(security, trustedModules);
        assertEquals(changeTpmStatusUpdateDefinition.getInterfaceType(), TPM1_2);
    }

    @Test
    public void requestedBothTpmInterfaceAndTpmPreset() {
        TrustedModule firstTrustedModule = mock(TrustedModule.class);
        when(firstTrustedModule.getInterfaceType()).thenReturn(TPM1_2);
        when(firstTrustedModule.getStatus()).thenReturn(new Status(DISABLED, null, null));

        TrustedModule secondTrustedModule = mock(TrustedModule.class);
        when(secondTrustedModule.getInterfaceType()).thenReturn(TPM2_0);
        when(secondTrustedModule.getStatus()).thenReturn(new Status(ENABLED, null, null));

        List<TrustedModule> trustedModules = new ArrayList<TrustedModule>() {{
            add(firstTrustedModule);
            add(secondTrustedModule);
        }};

        Security security = mock(Security.class);
        when(security.getTpmPresent()).thenReturn(TRUE);
        when(security.getTpmInterfaceType()).thenReturn(TPM1_2);

        ChangeTpmStatusUpdateDefinition changeTpmStatusUpdateDefinition =
            trustedModuleOverseer.prepareChangeTpmStateUpdateDefinition(security, trustedModules);
        assertEquals(changeTpmStatusUpdateDefinition.getInterfaceType(), TPM1_2);
    }

    @Test(expectedExceptions = RuntimeException.class)
    public void requestedNotExistingTpmInterface() {
        TrustedModule firstTrustedModule = mock(TrustedModule.class);
        when(firstTrustedModule.getInterfaceType()).thenReturn(TPM1_2);
        when(firstTrustedModule.getStatus()).thenReturn(new Status(ENABLED, null, null));

        List<TrustedModule> trustedModules = new ArrayList<TrustedModule>() {{
            add(firstTrustedModule);
        }};

        Security security = mock(Security.class);
        when(security.getTpmPresent()).thenReturn(TRUE);
        when(security.getTpmInterfaceType()).thenReturn(TPM2_0);

        trustedModuleOverseer.prepareChangeTpmStateUpdateDefinition(security, trustedModules);
    }
}
