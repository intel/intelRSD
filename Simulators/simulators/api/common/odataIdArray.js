/*
 * Copyright (c) 2016-2017 Intel Corporation
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

class OdataIdArray extends Array {
    Add(elements, elementProcessor) {
        if (!(elements instanceof Array)) {
            elements = [elements];
        }
        elements.forEach((element) => {
            this.push(OdataIdArray.OdataIdForAsset(element));
            if (elementProcessor !== undefined) {
                elementProcessor(element);
            }
        });

        return this;
    }

    AddWithODataId(oDataId) {
        this.push({'@odata.id': oDataId});
    }

    Clear() {
        this.length = 0;
    }

    static OdataIdForAsset(element) {
        return {'@odata.id': element.GetBaseUri()};
    }

    GetPositionOfElement(odataId) {
        for (var i = 0; i < this.length; i++) {
            if (this[i]['@odata.id'] == odataId) {
                return i;
            }
        }
        return -1;
    };

    Remove(odataId) {
        if (this.length > 0) {
            var index = this.GetPositionOfElement(odataId);
            this.splice(index, 1);
        }
    };
}

exports.OdataIdArray = OdataIdArray;
