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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "metric_item", indexes = @Index(name = "idx_metric_item_entity_id", columnList = "entity_id", unique = true))
public class MetricItem extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "metric_value")
    private String metricValue;

    @OneToOne(mappedBy = "metricItem", fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "metric_definition_id")
    private MetricDefinition metricDefinition;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "metric_report_definition_id")
    private MetricReportDefinition metricReportDefinition;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public String getMetricValue() {
        return metricValue;
    }

    public void setMetricValue(String metricValue) {
        this.metricValue = metricValue;
    }

    public MetricReportDefinition getMetricReportDefinition() {
        return metricReportDefinition;
    }

    public void setMetricReportDefinition(MetricReportDefinition metricReportDefinition) {
        if (metricReportDefinition == null) {
            unlinkMetricReportDefinition(this.metricReportDefinition);
        } else {
            this.metricReportDefinition = metricReportDefinition;
            if (!metricReportDefinition.getMetricItems().contains(this)) {
                metricReportDefinition.addMetricItem(this);
            }
        }
    }

    public void unlinkMetricReportDefinition(MetricReportDefinition metricReportDefinition) {
        if (Objects.equals(this.metricReportDefinition, metricReportDefinition)) {
            this.metricReportDefinition = null;
            if (metricReportDefinition != null) {
                metricReportDefinition.unlinkMetricItem(this);
            }
        }
    }

    public MetricDefinition getMetricDefinition() {
        return metricDefinition;
    }

    public void setMetricDefinition(MetricDefinition entity) {
        if (!Objects.equals(this.metricDefinition, entity)) {
            unlinkMetricDefinition(this.metricDefinition);
            this.metricDefinition = entity;
            if (entity != null) {
                entity.setMetricItem(this);
            }
        }
    }

    public void unlinkMetricDefinition(MetricDefinition metricDefinition) {
        if (Objects.equals(this.metricDefinition, metricDefinition)) {
            this.metricDefinition = null;
            if (metricDefinition != null) {
                metricDefinition.unlinkMetricItem(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkMetricReportDefinition(metricReportDefinition);
        unlinkMetricDefinition(metricDefinition);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, metricReportDefinition);
    }
}
