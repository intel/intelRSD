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

package com.intel.podm.business.entities.redfish;

import org.testng.annotations.Test;

import java.time.Duration;

import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName"})
public class ExternalServiceTest {

    private static final int MIN_DELAY = 1;
    private static final int DAYS = 1000;

    @Test
    public void whenCreated_shouldBeReachable() {
        ExternalService sut = new ExternalService();

        assertTrue(sut.isReachable());
        assertFalse(sut.isUnreachableLongerThan(Duration.ZERO));
        assertFalse(sut.isUnreachableLongerThan(Duration.ofDays(DAYS)));
    }

    @Test
    public void whenMarkedAsUnreachable_shouldBeUnreachable() throws InterruptedException {
        ExternalService sut = new ExternalService();

        sut.markAsNotReachable();

        Thread.sleep(MIN_DELAY);

        assertFalse(sut.isReachable());
        assertTrue(sut.isUnreachableLongerThan(Duration.ZERO));
        assertFalse(sut.isUnreachableLongerThan(Duration.ofDays(DAYS)));
    }

    @Test(dependsOnMethods = "whenMarkedAsUnreachable_shouldBeUnreachable")
    public void whenMarkedAsUnreachableAndThenReachable_shouldBeReachable() throws InterruptedException {
        ExternalService sut = new ExternalService();

        sut.markAsNotReachable();
        sut.markAsReachable();

        Thread.sleep(MIN_DELAY);

        assertTrue(sut.isReachable());
        assertFalse(sut.isUnreachableLongerThan(Duration.ZERO));
        assertFalse(sut.isUnreachableLongerThan(Duration.ofDays(DAYS)));
    }
}
