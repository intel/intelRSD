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

package com.intel.rsd.resourcemanager.common;

import lombok.val;
import org.springframework.cloud.client.loadbalancer.LoadBalanced;
import org.testng.annotations.Test;

import java.io.IOException;

import static org.mockito.Mockito.spy;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ResourceManagerConfigurationTest {

    @Test
    public void testLoadBalancedRestTemplate() throws IOException {
        val resourceManagerConfiguration = new ResourceManagerConfiguration();
        val sut = spy(resourceManagerConfiguration.loadBalancedRestTemplate());
        assertFalse(sut.getErrorHandler().hasError(null));
    }

    @Test
    public void testLoadBalancedRestTemplateMethodIsAnnotatedAsLoadBalanced() throws NoSuchMethodException {
        assertTrue(ResourceManagerConfiguration.class.getMethod("loadBalancedRestTemplate").isAnnotationPresent(LoadBalanced.class));
    }
}
