/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.podm.security.jpa;

import com.intel.rsd.podm.services.RoleRepository;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.core.io.ClassPathResource;
import org.springframework.jdbc.datasource.init.DataSourceInitializer;
import org.springframework.jdbc.datasource.init.ResourceDatabasePopulator;
import org.springframework.security.core.userdetails.UserDetailsService;

import javax.sql.DataSource;

import static java.util.Optional.ofNullable;

@Configuration
public class UserDetailsServiceJpaConfiguration {

    @Value("${aaa-config.authority-data.data}")
    private String initialData;

    @Bean
    public UserDetailsService userDetailsService(AccountDetailsRepository accountDetailsRepository) {
        return new PodmJpaUserDetailsService(accountDetailsRepository);
    }

    @Bean
    public DataSourceInitializer dataSourceInitializer(DataSource dataSource,
                                                       AccountDetailsRepository accountDetailsRepository, RoleRepository roleRepository) {
        final ResourceDatabasePopulator populator = new ResourceDatabasePopulator();
        ofNullable(initialData).ifPresent(s -> populator.addScript(new ClassPathResource(s)));

        boolean noUsersExist = accountDetailsRepository.findAll().isEmpty();
        boolean noRolesExist = roleRepository.findAll().isEmpty();
        final DataSourceInitializer initializer = new DataSourceInitializer();
        initializer.setDataSource(dataSource);
        initializer.setDatabasePopulator(populator);
        initializer.setEnabled(noUsersExist && noRolesExist);
        return initializer;
    }
}
