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

package com.intel.podm.services.detection;

import com.intel.podm.services.detection.filesystem.ServiceListFileRecordsParser;
import com.intel.podm.services.detection.filesystem.ServiceListRecord;
import com.intel.podm.services.detection.filesystem.ServicesFileReader;
import com.intel.podm.services.detection.filesystem.TmpLeasesFileRecordsParser;
import com.intel.podm.services.detection.filesystem.TmpLeasesRecord;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.nio.file.Path;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static java.nio.file.Files.exists;
import static java.nio.file.Paths.get;

@Dependent
public class ServiceEndpointCandidatesProvider {

    private static final String TMP_LEASES_LOCATION = "/tmp/leases";
    private static final String SERVICES_LIST_LOCATION = "/tmp/services.list";

    @Inject
    UrlProvider urlProvider;

    @Inject
    TmpLeasesFileRecordsParser tmpLeasesFileRecordsParser;

    @Inject
    ServiceListFileRecordsParser serviceListFileRecordsParser;


    public Set<ServiceEndpointCandidate> getEndpointCandidates() {
        Set<ServiceEndpointCandidate> candidates = new HashSet<>();

        Path tmpLeasesPath = get(TMP_LEASES_LOCATION);
        if (exists(tmpLeasesPath)) {
            List<String> fileRecords = new ServicesFileReader(tmpLeasesPath).readServicesFile();
            Collection<TmpLeasesRecord> records = tmpLeasesFileRecordsParser.toTmpLeasesRecordCollection(fileRecords);
            records.stream().forEach(record ->
                    candidates.add(new ServiceEndpointCandidate(record.getServiceType(),
                            urlProvider.getEndpointUri(record), record.getMacAddress(), record.getDate())));
        }

        Path servicesListPath = get(SERVICES_LIST_LOCATION);
        if (exists(servicesListPath)) {
            List<String> fileRecords = new ServicesFileReader(servicesListPath).readServicesFile();
            Collection<ServiceListRecord> records = serviceListFileRecordsParser.toServiceListRecords(fileRecords);
            records.stream().forEach(record ->
                    candidates.add(new ServiceEndpointCandidate(record.getServiceType(),
                            urlProvider.getEndpointUri(record.getUrl()), null, null)));
        }

        return candidates;
    }

}
