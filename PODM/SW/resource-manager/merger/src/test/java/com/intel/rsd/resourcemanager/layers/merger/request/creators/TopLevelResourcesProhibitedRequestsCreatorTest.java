/*
 * Copyright (c) 2018-2019 Intel Corporation
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
import com.fasterxml.jackson.databind.node.NullNode;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.merger.ResourceManagerResourcesProvider;
import com.intel.rsd.resourcemanager.layers.merger.request.ExecutableRequest;
import com.intel.rsd.resourcemanager.common.resources.ResourceProvider;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.Arrays;
import java.util.List;

import static java.util.Collections.emptyList;
import static org.assertj.core.util.Lists.newArrayList;
import static org.mockito.Mockito.mock;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpStatus.METHOD_NOT_ALLOWED;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings("checkstyle:AnonInnerLength")
public class TopLevelResourcesProhibitedRequestsCreatorTest {

    @Test
    public void matchingResourceExposedByResourceManagerResourcesProviderShouldBeAccepted() {
        List<HttpMethod> supportedMethods = Arrays.asList(GET);
        HttpMethod notSupportedMethod = HttpMethod.POST;
        ResourceProvider rp = new ResourceProvider() {
            @Override
            public String getODataId() {
                return "odataid";
            }

            @Override
            public JsonNode getResource() {
                return null;
            }

            @Override
            public boolean isMethodSupported(HttpMethod httpMethod) {
                return supportedMethods.contains(httpMethod);
            }
        };

        TopLevelResourcesProhibitedRequestsCreator sut
            = new TopLevelResourcesProhibitedRequestsCreator(new ResourceManagerResourcesProvider(newArrayList(rp)));

        assertTrue(sut.canBeSelected(notSupportedMethod, rp.getODataId()));
    }

    @Test(dataProvider = "allHttpMethods")
    public void anyPathNotExposedByResourceManagerResourcesProviderShouldNotBeAccepted(HttpMethod httpMethod) {
        List<ResourceProvider> resourceProviders = emptyList();
        ResourceManagerResourcesProvider resourceManagerResourcesProvider = new ResourceManagerResourcesProvider(resourceProviders);
        TopLevelResourcesProhibitedRequestsCreator sut = new TopLevelResourcesProhibitedRequestsCreator(resourceManagerResourcesProvider);
        assertFalse(sut.canBeSelected(httpMethod, "anyPath"));
    }

    @Test
    public void anyRequestCreatedBySutShouldBeFinishedWith405() {
        ResourceManagerResourcesProvider resourceManagerResourceProvider = mock(ResourceManagerResourcesProvider.class);
        TopLevelResourcesProhibitedRequestsCreator sut = new TopLevelResourcesProhibitedRequestsCreator(resourceManagerResourceProvider);
        ExecutableRequest executablerequest = sut.create("anyPath", HttpMethod.POST, new HttpHeaders(), NullNode.getInstance(), null);
        Response[] response = executablerequest.execute(mock(Layer.class));
        assertTrue(response.length == 1);
        assertTrue(response[0].getHttpStatus() == METHOD_NOT_ALLOWED);
    }

    @DataProvider
    Object[][] allHttpMethods() {
        return Arrays.stream(HttpMethod.values())
            .map(method -> new Object[]{method})
            .toArray(Object[][]::new);
    }
}
