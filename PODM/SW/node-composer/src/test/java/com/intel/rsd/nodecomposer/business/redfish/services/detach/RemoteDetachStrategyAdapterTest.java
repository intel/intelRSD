/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.detach;

import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.RestRequestInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.ZoneActionsInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachDetachOperationContext;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.redfish.response.RedfishClientException;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import com.intel.rsd.nodecomposer.types.net.HttpStatusCode;
import com.intel.rsd.nodecomposer.types.redfish.ExternalServiceError;
import com.intel.rsd.nodecomposer.types.redfish.RedfishErrorResponse;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.Collections;
import java.util.Set;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromString;
import static com.intel.rsd.nodecomposer.types.net.HttpStatusCode.NOT_FOUND;
import static com.intel.rsd.nodecomposer.types.net.HttpStatusCode.SERVICE_UNAVAILABLE;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.testng.Assert.fail;

public class RemoteDetachStrategyAdapterTest {

    @Mock
    private ZoneActionsInvoker zoneActionsInvoker;

    @Mock
    private RestRequestInvoker restRequestInvoker;

    @Mock
    private AttachDetachOperationContext attachDetachOperationContext;

    @Mock
    private Volume entityToDetach;

    @Mock
    private Endpoint endpointMock;

    @Mock
    private Zone zoneMock;

    @Mock
    private ExternalServiceError.Response responseMock;

    @Mock
    private RedfishErrorResponse redfishErrorResponseMock;

    @InjectMocks
    private RemoteDetachStrategyAdapter<Volume> sut;

    @BeforeMethod
    public void setUp() {
        initMocks(this);

        Set<Endpoint> endpoints = Collections.singleton(endpointMock);
        when(entityToDetach.getEndpoints()).thenReturn(endpoints);
        when(endpointMock.getZone()).thenReturn(zoneMock);
        when(endpointMock.getUri()).thenReturn(oDataIdFromString("endpointUri"));
        when(zoneMock.getUri()).thenReturn(oDataIdFromString("zoneTargetUri"));
    }

    @Test
    public void whenEndpointNotFoundProceedWithDetach() throws WebClientRequestException {
        whenRemoteCallFailsWith(NOT_FOUND);

        try {
            sut.detachOnRemoteFabric(entityToDetach, attachDetachOperationContext);
        } catch (EntityOperationException e) {
            fail();
        }
    }


    @Test(expectedExceptions = EntityOperationException.class)
    public void whenEndpointDeletionFailsStopDetachProcess() throws EntityOperationException, WebClientRequestException {
        whenRemoteCallFailsWith(SERVICE_UNAVAILABLE);

        sut.detachOnRemoteFabric(entityToDetach, attachDetachOperationContext);
    }

    @SuppressWarnings({"checkstyle:MethodLength", "checkstyle:AnonInnerLength"})
    private void whenRemoteCallFailsWith(HttpStatusCode httpStatusCode) throws WebClientRequestException {
        doThrow(new WebClientRequestException("msg", URI.create("resourceUri"), new RedfishClientException("msg", new ExternalServiceError() {
            @Override
            public Request getRequest() {
                return mock(Request.class);
            }

            @Override
            public Response getResponse() {
                return responseMock;
            }

            @Override
            public RedfishErrorResponse getRedfishErrorResponse() {
                return redfishErrorResponseMock;
            }
        }))).when(restRequestInvoker).delete(any(URI.class));
        when(responseMock.getHttpStatusCode()).thenReturn(httpStatusCode);
    }
}
