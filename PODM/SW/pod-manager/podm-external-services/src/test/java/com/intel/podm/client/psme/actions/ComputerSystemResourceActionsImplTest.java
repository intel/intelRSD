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

package com.intel.podm.client.psme.actions;

import com.intel.podm.client.actions.ComputerSystemResourceActionsImpl;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.actions.ComputerSystemResourceActions;
import com.intel.podm.client.resources.redfish.RedfishErrorResponseImpl;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.actions.ResetType;
import org.testng.annotations.Test;

import javax.ws.rs.core.Response;
import java.net.URI;

import static com.intel.podm.common.types.BootSourceState.CONTINUOUS;
import static com.intel.podm.common.types.BootSourceType.HDD;
import static java.net.URI.create;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;

public class ComputerSystemResourceActionsImplTest {
    private static final URI ANY_URI = create("");

    @Test(expectedExceptions = ExternalServiceApiActionException.class)
    public void whenResetWithErrorResponse_ShouldThrowException() throws ExternalServiceApiActionException {
        ResetType anyResetType = ResetType.ON;

        ComputerSystemResourceActions computerSystemResourceActions = createPsmeComputerSystemActions(Response.serverError().build());
        computerSystemResourceActions.reset(ANY_URI, anyResetType);
    }

    @Test
    public void whenResetWithOkResponse_ShouldNotThrowException() throws ExternalServiceApiActionException {
        ResetType anyResetType = ResetType.ON;
        ComputerSystemResourceActions computerSystemResourceActions = createPsmeComputerSystemActions(Response.noContent().build());

        computerSystemResourceActions.reset(ANY_URI, anyResetType);
    }

    @Test(expectedExceptions = ExternalServiceApiActionException.class)
    public void whenBootSourceOverrideWithErrorResponse_ShouldThrowException() throws ExternalServiceApiActionException, ExternalServiceApiReaderException {
        BootSourceType anyBootSourceType = HDD;
        BootSourceState anyBootSourceState = CONTINUOUS;

        ComputerSystemResourceActions computerSystemResourceActions = createPsmeComputerSystemActions(Response.serverError().build());

        computerSystemResourceActions.bootSourceOverride(ANY_URI, anyBootSourceType, anyBootSourceState);
    }

    @Test
    public void whenBootSourceOverrideWithOkResponse_ShouldNotThrowException() throws ExternalServiceApiActionException, ExternalServiceApiReaderException {
        BootSourceType anyBootSourceType = HDD;
        BootSourceState anyBootSourceState = CONTINUOUS;

        ComputerSystemResourceActions computerSystemResourceActions = createPsmeComputerSystemActions(Response.ok().build());

        computerSystemResourceActions.bootSourceOverride(ANY_URI, anyBootSourceType, anyBootSourceState);
    }

    private WebClient createWebClient(Response response) throws ExternalServiceApiActionException {
        WebClient webClientMock = mock(WebClient.class);
        if (response.getStatusInfo().getFamily() == Response.Status.Family.SERVER_ERROR) {
            doThrow(createExternalServiceApiActionException()).when(webClientMock).patch(any(URI.class), any(Object.class));
            doThrow(createExternalServiceApiActionException()).when(webClientMock).post(any(URI.class), any(Object.class));
        } else {
            doNothing().when(webClientMock).patch(any(URI.class), any(Object.class));
            doReturn(URI.create("/dummy-uri")).when(webClientMock).post(any(URI.class), any(Object.class));
        }
        return webClientMock;
    }

    private ExternalServiceApiActionException createExternalServiceApiActionException() {
        return new ExternalServiceApiActionException("Server error", create("/redfish/v1/"), new RedfishErrorResponseImpl());
    }

    private ComputerSystemResourceActions createPsmeComputerSystemActions(Response response) throws ExternalServiceApiActionException {
        return new ComputerSystemResourceActionsImpl(createWebClient(response));
    }
}
