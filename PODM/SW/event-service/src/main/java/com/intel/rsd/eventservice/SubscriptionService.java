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

package com.intel.rsd.eventservice;

import com.intel.rsd.dto.events.SubscriptionRequest;
import com.intel.rsd.eventservice.data.SubscriptionEntity;
import com.intel.rsd.eventservice.data.SubscriptionMapper;
import com.intel.rsd.eventservice.data.SubscriptionRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.Collection;
import java.util.Optional;

import static com.intel.rsd.eventservice.data.SubscriptionMapper.generateOdataId;
import static com.intel.rsd.eventservice.data.SubscriptionMapper.toSubscriptionEntity;
import static java.util.stream.Collectors.toList;

@Component
public class SubscriptionService {

    private SubscriptionRepository subscriptionRepository;

    @Autowired
    public SubscriptionService(SubscriptionRepository subscriptionRepository) {
        this.subscriptionRepository = subscriptionRepository;
    }

    public URI createSubscription(SubscriptionRequest subscriptionRequest) {
        SubscriptionEntity subscriptionEntity = toSubscriptionEntity(subscriptionRequest);
        subscriptionRepository.save(subscriptionEntity);
        String subscriptionOdataId = generateOdataId(subscriptionEntity.getId());
        return URI.create(subscriptionOdataId);
    }

    public Collection<Subscription> getAllSubscriptions() {
        return subscriptionRepository.findAll().stream().map(SubscriptionMapper::toSubscription).collect(toList());
    }

    public Subscription getOrThrowNotFoundException(Integer id) {
        Optional<SubscriptionEntity> subscriptionEntity = subscriptionRepository.findById(id);
        return subscriptionEntity.map(SubscriptionMapper::toSubscription).orElseThrow(NotFoundException::new);
    }

    public void removeOrThrowNotFoundException(Integer id) {
        Optional<SubscriptionEntity> subscriptionEntity = subscriptionRepository.findById(id);
        if (subscriptionEntity.isPresent()) {
            subscriptionRepository.delete(subscriptionEntity.get());
        } else {
            throw new NotFoundException();
        }
    }

    public void removeAll() {
        subscriptionRepository.deleteAll();
    }

}
