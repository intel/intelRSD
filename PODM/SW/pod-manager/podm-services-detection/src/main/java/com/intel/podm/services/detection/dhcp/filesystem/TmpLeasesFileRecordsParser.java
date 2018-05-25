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

package com.intel.podm.services.detection.dhcp.filesystem;

import javax.enterprise.context.Dependent;
import java.util.Collection;
import java.util.List;

import static com.google.common.net.InetAddresses.isInetAddress;
import static java.util.Objects.nonNull;
import static java.util.stream.Collectors.toList;
import static org.apache.commons.lang3.StringUtils.isNotEmpty;
import static org.apache.commons.lang3.StringUtils.split;

@Dependent
public class TmpLeasesFileRecordsParser {

    private static final int TMP_LEASES_MIN_PARTS_LENGTH = 4;

    private static final int TMP_LEASES_MAC_POSITION = 0;
    private static final int TMP_LEASES_IP_POSITION = 1;
    private static final int TMP_LEASES_TYPE_POSITION = 2;
    private static final int TMP_LEASES_DATE_POSITION = 3;

    public Collection<TmpLeasesRecord> toTmpLeasesRecordCollection(List<String> inputRecords) {
        return inputRecords.stream()
                .map(this::toRecordParts)
                .filter(this::isRecordFormatValid)
                .filter(this::containsValidIpAddress)
                .filter(this::containsValidServiceTypeId)
                .filter(this::containsValidDate)
                .map(this::toTmpLeasesRecord)
                .collect(toList());
    }

    private String[] toRecordParts(String entry) {
        return split(entry);
    }

    private boolean isRecordFormatValid(String[] recordParts) {
        return nonNull(recordParts) && recordParts.length >= TMP_LEASES_MIN_PARTS_LENGTH;
    }

    private boolean containsValidIpAddress(String[] recordParts) {
        return isInetAddress(recordParts[TMP_LEASES_IP_POSITION]);
    }

    private boolean containsValidServiceTypeId(String[] recordParts) {
        return TypeIdServiceTypeDictionary.fromTypeId(recordParts[TMP_LEASES_TYPE_POSITION]).isPresent();
    }

    private boolean containsValidDate(String[] recordParts) {
        String dateString = recordParts[TMP_LEASES_DATE_POSITION];
        return isNotEmpty(dateString);
    }

    private TmpLeasesRecord toTmpLeasesRecord(String[] recordParts) {
        return new TmpLeasesRecord(TypeIdServiceTypeDictionary.fromTypeId(recordParts[TMP_LEASES_TYPE_POSITION]).get(),
                recordParts[TMP_LEASES_IP_POSITION],
                recordParts[TMP_LEASES_DATE_POSITION]
        );
    }
}
