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

package com.intel.rsd.resourcemanager.layers.requestenhancer;

import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.val;
import org.mockito.ArgumentCaptor;
import org.mockito.Captor;
import org.springframework.http.HttpHeaders;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import static org.assertj.core.api.Assertions.assertThat;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpHeaders.CACHE_CONTROL;
import static org.springframework.http.HttpMethod.GET;

public class RequestEnhancerTest {

    @Captor
    ArgumentCaptor<HttpHeaders> captor;

    @BeforeMethod
    public void setUp() {
        initMocks(this);
    }

    @Test
    public void whenGlobalResourceIsRequestedAndCacheControlIsNotDefined_thenCacheControlWithMaxStaleShouldBeAdded() {
        val globalResources = mock(GlobalResources.class);
        when(globalResources.isGlobalResource(any())).thenReturn(true);

        val sut = new RequestEnhancer(globalResources);
        val nextLayer = mock(Layer.class);
        sut.setNext(nextLayer);

        sut.invokeImpl(new ServiceId("any"), "any", GET, new HttpHeaders(), null, null);

        verify(nextLayer).invoke(any(), any(), any(), captor.capture(), any(), any());
        assertThat(captor.getValue()).containsKey(CACHE_CONTROL);
    }


    @Test
    public void whenNonGlobalResourceIsRequestedAndCacheControlIsNotDefined_thenCacheControlWithMaxStaleShouldNotBeAdded() {
        val globalResources = mock(GlobalResources.class);
        when(globalResources.isGlobalResource(any())).thenReturn(false);

        val sut = new RequestEnhancer(globalResources);
        val nextLayer = mock(Layer.class);
        sut.setNext(nextLayer);

        sut.invokeImpl(new ServiceId("any"), "any", GET, new HttpHeaders(), null, null);

        verify(nextLayer).invoke(any(), any(), any(), captor.capture(), any(), any());
        assertThat(captor.getValue()).doesNotContainKey(CACHE_CONTROL);
    }

    @Test
    public void whenGlobalResourceIsRequestedAndCacheControlIsDefined_thenCacheControlShouldNotBeOverriden() {
        val globalResources = mock(GlobalResources.class);
        when(globalResources.isGlobalResource(any())).thenReturn(true);

        val sut = new RequestEnhancer(globalResources);
        val nextLayer = mock(Layer.class);
        sut.setNext(nextLayer);

        val requestedHeaders = new HttpHeaders();
        requestedHeaders.setCacheControl("max-stale=-1");
        sut.invokeImpl(new ServiceId("any"), "any", GET, requestedHeaders, null, null);

        verify(nextLayer).invoke(any(), any(), any(), captor.capture(), any(), any());
        assertThat(captor.getValue()).containsKey(CACHE_CONTROL);
        assertThat(captor.getValue()).extracting(CACHE_CONTROL).contains(requestedHeaders.get(CACHE_CONTROL));
    }
}
