/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm;


import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;
import com.intel.podm.config.version.VersionLoader;
import com.intel.podm.discovery.external.ComposedNodeSanitizer;
import com.intel.podm.discovery.external.ComputerSystemSanitizer;
import com.intel.podm.discovery.external.ExternalServiceSanitizer;
import com.intel.podm.discovery.external.deep.DeepDiscoveryTaskScheduler;
import com.intel.podm.services.detection.ServiceDetector;
import org.testng.annotations.Test;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class ApplicationStartupTest {

    @Test
    public void whenStartingApplication_ShouldInitializePollingForServicesOnce() {
        ApplicationStartup sut = createApplicationStartup();

        sut.applicationStartup();

        verify(sut.serviceDetector, times(1)).pollForServices();
    }

    @Test
    public void whenStartingApplication_ShouldInitializeDeepDiscoverySchedulerOnce() {
        ApplicationStartup sut = createApplicationStartup();

        sut.applicationStartup();

        verify(sut.deepDiscoveryTaskScheduler, times(1)).start();
    }

    private ApplicationStartup createApplicationStartup() {
        ApplicationStartup sut = new ApplicationStartup();
        sut.logger = mock(Logger.class);
        sut.serviceDetector = mock(ServiceDetector.class);
        sut.versionLoader = mock(VersionLoader.class);
        sut.deepDiscoveryTaskScheduler = mock(DeepDiscoveryTaskScheduler.class);
        sut.computerSystemSanitizer = mock(ComputerSystemSanitizer.class);
        sut.composedNodeSanitizer = mock(ComposedNodeSanitizer.class);
        sut.externalServiceSanitizer = mock(ExternalServiceSanitizer.class);

        sut.discoveryConfig = mock(Holder.class);
        DiscoveryConfig config = mock(DiscoveryConfig.class);
        when(sut.discoveryConfig.get()).thenReturn(config);

        when(config.isDeepDiscoveryEnabled()).thenReturn(true);

        return sut;
    }
}
