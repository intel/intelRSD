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

package com.intel.podm.services.detection.dhcp;

import com.intel.podm.services.detection.dhcp.filesystem.ServiceListFileRecordsParser;
import com.intel.podm.services.detection.dhcp.filesystem.ServiceListRecord;
import com.intel.podm.services.detection.dhcp.filesystem.ServicesFileReader;
import com.intel.podm.services.detection.dhcp.filesystem.TmpLeasesFileRecordsParser;
import com.intel.podm.services.detection.dhcp.filesystem.TmpLeasesRecord;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.nio.file.Path;
import java.util.Collection;
import java.util.List;
import java.util.Set;
import java.util.stream.Stream;

import static java.nio.file.Files.exists;
import static java.nio.file.Paths.get;
import static java.util.stream.Collectors.toSet;
import static java.util.stream.Stream.empty;

@Dependent
public class CandidatesProvider {

    private static final String TMP_LEASES_LOCATION = "/tmp/leases";
    private static final String SERVICES_LIST_LOCATION = "/tmp/services.list";

    @Inject
    UrlProvider urlProvider;

    @Inject
    TmpLeasesFileRecordsParser tmpLeasesFileRecordsParser;

    @Inject
    ServiceListFileRecordsParser serviceListFileRecordsParser;

    public Set<DhcpServiceCandidate> getEndpointCandidates() {
        return Stream.concat(getTmpLeasesCandidates(), getServicesListCandidates()).collect(toSet());
    }

    public Stream<DhcpServiceCandidate> getTmpLeasesCandidates() {
        Path tmpLeasesPath = get(TMP_LEASES_LOCATION);
        if (!exists(tmpLeasesPath)) {
            return empty();
        }

        List<String> fileRecords = new ServicesFileReader(tmpLeasesPath).readServicesFile();
        Collection<TmpLeasesRecord> records = tmpLeasesFileRecordsParser.toTmpLeasesRecordCollection(fileRecords);
        return records.stream().map(record ->
                new DhcpServiceCandidate(record.getServiceType(),
                        urlProvider.getEndpointUri(record), record.getDate()));
    }

    public Stream<DhcpServiceCandidate> getServicesListCandidates() {
        Path servicesListPath = get(SERVICES_LIST_LOCATION);
        if (!exists(servicesListPath)) {
            return empty();
        }

        List<String> fileRecords = new ServicesFileReader(servicesListPath).readServicesFile();
        Collection<ServiceListRecord> records = serviceListFileRecordsParser.toServiceListRecords(fileRecords);
        return records.stream().map(record ->
                new DhcpServiceCandidate(record.getServiceType(),
                        urlProvider.getEndpointUri(record.getUrl()), null));
    }
}
