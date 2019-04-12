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

package com.intel.rsd.eventservice.data;

import com.intel.rsd.dto.events.SubscriptionRequest;
import com.intel.rsd.eventservice.Subscription;
import org.springframework.beans.BeanUtils;

import static java.lang.String.format;

public class SubscriptionMapper {

    private static final String SUBSCRIPTION_ODATA_ID_PREFIX = "/redfish/v1/EventService/Subscriptions/";

    public static SubscriptionEntity toSubscriptionEntity(SubscriptionRequest subscriptionRequest) {
        SubscriptionEntity subscriptionEntity = new SubscriptionEntity();
        BeanUtils.copyProperties(subscriptionRequest, subscriptionEntity);
        return subscriptionEntity;
    }

    public static Subscription toSubscription(SubscriptionEntity subscriptionEntity) {
        Integer id = subscriptionEntity.getId();
        return new Subscription(
            generateOdataId(id),
            id,
            toSubscriptionRequest(subscriptionEntity));
    }

    public static String generateOdataId(Integer id) {
        return format("%s%d", SUBSCRIPTION_ODATA_ID_PREFIX, id);
    }

    private static SubscriptionRequest toSubscriptionRequest(SubscriptionEntity entity) {
        SubscriptionRequest subscriptionRequest = new SubscriptionRequest();
        BeanUtils.copyProperties(entity, subscriptionRequest);
        return subscriptionRequest;
    }

}
