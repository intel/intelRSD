/*
 * Copyright (c) 2016-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const OdataIdArray = require('../../common/odataIdArray').OdataIdArray;

class PowerSupply {
    constructor(baseUri, id){
        this.baseUri = baseUri;
        this.id = id;
        this.relatedItems = new OdataIdArray();
        this.redundancy = new OdataIdArray();
    }

    Id() {
        return this.id;
    }

    GetBaseUri() {
        return this.baseUri + '#/PowerSupplies/' + this.Id();
    }

    Json(){
        return {
            '@odata.id': this.GetBaseUri(),
            'MemberId': this.Id(),
            "Name": "Power Supply Bay 1",
            "Status": {
                "State": "Enabled",
                "Health": "Warning"
            },
            "Oem": {},
            "PowerSupplyType": "DC",
            "LineInputVoltageType": "DCNeg48V",
            "LineInputVoltage": -48,
            "PowerCapacityWatts": 400,
            "LastPowerOutputWatts": 192,
            "Model": "499253-B21",
            "Manufacturer": "ManufacturerName",
            "FirmwareVersion": "1.00",
            "SerialNumber": "1z0000001",
            "PartNumber": "1z0000001A3a",
            "SparePartNumber": "0000001A3a",
            "InputRanges": [
                {
                    "InputType": "DC",
                    "MinimumVoltage": -47,
                    "MaximumVoltage": -49,
                    "OutputWattage": 400,
                    "MinimumFrequencyHz": 50,
                    "MaximumFrequencyHz": 60,
                    "Oem": {}
                }
            ],
            "IndicatorLED": "Off",
            'Redundancy' : this.redundancy,
            'RelatedItem' : this.relatedItems
        }
    }

    AddToRelatedItems(element) {
        this.relatedItems.AddWithODataId(element.GetBaseUri());
    }

    AddToRedundancySet(element) {
        this.redundancy.AddWithODataId(element.GetBaseUri());
    }

}

exports.PowerSupply = PowerSupply;
