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

package com.intel.podm.discovery.external;

import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderConnectionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ExternalServiceReader;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;
import java.util.UUID;

@Dependent
public class DiscoveryRunner {
    @Inject
    private Logger logger;

    @Inject
    private Discovery discovery;

    @Inject
    private ExternalServiceAvailabilityChecker availabilityChecker;

    @Inject
    private ReaderFactory readerFactory;

    @Inject
    private ExternalServiceSanitizer externalServiceSanitizer;

    public void run(UUID serviceUuid) {
        logger.i("Polling data from '{}' started", serviceUuid);

        try (ExternalServiceReader reader = readerFactory.create(serviceUuid)) {
            UUID uuidFromApi = reader.getServiceRoot().getUuid();
            if (!Objects.equals(serviceUuid, uuidFromApi)) {
                externalServiceSanitizer.sanitizeService(serviceUuid, uuidFromApi);
                logger.w("Service '{}' changed it's UUID to '{}'", serviceUuid, uuidFromApi);
            }

            discovery.discover(reader);
            logger.i("Polling data from '{}' finished", serviceUuid);
        } catch (ExternalServiceApiReaderConnectionException e) {
            logger.w("Connection error while getting data from '{}' service - performing check on this service", serviceUuid);
            availabilityChecker.verifyServiceAvailabilityByUuid(serviceUuid);
        } catch (ExternalServiceApiReaderException e) {
            logger.w("Unable to process data from '{}' service, error: {}", serviceUuid, e.getErrorResponse());
        } catch (RuntimeException e) {
            logger.e("Error while polling data from '" + serviceUuid + "'", e);
        } catch (ExternalServiceApiActionException e) {
            logger.e("Unable to delete existing subscriptions for service '" + serviceUuid + "', error: " + e.getErrorResponse(), e);
        }
    }
}
