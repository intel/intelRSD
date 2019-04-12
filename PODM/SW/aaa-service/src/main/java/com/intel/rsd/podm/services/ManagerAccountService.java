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

package com.intel.rsd.podm.services;

import com.intel.rsd.podm.security.jpa.AccountDetailsRepository;
import com.intel.rsd.podm.security.jpa.ManagerAccountEntity;
import com.intel.rsd.podm.security.jpa.RoleEntity;
import com.intel.rsd.podm.services.dto.ManagerAccount;
import com.intel.rsd.podm.services.dto.ManagerAccountUpdateRequest;
import com.intel.rsd.podm.services.password.CredentialsValidator;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import javax.validation.Valid;
import javax.validation.ValidationException;
import javax.validation.constraints.NotNull;
import java.util.Collection;
import java.util.Optional;

@Slf4j
@Component
public class ManagerAccountService {

    private final AccountDetailsRepository accountDetailsRepository;

    private final PasswordEncoder passwordEncoder;

    private final CredentialsValidator credentialsValidator;

    @Autowired
    public ManagerAccountService(AccountDetailsRepository accountDetailsRepository,
                                 PasswordEncoder passwordEncoder, CredentialsValidator credentialsValidator) {
        this.accountDetailsRepository = accountDetailsRepository;
        this.passwordEncoder = passwordEncoder;
        this.credentialsValidator = credentialsValidator;
    }

    @Transactional
    public ManagerAccountEntity createManagerAccount(@NotNull ManagerAccount managerAccount, @NotNull RoleEntity roleEntity) {
        validateManager(roleEntity, managerAccount);

        ManagerAccountEntity accountDatabaseObject = new ManagerAccountEntity();
        accountDatabaseObject.setRole(roleEntity);
        accountDatabaseObject.setUsername(managerAccount.getUsername());
        accountDatabaseObject.setPassword(passwordEncoder.encode(managerAccount.getPassword()));
        accountDetailsRepository.save(accountDatabaseObject);
        return accountDatabaseObject;
    }

    private void validateManager(RoleEntity requestedRole, @NotNull ManagerAccount managerAccount) {
        ManagerAccountEntity existingUser = accountDetailsRepository.findByUsername(managerAccount.getUsername());

        if (requestedRole == null) {
            log.warn("Unable to create/retrieve required role for manager account.");
            throw new ValidationException("Unable to create/retrieve required role for manager account.");
        }
        if (existingUser != null) {
            log.warn("Unable to create user because username is already taken.");
            throw new ValidationException("Unable to create user because username is already taken.");
        }

        credentialsValidator.validate(managerAccount.getPassword(), managerAccount.getUsername());
    }

    Collection<ManagerAccountEntity> getManagerAccounts() {
        return accountDetailsRepository.findAll();
    }

    @Transactional
    public ManagerAccountEntity getManagerAccount(String username) {
        return accountDetailsRepository.findByUsername(username);
    }

    @Transactional
    public void deleteManagerAccount(String username) {
        ManagerAccountEntity user = accountDetailsRepository.findByUsername(username);
        if (user == null) {
            throw new ResourceNotFoundException("Unable to find manager account for given username.");
        }
        if (accountDetailsRepository.findAll().size() > 1) {
            accountDetailsRepository.deleteById(user.getId());
        } else {
            log.error("Unable to delete last user. At least one remaining user required after removal operation.");
            throw new DataIntegrityException("Unable to delete last user. At least one remaining user required after removal operation.");
        }
    }

    @Transactional
    public void updateManagerAccount(Long id, @Valid ManagerAccountUpdateRequest changes) {
        Optional<ManagerAccountEntity> accountToUpdate = accountDetailsRepository.findById(id);
        if (accountToUpdate.isPresent()) {
            ManagerAccountEntity managerAccountEntity = accountToUpdate.get();
            credentialsValidator.validate(changes.getPassword(), managerAccountEntity.getUsername());
            String password = passwordEncoder.encode(changes.getPassword());
            managerAccountEntity.setPassword(password);
        } else {
            throw new ResourceNotFoundException("Unable to find account to update");
        }
    }
}
