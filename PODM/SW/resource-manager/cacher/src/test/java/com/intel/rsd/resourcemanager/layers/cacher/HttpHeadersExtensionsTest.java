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

package com.intel.rsd.resourcemanager.layers.cacher;

import lombok.val;
import org.springframework.http.HttpHeaders;
import org.testng.annotations.Test;

import static com.intel.rsd.resourcemanager.layers.cacher.HttpHeadersExtensions.getMaxStale;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class HttpHeadersExtensionsTest {

    @Test
    public void getMaxStale_whenValidCacheControlWithMaxStaleProvided_thenResultShouldBePresent() throws Exception {
        HttpHeaders headers = createHeadersWithCacheControl("max-stale=12");

        val result = getMaxStale(headers);
        assertTrue(result.isPresent());
    }

    @Test
    public void getMaxStale_whenCacheControlWithInvalidSecondsInMaxStaleProvided_thenResultShouldNotBePresent() throws Exception {
        HttpHeaders headers = createHeadersWithCacheControl("max-stale=12k");

        val result = getMaxStale(headers);
        assertFalse(result.isPresent());
    }

    @Test
    public void getMaxStale_whenValidCacheControlWithMaxStaleAndDifferentDirectiveIsProvided_thenResultShouldBePresent() throws Exception {
        HttpHeaders headers = createHeadersWithCacheControl("max-stale=12,max-age=2");

        val result = getMaxStale(headers);
        assertTrue(result.isPresent());
    }

    @Test
    public void getMaxStale_whenCacheControlWithInvalidInMaxStaleProvided__thenResultShouldNotBePresent() throws Exception {
        HttpHeaders headers = createHeadersWithCacheControl("max stale 12");

        val result = getMaxStale(headers);
        assertFalse(result.isPresent());
    }

    private HttpHeaders createHeadersWithCacheControl(String cacheControl) {
        val headers = new HttpHeaders();
        headers.setCacheControl(cacheControl);
        return headers;
    }
}
