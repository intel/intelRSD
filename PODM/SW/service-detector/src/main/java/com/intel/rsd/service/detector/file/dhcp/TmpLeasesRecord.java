/*
 * Copyright (c) 2015-2019 Intel Corporation
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

import com.intel.rsd.Contracts;

import java.time.LocalDateTime;
import java.util.Objects;

import static java.lang.String.format;
import static java.time.LocalDateTime.parse;
import static java.util.Objects.hash;

public class TmpLeasesRecord {

    private String ipAddress;
    private LocalDateTime date;

    public TmpLeasesRecord(String ip, String date) {
        Contracts.requiresNonNull(ip, "ip");
        Contracts.requiresNonNull(date, "date");
        this.ipAddress = ip;
        this.date = parse(date);
    }

    public String getIpAddress() {
        return ipAddress;
    }

    public LocalDateTime getDate() {
        return date;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof TmpLeasesRecord)) {
            return false;
        }

        TmpLeasesRecord that = (TmpLeasesRecord) o;
        return Objects.equals(ipAddress, that.ipAddress)
            && Objects.equals(date, that.date);
    }

    @Override
    public int hashCode() {
        return hash(ipAddress, date);
    }

    @Override
    public String toString() {
        return format("{IP=%s, date=%s}", ipAddress, date);
    }
}
