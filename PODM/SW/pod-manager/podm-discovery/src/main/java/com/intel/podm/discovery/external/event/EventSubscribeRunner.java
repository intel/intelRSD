/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.discovery.external.event;

import com.intel.podm.client.api.ExternalServiceApiReaderConnectionException;
import com.intel.podm.client.api.RedfishApiException;
import com.intel.podm.client.api.events.EventSubscriptionManager;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.discovery.external.ExternalServiceAvailabilityChecker;

import javax.ejb.Stateless;
import javax.inject.Inject;
import java.util.UUID;

@Stateless
public class EventSubscribeRunner {
    @Inject
    private Logger logger;

    @Inject
    private EventSubscriberCreator eventSubscriberCreator;

    @Inject
    private ExternalServiceAvailabilityChecker availabilityChecker;

    public boolean subscribeIfNotAlreadySubscribed(UUID serviceUuid) {
        try (EventSubscriptionManager eventSubscriptionManager = eventSubscriberCreator.create(serviceUuid)) {
            if (eventSubscriptionManager.subscribeIfNotAlreadySubscribed()) {
                logger.t("Successfully subscribed to event service for '" + serviceUuid + "'");
                return true;
            }
        } catch (ExternalServiceApiReaderConnectionException e) {
            logger.t("Connection error while checking subscriptions for service with UUID: " + serviceUuid + ", " + e.getMessage());
            availabilityChecker.verifyServiceAvailabilityByUuid(serviceUuid);
        } catch (RedfishApiException e) {
            logger.e("Error while subscribing to event service for '" + serviceUuid + "', error: " + e.getErrorResponse());
        } catch (IllegalArgumentException | UnsupportedOperationException e) {
            logger.e("Error while subscribing to event service for '" + serviceUuid + "': " + e.getMessage());
        }

        return false;
    }
}
