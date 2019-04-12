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

package com.intel.rsd.resourcemanager.layers.unifier.unification;

import org.springframework.http.HttpHeaders;
import org.testng.annotations.Test;

import static java.net.URI.create;
import static java.util.Optional.of;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.springframework.http.ResponseEntity.created;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

public class HttpHeadersUnifierTest {

    private HttpHeadersUnifier sut = new HttpHeadersUnifier();

    @Test(expectedExceptions = NullPointerException.class)
    public void whenCalledWithNullAsInputHeaders_exceptionShouldBeThrown() throws Exception {
        sut.unify(null, null);
    }

    @Test
    public void whenCalledWithEmptyHeaders_emptyHttpHeadersShouldBeReturned() throws Exception {
        HttpHeaders headers = new HttpHeaders();
        UnifiedOdataIdResolver mapper = mock(UnifiedOdataIdResolver.class);
        HttpHeaders unifiedHeaders = sut.unify(headers, mapper);
        assertTrue(unifiedHeaders.isEmpty());
    }

    @Test
    public void whenAnySupportedHeaderIsGiven_resultShouldContainsThisHeader() throws Exception {
        HttpHeaders inputHeaders = new HttpHeaders();
        inputHeaders.add("supportedHeader", "anyValue");
        UnifiedOdataIdResolver mapper = mock(UnifiedOdataIdResolver.class);
        HttpHeaders unifiedHeaders = sut.unify(inputHeaders, mapper);
        assertEquals(unifiedHeaders, inputHeaders);
    }

    @Test
    public void whenLocationHeaderIsGiven_resultShouldContainUnifiedVersionOfLocationHeader() throws Exception {
        String initialLocation = "/initial/location";
        String unifiedLocation = "/unified/location";
        HttpHeaders inputHeaders = created(create(initialLocation)).build().getHeaders();
        HttpHeaders expectedHeaders = created(create(unifiedLocation)).build().getHeaders();
        UnifiedOdataIdResolver mapper = mock(UnifiedOdataIdResolver.class);
        when(mapper.toUnified(any())).thenReturn(of(unifiedLocation));
        HttpHeaders unifiedHeaders = sut.unify(inputHeaders, mapper);
        assertEquals(unifiedHeaders, expectedHeaders);
    }
}
