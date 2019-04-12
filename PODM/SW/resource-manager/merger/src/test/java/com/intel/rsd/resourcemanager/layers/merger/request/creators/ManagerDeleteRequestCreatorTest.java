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

import com.google.common.collect.ImmutableList;
import com.intel.rsd.resourcemanager.common.DetectedManagersCache;
import com.intel.rsd.resourcemanager.common.ServiceDetectorApiConsumer;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.val;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.Arrays;

import static java.util.Collections.emptyList;
import static java.util.UUID.randomUUID;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.springframework.http.HttpMethod.DELETE;
import static org.springframework.http.HttpStatus.METHOD_NOT_ALLOWED;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.springframework.http.HttpStatus.NO_CONTENT;
import static org.springframework.http.ResponseEntity.noContent;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ManagerDeleteRequestCreatorTest {

    ManagerDeleteRequestCreator sut;

    @Test
    public void whenDeleteMethodAndManagerInstancePathIsGiven_creatorShouldBeSelectable() {
        sut = new ManagerDeleteRequestCreator(null, null, null);
        assertTrue(sut.canBeSelected(DELETE, "/redfish/v1/Managers/1"));
    }

    @Test
    public void whenDeleteMethodAndManagerCollectionPathIsGiven_creatorShouldNotBeSelectable() {
        sut = new ManagerDeleteRequestCreator(null, null, null);
        assertFalse(sut.canBeSelected(DELETE, "/redfish/v1/Managers"));
    }

    @Test(dataProvider = "allMethodsBesideDelete")
    public void whenAnyMethodBesideDeleteAndManagerInstancePathIsGiven_creatorShouldNotBeSelectable(HttpMethod httpMethod) {
        sut = new ManagerDeleteRequestCreator(null, null, null);
        assertFalse(sut.canBeSelected(httpMethod, "/redfish/v1/Managers"));
    }

    @Test
    public void whenRequestedUriIsNotRelatedWithAnyUnderlyingManager_404ShouldBeReturned() {
        val serviceDetectorApiConsumer = mock(ServiceDetectorApiConsumer.class);
        when(serviceDetectorApiConsumer.deleteManager(any())).thenReturn(ResponseEntity.notFound().build());

        val detectedManagersReadableCache = mock(DetectedManagersCache.class);
        sut = new ManagerDeleteRequestCreator(serviceDetectorApiConsumer, detectedManagersReadableCache, null);

        val layer = mock(Layer.class);
        when(layer.getServicesExposingPath(any())).thenReturn(emptyList());

        val results = sut.create("/redfish/v1/Managers/1", null, null, null, null).execute(layer);

        assertEquals(results.length, 1);
        assertEquals(results[0].getHttpStatus(), NOT_FOUND);
    }

    @Test
    public void whenRequestedUriIsRelatedWithManagerKnownForUnderlyingLayers_400ShouldBeReturned() {
        val serviceDetectorApiConsumer = mock(ServiceDetectorApiConsumer.class);
        when(serviceDetectorApiConsumer.deleteManager(any())).thenReturn(ResponseEntity.notFound().build());

        val detectedManagersReadableCache = mock(DetectedManagersCache.class);
        when(detectedManagersReadableCache.contains(any())).thenReturn(false);

        sut = new ManagerDeleteRequestCreator(serviceDetectorApiConsumer, detectedManagersReadableCache, null);

        val layer = mock(Layer.class);
        when(layer.getServicesExposingPath(any())).thenReturn(ImmutableList.of(new ServiceId("anyId")));

        val results = sut.create("/redfish/v1/Managers/1", null, null, null, null).execute(layer);

        assertEquals(results.length, 1);
        assertEquals(results[0].getHttpStatus(), METHOD_NOT_ALLOWED);
    }

    @Test
    public void whenRequestedUriIsRelatedWithServiceKnownForServiceDetector_204ShouldBeReturned() {
        val serviceDetectorApiConsumer = mock(ServiceDetectorApiConsumer.class);
        when(serviceDetectorApiConsumer.deleteManager(any())).thenReturn(noContent().build());

        val detectedManagersReadableCache = mock(DetectedManagersCache.class);
        when(detectedManagersReadableCache.contains(any())).thenReturn(true);

        sut = new ManagerDeleteRequestCreator(serviceDetectorApiConsumer, detectedManagersReadableCache, null);

        val layer = mock(Layer.class);
        when(layer.getServicesExposingPath(any())).thenReturn(emptyList());

        val requestedManagerUri = "/redfish/v1/Managers/" + randomUUID().toString();
        val results = sut.create(requestedManagerUri, null, null, null, null).execute(layer);

        assertEquals(results.length, 1);
        assertEquals(results[0].getHttpStatus(), NO_CONTENT);
    }


    @DataProvider
    private Object[][] allMethodsBesideDelete() {
        return Arrays.stream(HttpMethod.values()).filter(method -> !DELETE.equals(method)).map(method -> new Object[]{method}).toArray(Object[][]::new);
    }
}
