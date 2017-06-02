/*
 * Copyright (c) 2017 Intel Corporation
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

import liquibase.change.custom.CustomTaskChange;
import liquibase.database.Database;
import liquibase.exception.CustomChangeException;
import liquibase.exception.SetupException;
import liquibase.exception.ValidationErrors;
import liquibase.resource.ResourceAccessor;

import javax.enterprise.context.spi.CreationalContext;
import javax.enterprise.inject.spi.Bean;
import javax.enterprise.inject.spi.BeanManager;
import javax.enterprise.inject.spi.CDI;
import java.util.Set;

import static com.intel.podm.common.utils.IterableHelper.single;

public abstract class JpaBasedDataMigrationTask implements CustomTaskChange {

    private static final ValidationErrors NO_ERRORS = new ValidationErrors();

    private MigrationTaskRegistry migrationTaskRegistry;

    @Override
    public final void execute(Database database) throws CustomChangeException {
        migrationTaskRegistry.register(this.getClass());
    }

    @Override
    public final String getConfirmationMessage() {
        return getClass().getSimpleName();
    }

    @Override
    public final void setUp() throws SetupException {
        BeanManager beanManager = CDI.current().getBeanManager();
        Set<Bean<?>> beans = beanManager.getBeans(MigrationTaskRegistry.class);
        Bean<MigrationTaskRegistry> bean = (Bean<MigrationTaskRegistry>) single(beans);
        CreationalContext<MigrationTaskRegistry> creationalContext = beanManager.createCreationalContext(bean);
        this.migrationTaskRegistry = (MigrationTaskRegistry) beanManager.getReference(bean, MigrationTaskRegistry.class, creationalContext);
    }

    @Override
    public final void setFileOpener(ResourceAccessor resourceAccessor) { }

    @Override
    public final ValidationErrors validate(Database database) {
        return NO_ERRORS;
    }

    public abstract void run();
}
