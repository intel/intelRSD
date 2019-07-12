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

package com.intel.rsd.nodecomposer.discovery.external;

import com.intel.rsd.nodecomposer.ModelState;
import com.intel.rsd.nodecomposer.discovery.external.restgraph.RestGraph;
import com.intel.rsd.nodecomposer.discovery.external.restgraph.RestGraphBuilder;
import com.intel.rsd.services.common.AutoConfigureTestDiscoveryClient;
import lombok.SneakyThrows;
import lombok.val;
import org.mockito.Mockito;
import org.mockito.verification.VerificationMode;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.jdbc.AutoConfigureTestDatabase;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.context.ApplicationEventPublisher;
import org.springframework.scheduling.TaskScheduler;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.time.LocalDateTime;

import static com.intel.rsd.nodecomposer.LocalDateTimeProvider.now;
import static com.intel.rsd.nodecomposer.LocalDateTimeProvider.setFixedClockAt;
import static com.intel.rsd.nodecomposer.LocalDateTimeProvider.setSystemDefaultZoneClock;
import static com.intel.rsd.nodecomposer.discovery.external.DiscoveryEvent.EventType.RETRY_REQUESTED;
import static java.time.Duration.ofMillis;
import static java.util.Collections.emptySet;
import static org.assertj.core.api.Assertions.assertThat;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.when;
import static org.springframework.boot.test.context.SpringBootTest.WebEnvironment.RANDOM_PORT;

@ActiveProfiles("test")
@SpringBootTest(webEnvironment = RANDOM_PORT)
@EnableDiscoveryClient(autoRegister = false)
@AutoConfigureTestDatabase
@AutoConfigureTestDiscoveryClient
@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:VariableDeclarationUsageDistance"})
public class ResourceManagerScannerTest extends AbstractTestNGSpringContextTests {

    private static final DiscoveryEvent RETRY_REQUESTED_EVENT = new DiscoveryEvent(RETRY_REQUESTED);

    @Autowired
    private ModelState modelState;

    @BeforeMethod
    public void beforeMethod() {
        setFixedClockAt(LocalDateTime.now());
        assertThat(modelState.isNewerThan(now())).isFalse();
    }

    @AfterClass
    public void afterClass() {
        setSystemDefaultZoneClock();
    }

    /*-
     *                   ____________
     *    __________    | Discovery |
     *   |Operation |   |           |
     * --|----------v---|---------- v----------------------------------------------->
     */
    @Test
    public void whenDiscoveryRunsAfterBlockingOperation() throws Exception {
        val now = now();
        val afterNow = now.plus(ofMillis(1));
        val discoveryController = new DiscoveryController();

        startDirtyingOperation();
        stopDirtyingOperation();
        discoveryController.start(afterNow);
        discoveryController.finish();
        discoveryController.verify(never()).publishEvent(RETRY_REQUESTED_EVENT);
    }

    /*-
     *               ____________
     *    __________|_  Discovery|
     *   |Operation | |          |
     * --|----------|-v----------v------------------------------------------------->
     */
    @Test
    public void whenDiscoveryStartsDuringAndEndsAfterBlockingOperation() throws Exception {
        val now = now();
        val discoveryController = new DiscoveryController();

        startDirtyingOperation();
        discoveryController.start(now);
        stopDirtyingOperation();
        discoveryController.finish();
        discoveryController.verify(times(1)).publishEvent(RETRY_REQUESTED_EVENT);
    }

    /*-
     *    _______________________________
     *   | Discovery  ______________     |
     *   |           |   Operation  |    |
     * --|-----------|--------------v----v----------------------------------------->
     */
    @Test
    public void whenDiscoveryStartsBeforeAndEndsAfterBlockingOperation() throws Exception {
        val now = now();
        val beforeNow = now.minus(ofMillis(100));
        val discoveryController = new DiscoveryController();

        discoveryController.start(beforeNow);
        startDirtyingOperation();
        stopDirtyingOperation();
        discoveryController.finish();
        discoveryController.verify(times(1)).publishEvent(RETRY_REQUESTED_EVENT);
    }

    /*-
     *    ______________
     *   | Discovery  _|____________
     *   |           | | Operation  |
     * --|-----------|-v------------v---------------------------------------------->
     */
    @Test
    public void whenDiscoveryStartsBeforeAndEndsDuringBlockingOperation() throws Exception {
        val now = now();
        val beforeNow = now.minus(ofMillis(100));
        val discoveryController = new DiscoveryController();

        discoveryController.start(beforeNow);
        startDirtyingOperation();
        discoveryController.finish();
        discoveryController.verify(times(1)).publishEvent(RETRY_REQUESTED_EVENT);
        stopDirtyingOperation();
    }

    /*-
     *                ____________
     *               | Discovery |
     *              _|___________|_______
     *    _________|_|_          |  Oper2|
     *   |Oper1    | | |         |       |
     * --|---------|-|-v---------v-------v------------------------------------------>
     */
    @Test
    public void whenDiscoveryStartsDuringOneBlockingOperationAndFinishDuringAnotherBlockingOperation() throws Exception {
        val now = now();
        val discoveryController = new DiscoveryController();
        val nextDiscoveryController = new DiscoveryController();

        runInNamedThread("T1", this::startDirtyingOperation);
        runInNamedThread("T2", this::startDirtyingOperation);
        discoveryController.start(now);
        runInNamedThread("T1", this::stopDirtyingOperation);
        discoveryController.finish();
        discoveryController.verify(times(1)).publishEvent(RETRY_REQUESTED_EVENT);
        runInNamedThread("T2", this::stopDirtyingOperation);

        nextDiscoveryController.start(now.plus(ofMillis(1)));
        nextDiscoveryController.finish();
        nextDiscoveryController.verify(never()).publishEvent(RETRY_REQUESTED_EVENT);
    }

    /*-
     *    __________________             ________________
     *   | Discovery       _|___________|_   Rescheduled |
     *   |                | | Operation | |  Discovery   |
     * --|----------------|-v-----------|-v--------------v----------------------------->
     */
    @Test
    public void whenDiscoveryStartsBeforeAndEndsDuringBlockingOperationAndThenSeveralRetriesRetriedDiscoveryStartsAgainDuringOperation() throws Exception {
        val now = now();
        val beforeNow = now.minus(ofMillis(100));
        val firstDiscoveryController = new DiscoveryController();
        val retriedDiscoveryController = new DiscoveryController();

        firstDiscoveryController.start(beforeNow);
        startDirtyingOperation();
        firstDiscoveryController.finish();
        firstDiscoveryController.verify(times(1)).publishEvent(RETRY_REQUESTED_EVENT);
        retriedDiscoveryController.start(now);
        stopDirtyingOperation();
        retriedDiscoveryController.finish();
        retriedDiscoveryController.verify(times(1)).publishEvent(RETRY_REQUESTED_EVENT);
    }


    private ResourceManagerScanner createResourceManagerScanner(ApplicationEventPublisher applicationEventPublisher,
                                                                LocalDateTime discoveryStart) throws Exception {

        val restGraphPersister = new RestGraphPersister(applicationEventPublisher, mock(EntityGraphMapper.class), modelState);
        val restGraph = new RestGraph(null, emptySet(), discoveryStart);
        val restGraphBuilder = mock(RestGraphBuilder.class);
        when(restGraphBuilder.build()).thenReturn(restGraph);

        return new ResourceManagerScanner(
            mock(TaskScheduler.class),
            0,
            applicationEventPublisher,
            restGraphBuilder,
            restGraphPersister
        );
    }

    private void stopDirtyingOperation() {
        modelState.arrived();
    }

    private void startDirtyingOperation() {
        modelState.register();
        assertThat(modelState.isNewerThan(now())).isTrue();
    }

    @SneakyThrows
    private void runInNamedThread(String threadName, Runnable runnable) {
        val thread = new Thread(threadName) {
            @Override
            public void run() {
                runnable.run();
            }
        };

        thread.start();
        thread.join();
    }

    private class DiscoveryController {
        private ApplicationEventPublisher applicationEventPublisher;
        private ResourceManagerScanner resourceManagerScanner;

        DiscoveryController() {
            applicationEventPublisher = spy(ApplicationEventPublisher.class);
        }

        void start(LocalDateTime discoveryStartTime) throws Exception {
            resourceManagerScanner = createResourceManagerScanner(applicationEventPublisher, discoveryStartTime);
        }

        void finish() {
            resourceManagerScanner.run();
        }

        ApplicationEventPublisher verify(VerificationMode verificationMode) {
            return Mockito.verify(applicationEventPublisher, verificationMode);
        }
    }

}
