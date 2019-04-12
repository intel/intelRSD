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

package com.intel.rsd.service.detector.file.dhcp;

import com.intel.rsd.service.detector.file.ServiceCandidate;
import com.intel.rsd.service.detector.file.ServiceCandidatesProvider;
import com.intel.rsd.service.detector.file.ServicesFileReader;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Profile;
import org.springframework.stereotype.Component;
import org.springframework.web.util.UriComponentsBuilder;

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
@Slf4j
@Profile("dhcp")
public class TmpLeaseServiceCandidateProvider implements ServiceCandidatesProvider {
    private static final String TMP_LEASES_LOCATION = "/var/lib/dhcp/leases";

    private static final String DEFAULT_ROOT = "/redfish/v1";
    private static final String HTTPS_SCHEME = "https";
    private static final int DEFAULT_PORT = 8443;

    @Autowired
    private TmpLeasesFileRecordsParser tmpLeasesFileRecordsParser;

    @PostConstruct
    private void init() {
        log.info("{} monitoring initialized...", TMP_LEASES_LOCATION);
    }

    @Override
    public Stream<ServiceCandidate> getServiceCandidates() {
        Path tmpLeasesPath = get(TMP_LEASES_LOCATION);
        if (!exists(tmpLeasesPath)) {
            return empty();
        }

        List<String> fileRecords = new ServicesFileReader(tmpLeasesPath).readServicesFile();
        Collection<TmpLeasesRecord> records = tmpLeasesFileRecordsParser.toTmpLeasesRecordCollection(fileRecords);
        return records.stream().map(record ->
            new ServiceCandidate(getEndpointUri(record), record.getDate()));
    }

    public URI getEndpointUri(TmpLeasesRecord record) {
        return getUriFromTmpLeasesRecord(record);
    }

    private URI getUriFromTmpLeasesRecord(TmpLeasesRecord record) {
        return UriComponentsBuilder.newInstance()
            .scheme(HTTPS_SCHEME)
            .host(record.getIpAddress())
            .port(DEFAULT_PORT)
            .path(DEFAULT_ROOT).build().toUri();
    }
}
