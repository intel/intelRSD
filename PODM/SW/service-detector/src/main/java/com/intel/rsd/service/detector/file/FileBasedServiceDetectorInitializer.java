/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.service.detector.file;

import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Profile;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;

@Slf4j
@Component
@Profile({"dhcp", "serviceslist"})
public class FileBasedServiceDetectorInitializer {

    @Autowired
    private ServiceChecker serviceChecker;

    @PostConstruct
    private void init() {
        log.info("File based service detector initialized...");
    }

    @Scheduled(
            initialDelayString = "#{T(java.util.concurrent.TimeUnit).SECONDS.toMillis(${protocols.dhcp.FilesCheckIntervalInSeconds})}",
            fixedDelayString = "#{T(java.util.concurrent.TimeUnit).SECONDS.toMillis(${protocols.dhcp.FilesCheckIntervalInSeconds})}"
    )
    private void provideEndpoints() {
        serviceChecker.retryFailedEndpointCandidates();
        serviceChecker.triggerEndpointCandidatesCheck();
    }

    @Scheduled(
            initialDelayString = "#{T(java.util.concurrent.TimeUnit).SECONDS.toMillis(${FailedEndpointRecheckIntervalInSeconds})}",
            fixedDelayString = "#{T(java.util.concurrent.TimeUnit).SECONDS.toMillis(${FailedEndpointRecheckIntervalInSeconds})}"
    )
    private void setRecheckFailedUris() {
        serviceChecker.reCheckForFailedUris();
    }
}
