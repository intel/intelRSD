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

package com.intel.rsd.resourcemanager.layers.unifier;

import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.CrawlerFactory;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.unifier.unification.GrandUnifier;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolver;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolverFactory;
import org.mockito.Mock;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Test;

import static java.util.Optional.empty;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.testng.Assert.assertEquals;

@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class UnificationLayerTest {

    private UnificationLayer sut;

    @Mock
    private UnifiedOdataIdResolverFactory factory;

    @Mock
    private UnifiedOdataIdResolver resolver;

    @Mock
    private Layer nextLayer;

    @Mock
    private GrandUnifier unifier;

    @BeforeTest
    void beforeTest() {
        initMocks(this);
        sut = new UnificationLayer(factory, new CrawlerFactory(1), unifier);
        sut.setNext(nextLayer);
    }

    @Test
    public void whenGettingUnknownResourceShouldReturnNotFoundStatusCode() throws Exception {
        ServiceId serviceId = new ServiceId("any");
        when(factory.createLoadingResolver(eq(serviceId.getValue()), any())).thenReturn(resolver);
        String path = "/redfish/v1/unknownResource";
        when(resolver.toServiceSpecific(path)).thenReturn(empty());
        Response actualResponse = sut.invoke(serviceId, path, GET);

        assertEquals(actualResponse.getHttpStatus(), NOT_FOUND);
    }
}
