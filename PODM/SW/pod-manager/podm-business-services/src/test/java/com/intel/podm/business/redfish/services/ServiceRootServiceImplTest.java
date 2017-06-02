/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.dto.redfish.ServiceRootDto;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;
import com.intel.podm.config.network.NetworkConfigurationIOException;
import org.testng.annotations.Test;

import java.util.UUID;

import static java.util.UUID.randomUUID;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MethodName"})
public class ServiceRootServiceImplTest {
    @Test
    public void whenConstructed_ShouldBeAbleToReturnServiceRootWithValidUuid() throws NetworkConfigurationIOException {
        ServiceRootServiceImpl service = createService();
        UUID expectedUuid = randomUUID();
        when(service.config.get().getUuid()).thenReturn(expectedUuid);

        ServiceRootDto serviceRoot = service.getServiceRoot();
        assertEquals(serviceRoot.getUuid(), expectedUuid);
    }

    @Test
    public void whenGettingServiceRoot_ShouldReturnSameUuidOnSubsequentInvocations() {
        ServiceRootServiceImpl service = createService();
        when(service.config.get().getUuid()).thenReturn(randomUUID());
        ServiceRootDto serviceRootFirstInvocation = service.getServiceRoot();
        ServiceRootDto serviceRootSecondInvocation = service.getServiceRoot();
        assertEquals(serviceRootFirstInvocation.getUuid(), serviceRootSecondInvocation.getUuid());
    }

    private ServiceRootServiceImpl createService() {
        ServiceRootServiceImpl service = new ServiceRootServiceImpl();
        service.config = mock(Holder.class);
        when(service.config.get()).thenReturn(mock(ServiceConfig.class));
        return service;
    }
}
