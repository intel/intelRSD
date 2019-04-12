"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
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


class ValueNotFound(BaseException):
    """
    Exception raised when
    """
    pass


class Configuration:
    """
    class for configuration objects passed to test cases
    """
    def __init__(self, **kwargs):
        for key, value in kwargs.iteritems():
            setattr(self, key, value)

    def update_configuration(self, **kwargs):
        for key, value in kwargs.iteritems():
            setattr(self, key, value)

    def __getattr__(self, item):
        if item not in self.__dict__.keys():
            raise ValueNotFound("Config flag \"%s\" missing" % item)
        else:
            return self.__dict__[item]

    def __getitem__(self, item):
        try:
            return self.__dict__[item]
        except KeyError:
            raise ValueNotFound("Config flag \"%s\" missing" % item)

    def __repr__(self):
        return str("{!r}").format(self.__dict__)
