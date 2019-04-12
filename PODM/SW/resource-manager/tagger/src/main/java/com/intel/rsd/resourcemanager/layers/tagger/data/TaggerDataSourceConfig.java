/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.tagger.data;

import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.boot.autoconfigure.jdbc.DataSourceProperties;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.boot.jdbc.DataSourceBuilder;
import org.springframework.boot.orm.jpa.EntityManagerFactoryBuilder;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Primary;
import org.springframework.context.annotation.PropertySource;
import org.springframework.data.jpa.repository.config.EnableJpaRepositories;
import org.springframework.orm.jpa.JpaTransactionManager;
import org.springframework.orm.jpa.LocalContainerEntityManagerFactoryBean;
import org.springframework.transaction.PlatformTransactionManager;
import org.springframework.transaction.annotation.EnableTransactionManagement;

import javax.persistence.EntityManagerFactory;
import javax.sql.DataSource;

@Configuration
@PropertySource("classpath:tagger.properties")
@EnableTransactionManagement
@EnableJpaRepositories(
    entityManagerFactoryRef = "taggerEntityManagerFactory",
    transactionManagerRef = "taggerTransactionManager",
    basePackageClasses = ResourceTaggedValues.class
)
public class TaggerDataSourceConfig {
    @Bean(name = "taggerDataSourceProperties")
    @ConfigurationProperties("tagger-datasource")
    @Primary
    public DataSourceProperties dataSourceProperties() {
        return new DataSourceProperties();
    }

    @Bean(name = "taggerDataSource")
    public DataSource getDataSource(@Qualifier("taggerDataSourceProperties") DataSourceProperties properties) {
        return DataSourceBuilder.create()
            .url(properties.getUrl())
            .username(properties.getUsername())
            .password(properties.getPassword())
            .build();
    }

    @Bean(name = "taggerEntityManagerFactory")
    public LocalContainerEntityManagerFactoryBean getEntityManagerFactoryBean(EntityManagerFactoryBuilder entityManagerFactoryBuilder,
                                                                              @Qualifier("taggerDataSource") DataSource dataSource) {
        return entityManagerFactoryBuilder.dataSource(dataSource)
            .packages(ResourceTaggedValues.class)
            .persistenceUnit("tagger")
            .build();
    }

    @Bean(name = "taggerTransactionManager")
    public PlatformTransactionManager getTransactionManager(@Qualifier("taggerEntityManagerFactory") EntityManagerFactory entityManagerFactory) {
        return new JpaTransactionManager(entityManagerFactory);
    }
}
