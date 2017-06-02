/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.EventOriginProvider;
import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.Chap;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embedded;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = RemoteTargetIscsiAddress.GET_REMOTE_TARGET_ISCSI_ADDRESSES_BY_TARGET_IQN,
        query = "SELECT iscsiAddress FROM RemoteTargetIscsiAddress iscsiAddress WHERE iscsiAddress.targetIqn = :targetIqn")
})
@Table(name = "remote_target_iscsi_address")
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class RemoteTargetIscsiAddress extends Entity {
    public static final String GET_REMOTE_TARGET_ISCSI_ADDRESSES_BY_TARGET_IQN = "GET_REMOTE_TARGET_ISCSI_ADDRESSES_BY_TARGET_IQN";

    @Column(name = "target_iqn")
    private String targetIqn;

    @Column(name = "target_portal_ip")
    private String targetPortalIp;

    @Column(name = "target_portal_port")
    private Integer targetPortalPort;

    @ElementCollection
    @CollectionTable(name = "remote_target_iscsi_address_target_lun", joinColumns = @JoinColumn(name = "remote_target_iscsi_address_id"))
    @Column(name = "target_lun")
    @OrderColumn(name = "target_lun_order")
    private List<Integer> targetLuns = new ArrayList<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "remote_target_id")
    private RemoteTarget remoteTarget;

    @Embedded
    private Chap chap;

    public String getTargetIqn() {
        return targetIqn;
    }

    public void setTargetIqn(String targetIqn) {
        this.targetIqn = targetIqn;
    }

    public String getTargetPortalIp() {
        return targetPortalIp;
    }

    public void setTargetPortalIp(String targetPortalIp) {
        this.targetPortalIp = targetPortalIp;
    }

    public Integer getTargetPortalPort() {
        return targetPortalPort;
    }

    public void setTargetPortalPort(Integer targetPortalPort) {
        this.targetPortalPort = targetPortalPort;
    }

    public List<Integer> getTargetLuns() {
        return targetLuns;
    }

    public void addTargetLun(Integer targetLun) {
        targetLuns.add(targetLun);
    }

    @EventOriginProvider
    public RemoteTarget getRemoteTarget() {
        return remoteTarget;
    }

    public void setRemoteTarget(RemoteTarget remoteTarget) {
        if (!Objects.equals(this.remoteTarget, remoteTarget)) {
            unlinkRemoteTarget(this.remoteTarget);
            this.remoteTarget = remoteTarget;
            if (remoteTarget != null && !remoteTarget.getRemoteTargetIscsiAddresses().contains(this)) {
                remoteTarget.addRemoteTargetIscsiAddress(this);
            }
        }
    }

    public void unlinkRemoteTarget(RemoteTarget remoteTarget) {
        if (Objects.equals(this.remoteTarget, remoteTarget)) {
            this.remoteTarget = null;
            if (remoteTarget != null) {
                remoteTarget.unlinkRemoteTargetIscsiAddress(this);
            }
        }
    }

    public Chap getChap() {
        return chap;
    }

    public void setChap(Chap chap) {
        this.chap = chap;
    }

    @Override
    public void preRemove() {
        unlinkRemoteTarget(remoteTarget);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, remoteTarget);
    }
}
