/*
 * Copyright (c) 2015-2018 Intel Corporation
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
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.types.ServiceType.DISCOVERY_SERVICE;
import static com.intel.podm.common.types.ServiceType.RSS;
import static java.nio.file.Files.exists;
import static java.nio.file.Paths.get;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toSet;

@Dependent
public class CandidatesProvider {

    private static final String TMP_LEASES_LOCATION = "/tmp/leases";
    private static final String SERVICES_LIST_LOCATION = "/tmp/services.list";

    @Inject
    private UrlProvider urlProvider;

    @Inject
    private TmpLeasesFileRecordsParser tmpLeasesFileRecordsParser;

    @Inject
    private ServiceListFileRecordsParser serviceListFileRecordsParser;

    Set<DhcpServiceCandidate> getEndpointCandidates() {
        Set<DhcpServiceCandidate> serviceCandidate = new HashSet<>();
        serviceCandidate.addAll(getTmpLeasesCandidates());
        serviceCandidate.addAll(getServicesListCandidates());
        return serviceCandidate;
    }

    private Set<DhcpServiceCandidate> getTmpLeasesCandidates() {
        Path tmpLeasesPath = get(TMP_LEASES_LOCATION);
        if (!exists(tmpLeasesPath)) {
            return emptySet();
        }
        List<String> fileRecords = new ServicesFileReader(tmpLeasesPath).readServicesFile();
        Collection<TmpLeasesRecord> records = tmpLeasesFileRecordsParser.toTmpLeasesRecordCollection(fileRecords);
        Collection<TmpLeasesRecord> optionalRecords = createOptionalDiscoveryServiceCandidate(records);

        Set<DhcpServiceCandidate> candidates = records.stream().flatMap(record ->
            urlProvider.getEndpointUris(record).stream().map(uri ->
                new DhcpServiceCandidate(record.getServiceType(), uri, record.getDate()))
        ).collect(toSet());

        Set<DhcpServiceCandidate> optionalCandidates = optionalRecords.stream().flatMap(record ->
            urlProvider.getEndpointUris(record).stream().map(uri ->
                new DhcpOptionalServiceCandidate(record.getServiceType(), uri, record.getDate()))
        ).filter(optionalCandidate -> candidates.stream().noneMatch(candidate ->
            candidate.getEndpointUri().equals(optionalCandidate.getEndpointUri()))
        ).collect(toSet());

        candidates.addAll(optionalCandidates);
        return candidates;
    }

    private Collection<TmpLeasesRecord> createOptionalDiscoveryServiceCandidate(Collection<TmpLeasesRecord> records) {
        return records.stream().
            filter(tm -> RSS.equals(tm.getServiceType())).
            map(tmpLeasesRecord -> new TmpLeasesRecord(DISCOVERY_SERVICE, tmpLeasesRecord.getIpAddress(), tmpLeasesRecord.getDate())).
            collect(toSet());
    }

    private Set<DhcpServiceCandidate> getServicesListCandidates() {
        Path servicesListPath = get(SERVICES_LIST_LOCATION);
        if (!exists(servicesListPath)) {
            return emptySet();
        }
        List<String> fileRecords = new ServicesFileReader(servicesListPath).readServicesFile();
        Collection<ServiceListRecord> records = serviceListFileRecordsParser.toServiceListRecords(fileRecords);
        return records.stream().map(record ->
            new DhcpServiceCandidate(record.getServiceType(),
                urlProvider.getEndpointUri(record.getUrl()), null)).collect(toSet());
    }
}
