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

package com.intel.rsd.podm.security;

import com.intel.rsd.podm.security.jpa.ManagerAccountEntity;
import lombok.Getter;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.core.userdetails.UserDetails;

import javax.validation.constraints.NotNull;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static java.util.Collections.unmodifiableList;

public class PodmUser implements UserDetails {

    private static final long serialVersionUID = -2441648293039281528L;

    private final List<GrantedAuthority> authorities = new ArrayList<>();

    @Getter
    private final String username;

    @Getter
    private final String password;

    public PodmUser(@NotNull ManagerAccountEntity user) {
        this.username = user.getUsername();
        this.password = user.getPassword();
        authorities.add(retrieveAuthoritiesFrom(user));
    }

    public PodmUser(String userName) {
        this.username = userName;
        this.password = null;
    }

    private GrantedAuthority retrieveAuthoritiesFrom(ManagerAccountEntity user) {
        return new SimpleGrantedAuthority(user.getRole().getRoleName());
    }

    @Override
    public Collection<? extends GrantedAuthority> getAuthorities() {
        return unmodifiableList(authorities);
    }

    @Override
    public boolean isAccountNonExpired() {
        return true;
    }

    @Override
    public boolean isAccountNonLocked() {
        return true;
    }

    @Override
    public boolean isCredentialsNonExpired() {
        return true;
    }

    @Override
    public boolean isEnabled() {
        return true;
    }
}
