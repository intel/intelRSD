/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.merger.request.creators;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.merger.request.ActionInfo;
import com.intel.rsd.resourcemanager.layers.merger.request.AnyActionRequestCreator;
import org.mockito.Mock;
import org.springframework.http.HttpHeaders;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import static com.google.common.collect.Sets.newHashSet;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.ResponseEntity.ok;
import static org.testng.Assert.assertTrue;

public class AnyActionRequestCreatorTest {

    AnyActionRequestCreator sut;

    @Mock
    private Layer nextLayer;

    @BeforeMethod
    public void setUp() throws Exception {
        initMocks(this);
        this.sut = new AnyActionRequestCreator();
    }

    @Test
    public void whenRequestedActionIsSupportedByOneUnderlyingService_responseReturnedByThisServiceShouldBeReturned() throws Exception {
        String actionPath = "/redfish/v1/Something/1/Actions/MYAction";
        ActionInfo actionInfo = new ActionInfo(actionPath);
        ServiceId targetService1 = new ServiceId("S1");

        when(nextLayer.getServicesExposingPath(actionPath))
            .thenReturn(newHashSet(targetService1));

        when(nextLayer.invoke(eq(targetService1), eq(actionInfo.getResourcePath()), eq(GET), any(), any()))
            .thenReturn(new Response(ok(JsonNodeFactory.instance.objectNode().set("#MYAction", mock(JsonNode.class)))));

        when(nextLayer.invoke(eq(targetService1), eq(actionPath), eq(POST), any(), any()))
            .thenReturn(new Response(ok().build()));

        Response[] result = sut.create(actionPath, POST, new HttpHeaders(), mock(JsonNode.class), null).execute(nextLayer);

        verify(nextLayer).invoke(eq(targetService1), eq(actionInfo.getResourcePath()), eq(GET), any(), any());
        verify(nextLayer).invoke(eq(targetService1), eq(actionPath), eq(POST), any(), any(), any());
        assertTrue(result.length == 1);
    }

    @Test
    public void whenRequestedActionIsNotSupportedByAnyOfUnderlyingServices_zeroResponsesShouldBeReturned() throws Exception {
        String actionPath = "/redfish/v1/Something/1/Actions/MYAction";
        ActionInfo actionInfo = new ActionInfo(actionPath);
        ServiceId targetService1 = new ServiceId("S1");

        when(nextLayer.getServicesExposingPath(actionPath))
            .thenReturn(newHashSet(targetService1));

        when(nextLayer.invoke(eq(targetService1), eq(actionInfo.getResourcePath()), eq(GET), any(), any()))
            .thenReturn(new Response(ok(JsonNodeFactory.instance.objectNode())));

        Response[] result = sut.create(actionPath, POST, new HttpHeaders(), mock(JsonNode.class), null).execute(nextLayer);

        verify(nextLayer).invoke(eq(targetService1), eq(actionInfo.getResourcePath()), eq(GET), any(), any());
        verify(nextLayer, never()).invoke(any(), any(), eq(POST), any(), any());
        assertTrue(result.length == 0);
    }
}
