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

package com.intel.rsd.service.detector.file.serviceslist;

import com.intel.rsd.service.detector.file.ServiceCandidate;
import com.intel.rsd.service.detector.file.ServiceCandidatesProvider;
import com.intel.rsd.service.detector.file.ServicesFileReader;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Profile;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import java.net.URI;
import java.nio.file.Path;
import java.util.Collection;
import java.util.List;
import java.util.stream.Stream;

import static java.nio.file.Files.exists;
import static java.nio.file.Paths.get;
import static java.util.stream.Stream.empty;

@Component
@Profile("serviceslist")
@Slf4j
public class ServiceListServiceCandidateProvider implements ServiceCandidatesProvider {

    private static final String SERVICES_LIST_LOCATION = "/tmp/services.list";

    @Autowired
    ServiceListFileRecordsParser serviceListFileRecordsParser;

    @PostConstruct
    private void init() {
        log.info("{} monitoring initialized...", SERVICES_LIST_LOCATION);
    }

    @Override
    public Stream<ServiceCandidate> getServiceCandidates() {
        Path servicesListPath = get(SERVICES_LIST_LOCATION);
        if (!exists(servicesListPath)) {
            return empty();
        }

        List<String> fileRecords = new ServicesFileReader(servicesListPath).readServicesFile();
        Collection<ServiceListRecord> records = serviceListFileRecordsParser.toServiceListRecords(fileRecords);
        return records.stream().map(record -> new ServiceCandidate(URI.create(record.getUrl()), null));
    }
}
