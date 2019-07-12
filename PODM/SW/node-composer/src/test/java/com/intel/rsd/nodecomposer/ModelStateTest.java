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

package com.intel.rsd.nodecomposer;

import com.intel.rsd.services.common.AutoConfigureTestDiscoveryClient;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.jdbc.AutoConfigureTestDatabase;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.time.LocalDateTime;

import static com.intel.rsd.nodecomposer.LocalDateTimeProvider.setFixedClockAt;
import static com.intel.rsd.nodecomposer.LocalDateTimeProvider.setSystemDefaultZoneClock;
import static java.time.Duration.ofMillis;
import static org.assertj.core.api.Assertions.assertThat;
import static org.springframework.boot.test.context.SpringBootTest.WebEnvironment.RANDOM_PORT;

@ActiveProfiles("test")
@SpringBootTest(webEnvironment = RANDOM_PORT)
@EnableDiscoveryClient(autoRegister = false)
@AutoConfigureTestDatabase
@AutoConfigureTestDiscoveryClient
public class ModelStateTest extends AbstractTestNGSpringContextTests {

    @Autowired
    ModelState modelState;

    @BeforeClass
    public void beforeClass() {
        setFixedClockAt(LocalDateTime.now());
    }

    @AfterClass
    public void afterClass() {
        setSystemDefaultZoneClock();
    }

    @Test(dataProvider = "afterRegistrationModelStateIsDirty")
    public void afterRegistrationModelStateIsDirty(LocalDateTime localDateTime, String desc) {
        try {
            modelState.register();
            assertThat(modelState.isNewerThan(localDateTime)).describedAs(desc).isTrue();
        } finally {
            modelState.arrived();
        }
    }

    @Test(dataProvider = "afterRegistrationAndArrivalModelStateIsInAppropriateState")
    public void afterRegistrationAndArrivalModelStateIsInAppropriateState(LocalDateTime time, String desc, boolean expectedState) {
        try {
            modelState.register();
        } finally {
            modelState.arrived();
            assertThat(modelState.isNewerThan(time)).describedAs(String.format("%s, %s", desc, modelState)).isEqualTo(expectedState);
        }
    }

    @DataProvider
    private Object[][] afterRegistrationModelStateIsDirty() {
        return new Object[][]{
            new Object[]{LocalDateTimeProvider.now().plus(ofMillis(1)), "Model has to be dirty when requested with FUTURE time"},
            new Object[]{LocalDateTimeProvider.now().minus(ofMillis(1)), "Model has to be dirty when requested with PAST time"},
        };
    }

    @DataProvider
    private Object[][] afterRegistrationAndArrivalModelStateIsInAppropriateState() {
        return new Object[][]{
            new Object[]{LocalDateTimeProvider.now().minus(ofMillis(1)), "Model has to be dirty when requested with PAST time", true},
            new Object[]{LocalDateTimeProvider.now(), "Model has to be dirty when requested with 'now' time", true},
            new Object[]{LocalDateTimeProvider.now().plus(ofMillis(1)), "Model has to be clean when requested with FUTURE time", false},
        };
    }
}
