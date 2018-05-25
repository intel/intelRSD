/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.services.detection.ssdp.transport.impl;

import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceDetectionConfig;
import com.intel.podm.config.base.dto.ServiceDetectionConfig.Protocols;
import com.intel.podm.config.base.dto.ServiceDetectionConfig.Protocols.Ssdp;
import org.apache.commons.net.util.SubnetUtils;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import static com.google.common.collect.Lists.newArrayList;
import static java.net.InetAddress.getByName;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.testng.Assert.assertEquals;

public class FilteredDatagramProcessorImplTest {

    public static final String SUBNET_192_168_1_1_MASK_24 = "192.168.1.1/24";

    public static final String SUBNET_0_0_0_0_MASK_0 = "0.0.0.0/0";

    @InjectMocks
    private FilteredDatagramProcessorImpl underTest;

    @Mock
    private Holder<ServiceDetectionConfig> serviceDetectionConfigHolder;

    @Mock
    private ServiceDetectionConfig serviceDetectionConfig;

    @Mock
    private Protocols protocols;

    @Mock
    private Ssdp ssdp;


    @BeforeMethod
    public void setUp() throws Exception {
        initMocks(this);
        when(serviceDetectionConfigHolder.get()).thenReturn(serviceDetectionConfig);
        when(serviceDetectionConfig.getProtocols()).thenReturn(protocols);
        when(protocols.getSsdp()).thenReturn(ssdp);
    }

    @Test(dataProvider = "testCheckNetworkOriginProvider")
    public void testCheckNetworkOrigin(boolean expectedResult, String ipAddress, String subnet) throws Exception {
        when(ssdp.getSubnets()).thenReturn(newArrayList(new SubnetUtils(subnet)));
        assertEquals(underTest.checkNetworkOrigin(getByName(ipAddress)), expectedResult);
    }

    @DataProvider
    private Object[][] testCheckNetworkOriginProvider() {
        return new Object[][]{
                {true, "192.168.1.1", SUBNET_192_168_1_1_MASK_24},
                {true, "192.168.1.254", SUBNET_192_168_1_1_MASK_24},
                {true, "192.168.1.25", SUBNET_192_168_1_1_MASK_24},
                {false, "192.168.1.255", SUBNET_192_168_1_1_MASK_24},
                {false, "192.168.2.25", SUBNET_192_168_1_1_MASK_24},
                {true, "1.2.3.4", SUBNET_0_0_0_0_MASK_0},
                {true, "100.100.100.254", SUBNET_0_0_0_0_MASK_0},
        };
    }
}
