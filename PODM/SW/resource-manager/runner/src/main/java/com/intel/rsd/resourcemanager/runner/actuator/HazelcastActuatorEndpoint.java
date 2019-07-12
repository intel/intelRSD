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

package com.intel.rsd.resourcemanager.runner.actuator;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.hazelcast.core.Hazelcast;
import com.hazelcast.core.HazelcastInstance;
import com.hazelcast.core.Member;
import org.springframework.boot.actuate.endpoint.annotation.ReadOperation;
import org.springframework.boot.actuate.endpoint.web.annotation.WebEndpoint;
import org.springframework.stereotype.Component;

import java.util.Iterator;

@WebEndpoint(id = "hazelcast")
@Component
public class HazelcastActuatorEndpoint {

    @ReadOperation
    public Details details() {
        return new Details(isLeader());
    }

    private Boolean isLeader() {
        Iterator<HazelcastInstance> iterator = Hazelcast.getAllHazelcastInstances().iterator();
        if (iterator.hasNext()) {
            HazelcastInstance hazelcastInstance = iterator.next();
            Iterator<Member> memberIterator = hazelcastInstance.getCluster().getMembers().iterator();
            if (memberIterator.hasNext()) {
                return memberIterator.next().localMember();
            }
        }
        return true;
    }

    @JsonInclude
    public class Details {
        private boolean isLeader;

        public Details(Boolean isLeader) {
            this.isLeader = isLeader;
        }

        public Boolean getIsLeader() {
            return isLeader;
        }

        public void setIsLeader(Boolean leader) {
            isLeader = leader;
        }
    }
}
