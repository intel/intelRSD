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

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.podm.security.jpa.ManagerAccountEntity;
import com.intel.rsd.podm.security.jpa.PrivilegeEntity;
import com.intel.rsd.podm.security.jpa.RoleEntity;
import com.intel.rsd.podm.services.dto.ManagerAccount;
import com.intel.rsd.podm.services.dto.ManagerAccountUpdateRequest;
import com.intel.rsd.podm.services.dto.Role;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PatchMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.validation.Valid;
import java.io.IOException;
import java.net.URI;
import java.util.Collection;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static com.intel.rsd.redfish.RedfishUtils.toODataIdArrayNode;
import static java.util.stream.Collectors.toList;
import static org.springframework.http.ResponseEntity.created;
import static org.springframework.http.ResponseEntity.noContent;
import static org.springframework.http.ResponseEntity.ok;

@RestController
@RequestMapping("/redfish/v1")
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class AccountService {

    private RoleService roleService;

    private ManagerAccountService managerAccountService;

    private ObjectNode accountServiceResource;
    private ObjectNode accountsCollectionResource;
    private ObjectNode rolesCollectionResource;
    private ObjectNode managerAccountResource;

    @Autowired
    public AccountService(RoleService roleService, ManagerAccountService managerAccountService) throws IOException {
        this.roleService = roleService;
        this.managerAccountService = managerAccountService;
        accountServiceResource = readJsonFromFile(this, "AccountServiceTemplate.json");
        accountsCollectionResource = readJsonFromFile(this, "AccountsCollectionTemplate.json");
        rolesCollectionResource = readJsonFromFile(this, "RolesCollectionTemplate.json");
        managerAccountResource = readJsonFromFile(this, "ManagerAccountTemplate.json");
    }

    @GetMapping("AccountService")
    public ObjectNode getAccountService() {
        return accountServiceResource;
    }

    @GetMapping("AccountService/Accounts")
    public JsonNode getAccounts() {
        Collection<ManagerAccountEntity> managerAccounts = managerAccountService.getManagerAccounts();
        val ids = managerAccounts.stream().map(ManagerAccountEntity::getUsername)
            .map(integer -> "/redfish/v1/AccountService/Accounts/" + integer).collect(toList());
        val idsColl = toODataIdArrayNode(ids);
        return accountsCollectionResource.deepCopy()
            .put("Members@odata.count", idsColl.size())
            .set("Members", idsColl);
    }

    @GetMapping("AccountService/Accounts/{username}")
    public ResponseEntity<JsonNode> getSpecificAccount(@PathVariable String username) {
        ManagerAccountEntity managerAccount = managerAccountService.getManagerAccount(username);
        if (managerAccount == null) {
            throw new ResourceNotFoundException("Unable to find account.");
        }
        return ok(createManagerAccountJson(managerAccount));
    }

    @PostMapping("AccountService/Accounts")
    public ResponseEntity createAccount(@RequestBody @Valid ManagerAccount account) {
        ManagerAccountEntity managerAccountEntity = managerAccountService.createManagerAccount(account, roleService.getRoleByName(account.getRoleId()));
        URI locationUri = URI.create("/redfish/v1/AccountService/Accounts/" + managerAccountEntity.getUsername());
        return created(locationUri).body(createManagerAccountJson(managerAccountEntity));
    }

    @PatchMapping("AccountService/Accounts/{username}")
    public ResponseEntity updateAccount(@PathVariable("username") String userName, @RequestBody @Valid ManagerAccountUpdateRequest changes) {
        ManagerAccountEntity managerAccountEntity = managerAccountService.getManagerAccount(userName);
        managerAccountService.updateManagerAccount(managerAccountEntity.getId(), changes);
        return getSpecificAccount(userName);
    }

    @DeleteMapping("AccountService/Accounts/{username}")
    public ResponseEntity deleteAccount(@PathVariable("username") String username) {
        managerAccountService.deleteManagerAccount(username);
        return noContent().build();
    }

    @GetMapping("AccountService/Roles")
    public JsonNode getRoles() {
        Collection<RoleEntity> allRoles = roleService.getAllRoles();
        val ids = allRoles.stream().map(RoleEntity::getRoleName)
            .map(roleName -> "/redfish/v1/AccountService/Roles/" + roleName)
            .collect(toList());
        val idsColl = toODataIdArrayNode(ids);
        return rolesCollectionResource.deepCopy()
            .put("Members@odata.count", idsColl.size())
            .set("Members", idsColl);
    }

    @GetMapping("AccountService/Roles/{roleName}")
    public ResponseEntity getRole(@PathVariable("roleName") String roleName) {
        RoleEntity roleEntity = roleService.getRoleByName(roleName);
        if (roleEntity == null) {
            throw new ResourceNotFoundException("Unable to find role.");
        }
        Role role = new Role(roleEntity.getRoleName());
        role.getAssignedPrivileges().addAll(roleEntity.getPrivileges().stream().map(PrivilegeEntity::getName).collect(toList()));
        return ok(role);
    }

    private JsonNode createManagerAccountJson(ManagerAccountEntity managerAccount) {
        String username = managerAccount.getUsername();
        String roleName = managerAccount.getRole().getRoleName();
        val links = JsonNodeFactory.instance.arrayNode();
        links.add(JsonNodeFactory.instance.objectNode().put("@odata.id", "/redfish/v1/AccountService/Roles/" + roleName));

        return managerAccountResource.deepCopy()
            .put("UserName", managerAccount.getUsername())
            .put("Id", managerAccount.getUsername())
            .put("RoleId", roleName)
            .put("@odata.id", "/redfish/v1/AccountService/Accounts/" + username)
            .set("Links", links);
    }
}
