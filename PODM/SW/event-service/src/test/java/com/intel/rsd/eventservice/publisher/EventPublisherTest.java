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

package com.intel.rsd.eventservice.publisher;

import com.intel.rsd.dto.events.Event;
import com.intel.rsd.dto.events.SubscriptionRequest;
import com.intel.rsd.eventservice.EventServiceConfiguration;
import com.intel.rsd.eventservice.Subscription;
import com.intel.rsd.eventservice.SubscriptionService;
import com.intel.rsd.eventservice.publisher.Schedulers.TestSchedulers;
import io.reactivex.schedulers.TestScheduler;
import lombok.val;
import org.mockito.Mock;
import org.springframework.web.client.RestTemplate;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import static io.reactivex.plugins.RxJavaPlugins.setComputationSchedulerHandler;
import static java.util.concurrent.TimeUnit.SECONDS;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;

public class EventPublisherTest {

    @Mock
    private RestTemplate restTemplate;

    @Mock
    private SubscriptionService subscriptionService;

    @BeforeMethod
    public void setUp(Object[] testParameters) {
        initMocks(this);
    }

    @Test(dataProvider = "eventServiceConfigurations")
    public void whenEventCannotBeDeliveredToTheSubscriber_subscriptionShouldBeDeregistered(int deliveryRetryAttempts, int deliveryRetryIntervalSeconds) {

        when(restTemplate.postForEntity(any(), any(), any())).thenThrow(new RuntimeException());

        val subscription = mock(Subscription.class);
        when(subscription.getSubscriptionRequest()).thenReturn(mock(SubscriptionRequest.class));

        val testScheduler = new TestScheduler();
        setComputationSchedulerHandler(current -> testScheduler);

        val eventServiceConfiguration = new EventServiceConfiguration();
        eventServiceConfiguration.setDeliveryRetryAttempts(deliveryRetryAttempts);
        eventServiceConfiguration.setDeliveryRetryIntervalSeconds(deliveryRetryIntervalSeconds);

        val sut = new EventPublisher(restTemplate, subscriptionService, eventServiceConfiguration, new TestSchedulers());
        val envelope = new Envelope(subscription, mock(Event.class));
        val testObserver = sut.createRetryablePublishingStream(envelope).test();

        testScheduler.advanceTimeBy(determineTotalTimeOfAllRetries(eventServiceConfiguration), SECONDS);

        testObserver.assertNoValues();
        verify(subscriptionService).removeOrThrowNotFoundException(any());
        verify(restTemplate, times(eventServiceConfiguration.getDeliveryRetryAttempts() + 1)).postForEntity(any(), any(), any());
    }

    private int determineTotalTimeOfAllRetries(EventServiceConfiguration eventServiceConfiguration) {
        return eventServiceConfiguration.getDeliveryRetryAttempts() * eventServiceConfiguration.getDeliveryRetryIntervalSeconds();
    }

    @DataProvider
    @SuppressWarnings("checkstyle:MagicNumber")
    private static Object[][] eventServiceConfigurations() {
        return new Object[][]{
            new Object[]{0, 0},
            new Object[]{2, 0},
            new Object[]{2, 1},
            new Object[]{2, 10},
            new Object[]{3, 20},
            new Object[]{10, 30}
        };
    }
}
