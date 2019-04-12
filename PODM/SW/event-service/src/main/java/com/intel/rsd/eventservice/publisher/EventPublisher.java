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

import com.intel.rsd.eventservice.EventServiceConfiguration;
import com.intel.rsd.eventservice.NotFoundException;
import com.intel.rsd.eventservice.Subscription;
import com.intel.rsd.eventservice.SubscriptionService;
import io.reactivex.Observable;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.web.client.RestTemplate;

import java.net.URI;
import java.util.concurrent.atomic.AtomicInteger;

import static io.reactivex.Observable.just;
import static java.util.concurrent.TimeUnit.SECONDS;

@Component
@Slf4j
class EventPublisher {

    private final RestTemplate restTemplate;
    private final SubscriptionService subscriptionService;
    private final EventServiceConfiguration eventServiceConfiguration;
    private Schedulers schedulers;

    @Autowired
    EventPublisher(RestTemplate restTemplate,
                   SubscriptionService subscriptionService,
                   EventServiceConfiguration configuration,
                   Schedulers schedulers) {

        this.restTemplate = restTemplate;
        this.subscriptionService = subscriptionService;
        this.eventServiceConfiguration = configuration;
        this.schedulers = schedulers;
    }

    void publish(Envelope envelope) {
        createRetryablePublishingStream(envelope)
            .subscribe(
                o -> log.info("Event Delivered({})", o.getEvent().getId()),
                e -> log.info("Event NOT Delivered({})", envelope.getEvent().getId())
            );
    }

    Observable<Envelope> createRetryablePublishingStream(Envelope envelope) {
        return just(envelope)
            .compose(schedulers.applySchedulers())
            .doOnNext(this::deliver)
            .retryWhen(this::getRetryHandler)
            .doOnError(throwable -> unsubscribe(envelope.getSubscription()));

    }

    private Observable<Long> getRetryHandler(Observable<Throwable> throwableObservable) {
        AtomicInteger counter = new AtomicInteger();

        return throwableObservable.takeWhile(throwable -> {
            if (counter.incrementAndGet() <= eventServiceConfiguration.getDeliveryRetryAttempts()) {
                return true;
            }
            throw new RuntimeException("No more retries");
        }).flatMap(
            retryNo -> Observable.timer(eventServiceConfiguration.getDeliveryRetryIntervalSeconds(), SECONDS)
        );

    }

    private void deliver(@NonNull Envelope envelope) {
        URI destination = envelope.getSubscription().getSubscriptionRequest().getDestination();
        log.info("Delivering event: {} to {}", envelope.getEvent().getId(), destination);
        restTemplate.postForEntity(destination, envelope.getEvent(), Void.class);
    }

    private void unsubscribe(@NonNull Subscription subscription) {
        log.info("Removing subscription: {}", subscription.getOdataId());
        try {
            subscriptionService.removeOrThrowNotFoundException(subscription.getId());
        } catch (NotFoundException e) {
            log.warn("Subscription with id {} is already unregistered", subscription.getId());
        }
    }
}
