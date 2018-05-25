/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.entities.migration;

import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.logger.Logger;

import javax.annotation.PostConstruct;
import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.util.ArrayList;
import java.util.List;

@ApplicationScoped
public class MigrationTaskRegistry {

    @Inject
    private BeanFactory beanFactory;

    @Inject
    private Logger logger;

    private List<Class<? extends JpaBasedDataMigrationTask>> upgradeTasks = new ArrayList<>();

    @PostConstruct
    private void init() {
        logger.i("Initializing {}", getClass().getSimpleName());
    }

    public void register(Class<? extends JpaBasedDataMigrationTask> upgradeTaskClass) {
        logger.i("Registering upgrade task: {} ", upgradeTaskClass);
        upgradeTasks.add(upgradeTaskClass);
    }

    public void runMigrationTasks() {
        upgradeTasks.stream()
            .map(upgradeTaskClass -> beanFactory.create(upgradeTaskClass))
            .peek(upgradeTask -> logger.i("Running upgrade task: {}", upgradeTask))
            .forEach(JpaBasedDataMigrationTask::run);
    }
}
