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

package com.intel.podm.services.detection;

import com.intel.podm.common.logger.Logger;
import org.testng.annotations.Test;

import javax.enterprise.concurrent.ManagedScheduledExecutorService;
import java.util.concurrent.RejectedExecutionException;

import static org.mockito.Matchers.any;
import static org.mockito.Matchers.anyLong;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;


public class ServiceDetectorTest {

    @Test(expectedExceptions = UnsupportedOperationException.class)
    public void subsequentCallsOfPollForServicesMethod_ShouldThrow() throws ServiceDetectionException {

        ServiceDetector serviceDetector = new ServiceDetector();
        serviceDetector.managedExecutorService = mock(ManagedScheduledExecutorService.class);

        serviceDetector.pollForServices();
        serviceDetector.pollForServices();
    }

    @Test(expectedExceptions = RejectedExecutionException.class)
    public void whenStartingPollingTaskViaManagedExecutorServiceFails_ShouldThrow() throws ServiceDetectionException {
        ServiceDetector serviceDetector = new ServiceDetector();
        serviceDetector.managedExecutorService = mock(ManagedScheduledExecutorService.class);
        serviceDetector.logger = mock(Logger.class);

        doThrow(new RejectedExecutionException("Test throw"))
                .when(serviceDetector.managedExecutorService)
                .scheduleWithFixedDelay(any(), anyLong(), anyLong(), any());

        serviceDetector.pollForServices();
    }
}
