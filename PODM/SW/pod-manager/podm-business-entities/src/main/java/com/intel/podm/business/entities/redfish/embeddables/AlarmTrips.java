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

package com.intel.podm.business.entities.redfish.embeddables;

import org.apache.commons.lang3.builder.EqualsBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import java.util.Objects;

@Embeddable
public class AlarmTrips {
    @Column(name = "temperature")
    private Boolean temperature;

    @Column(name = "spare_block")
    private Boolean spareBlock;

    @Column(name = "uncorrectable_ecc_error")
    private Boolean uncorrectableEccError;

    @Column(name = "correctable_ecc_error")
    private Boolean correctableEccError;

    @Column(name = "address_parity_error")
    private Boolean addressParityError;

    public Boolean getTemperature() {
        return temperature;
    }

    public void setTemperature(Boolean temperature) {
        this.temperature = temperature;
    }

    public Boolean getSpareBlock() {
        return spareBlock;
    }

    public void setSpareBlock(Boolean spareBlock) {
        this.spareBlock = spareBlock;
    }

    public Boolean getUncorrectableEccError() {
        return uncorrectableEccError;
    }

    public void setUncorrectableEccError(Boolean uncorrectableEccError) {
        this.uncorrectableEccError = uncorrectableEccError;
    }

    public Boolean getCorrectableEccError() {
        return correctableEccError;
    }

    public void setCorrectableEccError(Boolean correctableEccError) {
        this.correctableEccError = correctableEccError;
    }

    public Boolean getAddressParityError() {
        return addressParityError;
    }

    public void setAddressParityError(Boolean addressParityError) {
        this.addressParityError = addressParityError;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        AlarmTrips that = (AlarmTrips) o;
        return new EqualsBuilder()
            .append(temperature, that.temperature)
            .append(spareBlock, that.spareBlock)
            .append(uncorrectableEccError, that.uncorrectableEccError)
            .append(correctableEccError, that.correctableEccError)
            .append(addressParityError, that.addressParityError)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return Objects.hash(temperature, spareBlock, uncorrectableEccError, correctableEccError, addressParityError);
    }
}
