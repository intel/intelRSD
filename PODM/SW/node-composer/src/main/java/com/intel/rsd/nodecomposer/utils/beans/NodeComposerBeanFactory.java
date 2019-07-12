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

package com.intel.rsd.nodecomposer.utils.beans;

import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.BeansException;
import org.springframework.beans.factory.BeanFactory;
import org.springframework.beans.factory.BeanFactoryAware;
import org.springframework.beans.factory.NoSuchBeanDefinitionException;
import org.springframework.beans.factory.NoUniqueBeanDefinitionException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.time.Instant;
import java.util.HashSet;
import java.util.Set;

import static java.time.Duration.between;
import static java.time.Instant.now;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class NodeComposerBeanFactory implements BeanFactoryAware {
    private BeanFactory beanFactory;

    @Autowired
    private ApplicationContext applicationContext;

    @Override
    public void setBeanFactory(BeanFactory beanFactory) throws BeansException {
        this.beanFactory = beanFactory;
    }

    /**
     * Creates new instance of specified CDI bean class.
     */
    public <T> T create(Class<T> beanClass) {
        Instant start = now();
        try {
            return beanFactory.getBean(beanClass);
        } catch (NoUniqueBeanDefinitionException e) {
            String msg = beanClass + " is satisfied by more than one bean";
            log.error(msg, e);
            throw new IllegalArgumentException(msg, e);
        } catch (NoSuchBeanDefinitionException e) {
            String msg = beanClass + " is not a managed bean";
            log.error(msg, e);
            throw new IllegalArgumentException(msg, e);
        } catch (BeansException e) {
            String msg = "Couldn't create bean: " + beanClass;
            log.error(msg, e);
            throw new IllegalArgumentException(msg, e);
        } finally {
            log.info("Creating bean: [{}] took: {}", beanClass, between(start, now()));
        }
    }

    public <T> Set<T> getBeansOfType(Class<T> beanClass) {
        Instant start = now();
        try {
            return new HashSet<>(applicationContext.getBeansOfType(beanClass).values());
        } finally {
            log.warn("Getting beans of type: [" + beanClass + "] took: " + between(start, now()));
        }
    }
}
