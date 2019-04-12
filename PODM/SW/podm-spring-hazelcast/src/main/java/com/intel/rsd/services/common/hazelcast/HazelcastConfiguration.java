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

package com.intel.rsd.services.common.hazelcast;

import com.hazelcast.config.Config;
import com.hazelcast.config.JoinConfig;
import com.hazelcast.config.ScheduledExecutorConfig;
import com.hazelcast.spi.discovery.integration.DiscoveryServiceProvider;
import com.hazelcast.spring.context.SpringManagedContext;
import lombok.extern.slf4j.Slf4j;
import org.springframework.boot.autoconfigure.AutoConfigureBefore;
import org.springframework.boot.autoconfigure.hazelcast.HazelcastAutoConfiguration;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import java.io.IOException;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.ServerSocket;
import java.net.SocketException;
import java.util.Collection;
import java.util.Collections;

import static com.hazelcast.spi.properties.GroupProperty.DISCOVERY_SPI_ENABLED;
import static com.hazelcast.spi.properties.GroupProperty.MERGE_FIRST_RUN_DELAY_SECONDS;
import static com.hazelcast.spi.properties.GroupProperty.MERGE_NEXT_RUN_DELAY_SECONDS;
import static com.hazelcast.spi.properties.GroupProperty.PHONE_HOME_ENABLED;
import static com.hazelcast.util.EmptyStatement.ignore;
import static java.lang.Boolean.FALSE;
import static java.lang.Boolean.TRUE;
import static java.util.Collections.emptyList;

@Configuration
@AutoConfigureBefore(HazelcastAutoConfiguration.class)
@Slf4j
public class HazelcastConfiguration {
    private static final int HAZELCAST_PORT = 5701;
    private static final int PORT_RANGE = 100;

    private static int getNextUnusedPort(int start, int range) {
        for (int port = start; port < start + range; port++) {
            try {
                new ServerSocket(port).close();
                return port;
            } catch (IOException portInUse) {
                ignore(portInUse);
            }
        }
        throw new RuntimeException("Could not find next unused port. Started from: " + start + " with range: " + range);
    }

    //FIXME: do this another way...
    // use hostname here?
    private static String findHostRemoteIp() {
        try {
            for (NetworkInterface networkInterface : Collections.list(NetworkInterface.getNetworkInterfaces())) {
                for (InetAddress inetAddress : Collections.list(networkInterface.getInetAddresses())) {
                    if (inetAddress instanceof Inet4Address) {
                        if (!"127.0.0.1".equals(inetAddress.getHostAddress())) {
                            return inetAddress.getHostAddress();
                        }
                    }
                }
            }
        } catch (SocketException e) {
            log.error(e.getMessage(), e);
        }
        return "127.0.0.1";
    }

    @Bean
    public SpringManagedContext managedContext() {
        return new SpringManagedContext();
    }

    @Bean
    Collection<ScheduledExecutorConfig> scheduledExecutorConfigs() {
        return emptyList();
    }

    @Bean
    @SuppressWarnings("checkstyle:MethodLength")
    Config hazelcastConfig(DiscoveryServiceProvider discoveryServiceProvider, SpringManagedContext context,
                           Collection<ScheduledExecutorConfig> scheduledExecutorConfigs) {
        Config config = new Config();
        config.setManagedContext(context);
        config.getNetworkConfig()
            .setPort(getNextUnusedPort(HAZELCAST_PORT, PORT_RANGE));

        config.getNetworkConfig().getInterfaces().addInterface(findHostRemoteIp());
        scheduledExecutorConfigs.forEach(config::addScheduledExecutorConfig);

        JoinConfig joinConfig = config
            .getNetworkConfig()
            .getJoin();

        config.setProperty(DISCOVERY_SPI_ENABLED.getName(), TRUE.toString());

        joinConfig
            .getMulticastConfig()
            .setEnabled(false);

        joinConfig
            .getDiscoveryConfig()
            .setDiscoveryServiceProvider(discoveryServiceProvider);

        //based on https://docs.hazelcast.org/docs/3.9/manual/html-single/index.html#system-properties
        config.setProperty(MERGE_FIRST_RUN_DELAY_SECONDS.getName(), "1"); //TODO: make configurable
        config.setProperty(MERGE_NEXT_RUN_DELAY_SECONDS.getName(), "2"); //TODO: make configurable
        config.setProperty(PHONE_HOME_ENABLED.getName(), FALSE.toString());

        return config;
    }
}
