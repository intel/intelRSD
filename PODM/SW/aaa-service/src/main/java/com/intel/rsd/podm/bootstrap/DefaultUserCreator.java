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

package com.intel.rsd.podm.bootstrap;

import com.intel.rsd.podm.security.jpa.ManagerAccountEntity;
import com.intel.rsd.podm.services.ManagerAccountService;
import com.intel.rsd.podm.services.RoleService;
import com.intel.rsd.podm.services.dto.ManagerAccount;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.ApplicationListener;
import org.springframework.context.event.ContextRefreshedEvent;
import org.springframework.dao.DataAccessException;
import org.springframework.dao.DataIntegrityViolationException;
import org.springframework.stereotype.Component;

import static com.intel.rsd.Contracts.requiresNonNull;

@Slf4j
@Component
public class DefaultUserCreator implements ApplicationListener<ContextRefreshedEvent> {

    @Value("${aaa-config.default-user.name:admin}")
    private String defaultUserName;

    @Value("${aaa-config.default-user.password:admin}")
    private String defaultPassword;

    @Value("${aaa-config.default-user.role:Administrator}")
    private String defaultRole;

    private ManagerAccountService managerAccountService;

    private RoleService roleService;

    public DefaultUserCreator(ManagerAccountService managerAccountService, RoleService roleService) {
        this.managerAccountService = managerAccountService;
        this.roleService = roleService;
    }

    @Override
    public void onApplicationEvent(ContextRefreshedEvent event) {
        requiresNonNull(defaultUserName, "Default admin username");
        requiresNonNull(defaultPassword, "Default admin password");

        ManagerAccountEntity managerAccountEntity = managerAccountService.getManagerAccount(defaultUserName);
        if (managerAccountEntity == null) {
            ManagerAccount managerAccount = new ManagerAccount(defaultUserName, defaultPassword, defaultRole);
            try {
                managerAccountService.createManagerAccount(managerAccount, roleService.getRoleByName(managerAccount.getRoleId()));
            } catch (DataIntegrityViolationException ex) {
                log.debug("Predefined user account may already exist.", ex);
            } catch (DataAccessException ex) {
                log.warn("Unable to create default user.", ex);
            }
        } else {
            log.debug("Predefined user account already exists.");
        }
    }
}
