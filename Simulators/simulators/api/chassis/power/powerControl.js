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

class PowerControl {
    constructor(baseUri, id){
        this.baseUri = baseUri;
        this.id = id;
        this.relatedItems = new OdataIdArray();
    }

    Id() {
        return this.id;
    }

    GetBaseUri() {
        return this.baseUri + '#/PowerControl/' + this.Id();
    }

    Json(){
        return {
            '@odata.id': this.GetBaseUri(),
            'MemberId': this.Id(),
            'Name': 'System Power Control',
            "PowerConsumedWatts": 8000,
            "PowerRequestedWatts": 8500,
            "PowerAvailableWatts": 8500,
            "PowerCapacityWatts": 10000,
            "PowerAllocatedWatts": 8500,
            "PowerMetrics": {
                "IntervalInMin": 30,
                "MinConsumedWatts": 7500,
                "MaxConsumedWatts": 8200,
                "AverageConsumedWatts": 8000
            },
            "PowerLimit": {
                "LimitInWatts": 9000,
                "LimitException": "LogEventOnly",
                "CorrectionInMs": 42
            },
            'RelatedItem' : this.relatedItems,
            "Status": {
                "State": "Enabled",
                "Health": "OK",
                "HealthRollup": "OK"
            },
            "Oem": {}
        }
    }

    AddToRelatedItems(element) {
        this.relatedItems.AddWithODataId(element.GetBaseUri());
    }

    RemoveRelatedItem(odataid) {
        this.relatedItems.Remove(odataid);
    }

    UnlinkComputerSystem(odataid) {
        this.RemoveRelatedItem(odataid);
    }

    AddToRedundancySet(element) {
        this.redundancy.AddWithODataId(element.GetBaseUri());
    }

}

exports.PowerControl = PowerControl;
