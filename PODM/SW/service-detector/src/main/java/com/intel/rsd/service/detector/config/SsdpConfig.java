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

package com.intel.rsd.service.detector.config;

import lombok.Getter;
import lombok.Setter;
import org.apache.commons.lang3.builder.ToStringBuilder;
import org.apache.commons.net.util.SubnetUtils;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.cloud.context.config.annotation.RefreshScope;
import org.springframework.stereotype.Component;

import java.util.Collection;

import static java.lang.String.format;
import static java.lang.System.setProperty;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;
import static org.apache.commons.lang3.builder.ToStringStyle.SHORT_PREFIX_STYLE;

@Getter
@Setter
@ConfigurationProperties(prefix = "protocols.ssdp")
@Component
@RefreshScope
public class SsdpConfig {
    private int announcementFrequencyInSeconds = 10;
    private int mx = 5;
    private int mSearchBulkRepeat = 3;
    private int mSearchBulkIntervalMilliseconds = 100;
    private Collection<SubnetUtils> subnets = emptyList();
    private Collection<SubnetUtils> exclusions = emptyList();
    private Collection<String> networkInterfacesToBind;
    private int threadPoolSize = 20;
    private int ssdpTaskQueueCapacity = 500;

    @Override
    public String toString() {
        return new ToStringBuilder(this, SHORT_PREFIX_STYLE)
            .append("announcementFrequencyInSeconds", announcementFrequencyInSeconds)
            .append("mx", mx)
            .append("mSearchBulkRepeat", mSearchBulkRepeat)
            .append("mSearchBulkIntervalMilliseconds", mSearchBulkIntervalMilliseconds)
            .append("threadPoolSize", threadPoolSize)
            .append("networkInterfacesToBind", networkInterfacesToBind)
            .append("subnets", asList(subnets))
            .append("exclusions", asList(exclusions))
            .append("ssdpTaskQueueCapacity", ssdpTaskQueueCapacity)
            .build();
    }

    public void setNetworkInterfacesToBind(Collection<String> networkInterfacesToBind) {
        this.networkInterfacesToBind = networkInterfacesToBind;
        setProperty("org.fourthline.cling.network.useInterfaces", networkInterfacesToBind.stream().reduce((s1, s2) -> s1 + "," + s2).orElse(""));
    }

    private Collection<String> asList(Collection<SubnetUtils> subnetUtilsCollection) {
        return subnetUtilsCollection.stream().map(SubnetUtils::getInfo).map(si -> format("%s/%s", si.getAddress(), si.getNetmask())).collect(toList());
    }
}
