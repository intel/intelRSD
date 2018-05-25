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

import com.intel.podm.common.types.ServiceType;
import org.apache.commons.validator.routines.UrlValidator;

import javax.enterprise.context.Dependent;
import java.util.Collection;
import java.util.List;

import static java.util.Objects.nonNull;
import static java.util.stream.Collectors.toList;
import static org.apache.commons.lang3.StringUtils.split;
import static org.apache.commons.validator.routines.UrlValidator.ALLOW_LOCAL_URLS;

@Dependent
public class ServiceListFileRecordsParser {

    private static final int SERVICE_LIST_PARTS_LENGTH = 2;
    private static final int SERVICE_LIST_URL_POSITION = 0;
    private static final int SERVICE_LIST_TYPE_POSITION = 1;

    public Collection<ServiceListRecord> toServiceListRecords(List<String> inputRecords) {
        return inputRecords.stream()
                .map(this::toRecordParts)
                .filter(this::isRecordFormatValid)
                .filter(this::containsValidUrl)
                .filter(this::containsValidServiceType)
                .map(this::toServiceListRecord)
                .collect(toList());
    }

    private String[] toRecordParts(String entry) {
        return split(entry);
    }

    private boolean isRecordFormatValid(String[] recordParts) {
        return nonNull(recordParts) && recordParts.length == SERVICE_LIST_PARTS_LENGTH;
    }

    private boolean containsValidUrl(String[] recordParts) {
        UrlValidator urlValidator = new UrlValidator(ALLOW_LOCAL_URLS);
        return urlValidator.isValid(recordParts[SERVICE_LIST_URL_POSITION]);
    }

    private boolean containsValidServiceType(String[] recordParts) {
        for (ServiceType type : ServiceType.values()) {
            if (type.name().equalsIgnoreCase(recordParts[SERVICE_LIST_TYPE_POSITION])) {
                return true;
            }
        }
        return false;
    }

    private ServiceListRecord toServiceListRecord(String[] recordParts) {
        return new ServiceListRecord(
            ServiceType.valueOf(recordParts[SERVICE_LIST_TYPE_POSITION].toUpperCase()),
            recordParts[SERVICE_LIST_URL_POSITION]
        );
    }
}
