"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

from abc import abstractmethod


class PsmeStructureElement:
    @abstractmethod
    def __init__(self):
        pass

    def parse_to_dict(self):
        ret = dict()
        for key, value in self.__dict__.iteritems():
            if type(value) == type(list()):
                ret[key] = []
                for var in value:
                    ret[key].append(self._parse_value(var))
            else:
                ret[key] = self._parse_value(value)

        return ret

    def _parse_value(self, value):
        try:
            return value.parse_to_dict()
        except AttributeError:
            # is not model object, basic python type
            return value
