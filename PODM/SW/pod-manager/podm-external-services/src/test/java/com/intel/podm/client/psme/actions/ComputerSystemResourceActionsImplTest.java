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

package com.intel.podm.client.psme.actions;

import com.intel.podm.client.actions.ComputerSystemResourceActionsImpl;
import com.intel.podm.client.actions.ResetResourceActionImpl;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.actions.ComputerSystemResourceActions;
import com.intel.podm.client.api.actions.ResetAction;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;
import com.intel.podm.client.resources.redfish.RedfishErrorResponseImpl;
import com.intel.podm.common.types.actions.ComputerSystemUpdateDefinition;
import com.intel.podm.common.types.actions.ResetType;
import org.testng.annotations.Test;

import java.net.URI;

import static com.intel.podm.common.types.BootSourceMode.UEFI;
import static com.intel.podm.common.types.BootSourceState.CONTINUOUS;
import static com.intel.podm.common.types.BootSourceType.HDD;
import static java.net.URI.create;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;
@SuppressWarnings({"checkstyle:MethodName", "checkstyle:ClassFanOutComplexity"})
public class ComputerSystemResourceActionsImplTest {
    private static final URI ANY_URI = create("");

    @Test(expectedExceptions = ExternalServiceApiActionException.class)
    public void whenResetWithErrorResponse_ShouldThrowException() throws Exception {
        ResetType anyResetType = ResetType.ON;

        WebClient mockedWebClient = mock(WebClient.class);
        doThrow(externalServiceApiActionException()).when(mockedWebClient).post(any(URI.class), any(Object.class));

        ResetAction resetAction = new ResetResourceActionImpl(mockedWebClient);
        resetAction.reset(ANY_URI, anyResetType);
    }

    @Test
    public void whenResetWithOkResponse_ShouldNotThrowException() throws Exception {
        ResetType anyResetType = ResetType.ON;

        WebClient mockedWebClient = mock(WebClient.class);
        doReturn(URI.create("/dummy-uri")).when(mockedWebClient).post(any(URI.class), any(Object.class));

        ResetAction resetAction = new ResetResourceActionImpl(mockedWebClient);
        resetAction.reset(ANY_URI, anyResetType);
    }

    @Test(expectedExceptions = ExternalServiceApiActionException.class)
    public void whenBootSourceOverrideWithErrorResponse_ShouldThrowExternalServiceApiActionException() throws Exception {
        ComputerSystemUpdateDefinition computerSystemUpdateDefinition = new ComputerSystemUpdateDefinition(UEFI, HDD, CONTINUOUS);

        WebClient mockedWebClient = mock(WebClient.class);
        doThrow(externalServiceApiActionException()).when(mockedWebClient).patchAndRetrieve(any(URI.class), any(Object.class));

        ComputerSystemResourceActions computerSystemResourceActions = new ComputerSystemResourceActionsImpl(mockedWebClient);
        computerSystemResourceActions.update(ANY_URI, computerSystemUpdateDefinition);
    }

    @Test(expectedExceptions = ExternalServiceApiReaderException.class)
    public void whenBootSourceOverrideWithErrorResponse_ShouldThrowExternalServiceApiReaderException() throws Exception {
        ComputerSystemUpdateDefinition computerSystemUpdateDefinition = new ComputerSystemUpdateDefinition(UEFI, HDD, CONTINUOUS);

        WebClient mockedWebClient = mock(WebClient.class);
        doThrow(externalServiceApiReaderException()).when(mockedWebClient).patchAndRetrieve(any(URI.class), any(Object.class));

        ComputerSystemResourceActions computerSystemResourceActions = new ComputerSystemResourceActionsImpl(mockedWebClient);
        computerSystemResourceActions.update(ANY_URI, computerSystemUpdateDefinition);
    }

    @Test
    public void whenBootSourceOverrideWithOkResponse_ShouldNotThrowException() throws Exception {
        ComputerSystemUpdateDefinition computerSystemUpdateDefinition = new ComputerSystemUpdateDefinition(UEFI, HDD, CONTINUOUS);

        WebClient mockedWebClient = mock(WebClient.class);
        ComputerSystemResource computerSystemResourceMock = mock(ComputerSystemResource.class);
        doReturn(computerSystemResourceMock).when(mockedWebClient).patchAndRetrieve(any(URI.class), any(Object.class));

        ComputerSystemResourceActions computerSystemResourceActions = new ComputerSystemResourceActionsImpl(mockedWebClient);
        computerSystemResourceActions.update(ANY_URI, computerSystemUpdateDefinition);
    }

    private ExternalServiceApiActionException externalServiceApiActionException() {
        return new ExternalServiceApiActionException("Server error", create("/redfish/v1/"), new RedfishErrorResponseImpl());
    }

    private ExternalServiceApiReaderException externalServiceApiReaderException() {
        return new ExternalServiceApiReaderException("Server error", create("/redfish/v1/"));
    }

}
