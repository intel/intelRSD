#! /usr/bin/python3

# Copyright (c) 2016 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
File : odata_validator.py

Author : Craig Janeczek <Craig.Janeczek@emc.com>

Brief : This file contains the definitions and functionalities for OData validation. It provides the
        ability to crawl through OData Metadata files, parse them and validate that they are OData
        4.0 compliant.

Copyright(c) 2016 EMC Corporation 

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

import xml.etree.ElementTree as ET
import copy
import collections
import base64
import datetime
import re
import sys
import unicodedata
import os
import argparse
import urllib.request

global_namespaces = {}

CSDL_NAMES = ['Edmx', 'DataServices', 'Reference', 'Include', 'IncludeAnnotations', 'Schema',
              'Property', 'NavigationProperty', 'ReferentialConstraint', 'OnDelete', 'EntityType',
              'Key', 'PropertyRef', 'ComplexType', 'EnumType', 'Member', 'TypeDefinition',
              'Action', 'Function', 'ReturnType', 'Parameter', 'EntityContainer', 'EntitySet',
              'Singleton', 'NavigationPropertyBinding', 'ActionImport', 'FunctionImport', 'Term',
              'Annotations', 'Annotation', 'Binary', 'Bool', 'Date', 'DateTimeOffset', 'Decimal',
              'Duration', 'EnumMember', 'Float', 'Guid', 'Int', 'String', 'TimeOfDay', 'And', 'Or',
              'Not', 'Eq', 'Ne', 'Gt', 'Ge', 'Lt', 'Le', 'AnnotationPath', 'Apply', 'Cast',
              'Collection', 'If', 'IsOf', 'LabeledElement', 'LabeledElementReference', 'Null',
              'NavigationPropertyPath', 'Path', 'PropertyPath', 'Record', 'UrlRef']

UNICODE_CATEGORY = collections.defaultdict(list)
for c in [chr(x) for x in range(sys.maxunicode + 1)]:
    UNICODE_CATEGORY[unicodedata.category(c)].append(c)

UNICODE_CATEGORY['LC'] = UNICODE_CATEGORY['Lu'] + UNICODE_CATEGORY['Ll'] + UNICODE_CATEGORY['Lt']

UNICODE_CATEGORY['L'] = (UNICODE_CATEGORY['Lu'] + UNICODE_CATEGORY['Ll'] + UNICODE_CATEGORY['Lt'] +
                         UNICODE_CATEGORY['Lm'] + UNICODE_CATEGORY['Lo'])

UNICODE_CATEGORY['M'] = UNICODE_CATEGORY['Mn'] + UNICODE_CATEGORY['Mc'] + UNICODE_CATEGORY['Me']

UNICODE_CATEGORY['N'] = UNICODE_CATEGORY['Nd'] + UNICODE_CATEGORY['Nl'] + UNICODE_CATEGORY['No']

UNICODE_CATEGORY['P'] = (UNICODE_CATEGORY['Pc'] + UNICODE_CATEGORY['Pd'] + UNICODE_CATEGORY['Ps'] +
                         UNICODE_CATEGORY['Pe'] + UNICODE_CATEGORY['Pi'] + UNICODE_CATEGORY['Pf'] +
                         UNICODE_CATEGORY['Po'])

UNICODE_CATEGORY['S'] = (UNICODE_CATEGORY['Sm'] + UNICODE_CATEGORY['Sc'] + UNICODE_CATEGORY['Sk'] +
                         UNICODE_CATEGORY['So'])

UNICODE_CATEGORY['Z'] = UNICODE_CATEGORY['Zs'] + UNICODE_CATEGORY['Zl'] + UNICODE_CATEGORY['Zp']

UNICODE_CATEGORY['C'] = (UNICODE_CATEGORY['Cc'] + UNICODE_CATEGORY['Cf'] + UNICODE_CATEGORY['Cs'] +
                         UNICODE_CATEGORY['Co'] + UNICODE_CATEGORY['Cn'])

SIMPLE_ID_RE = re.compile(
    u'[{L}{Nl}_][{L}{Nl}{Nd}{Mn}{Mc}{Pc}{Cf}]{{0,}}'.format(L=''.join(UNICODE_CATEGORY['L']),
                                                            Nl=''.join(UNICODE_CATEGORY['Nl']),
                                                            Nd=''.join(UNICODE_CATEGORY['Nd']),
                                                            Mn=''.join(UNICODE_CATEGORY['Mn']),
                                                            Mc=''.join(UNICODE_CATEGORY['Mc']),
                                                            Pc=''.join(UNICODE_CATEGORY['Pc']),
                                                            Cf=''.join(UNICODE_CATEGORY['Cf'])))

NAMESPACE_RE = re.compile((u'[{L}{Nl}_][{L}{Nl}{Nd}{Mn}{Mc}{Pc}{Cf}]{{0,}}'
                           '(\.[{L}{Nl}_][{L}{Nl}{Nd}{Mn}{Mc}{Pc}{Cf}]{{0,}}){{0,}}'
                          ).format(L=''.join(UNICODE_CATEGORY['L']),
                                   Nl=''.join(UNICODE_CATEGORY['Nl']),
                                   Nd=''.join(UNICODE_CATEGORY['Nd']),
                                   Mn=''.join(UNICODE_CATEGORY['Mn']),
                                   Mc=''.join(UNICODE_CATEGORY['Mc']),
                                   Pc=''.join(UNICODE_CATEGORY['Pc']),
                                   Cf=''.join(UNICODE_CATEGORY['Cf'])))

QUALIFIED_NAME_RE = re.compile((u'[{L}{Nl}_][{L}{Nl}{Nd}{Mn}{Mc}{Pc}{Cf}]{{0,}}'
                                '(\.[{L}{Nl}_][{L}{Nl}{Nd}{Mn}{Mc}{Pc}{Cf}]{{0,}}){{1,}}'
                               ).format(L=''.join(UNICODE_CATEGORY['L']),
                                        Nl=''.join(UNICODE_CATEGORY['Nl']),
                                        Nd=''.join(UNICODE_CATEGORY['Nd']),
                                        Mn=''.join(UNICODE_CATEGORY['Mn']),
                                        Mc=''.join(UNICODE_CATEGORY['Mc']),
                                        Pc=''.join(UNICODE_CATEGORY['Pc']),
                                        Cf=''.join(UNICODE_CATEGORY['Cf'])))

# workaround
QUALIFIED_NAME_RE = re.compile(u'.+')
NAMESPACE_RE = re.compile(u'.+')
SIMPLE_ID_RE = re.compile(u'.+')

class SchemaError(Exception):
    """Error for catching all syntax errors in Schema files.

    Seperate class so we can catch it and create an easy to read error chain which will point to the
    schema error by crawling from metadata root down throuigh the elements.

    Attributes:
        message: Explanitory message for the current error
    """

    def __init__(self, message):
        self.message = message
        super().__init__(message)


def is_collection(data):
    """Determines whether type input is a collection.

    Splits the string type inputted and tests whether a collection is specified. Can be used to
    check for collection or strip collection away and isolate the type in question.

    Args:
        data: String which indicates the type to be parsed.

    Returns:
        A boolean indicating whether the type passed in is a collection.
        A string indicating the underlying type.
    """

    if data.startswith('Collection('):
        collection = True
        data = '('.join(data.split('(')[1:])
        data = ')'.join(data.split(')')[:-1])
    else:
        collection = False

    return collection, data


def parse_type_string(type_name):
    """Digests a type string.

    Splits a type string into whether it is a collection, the namespace and the element.

    Args:
        type_name: Type string to be parsed.

    Returns:
        A boolean indicating whether the type passed in is a collection.
        A string indicating the namespace of the type passed in.
        A string indicating the element of the type passed in.
    """

    collection, type_name = is_collection(type_name)

    split = type_name.split('.')

    element = split[-1]
    namespace = '.'.join(split[:-1])

    return collection, namespace, element


def check_type(data, type_name):
    """Checks the format of an OData identifier.

    Validates that the string passed in is valid for the OData identifier type passed in.

    Args:
        data: String to be verified.
        type_name: OData identifier type to verify the string against.

    Returns:
        None

    Raises:
        SchemaError: if the string passed in does not fit the format passed in.
    """

    if type_name == 'SimpleIdentifier':
        if len(data) > 128:
            raise SchemaError("{} is too long for SimpleIdentifier".format(data))
        if not SIMPLE_ID_RE.fullmatch(data):
            raise SchemaError("{} is not a valid SimpleIdentifier".format(data))

    elif type_name == 'Namespace':
        if len(data) > 511:
            raise SchemaError("{} is too long for Namespace".format(data))
        if not NAMESPACE_RE.fullmatch(data):
            raise SchemaError("{} is not a valid Namespace".format(data))

    elif type_name == 'QualifiedName':
        if not QUALIFIED_NAME_RE.fullmatch(data):
            raise SchemaError("{} is not a valid QualifiedName".format(data))

    elif type_name == 'TypeName':
        _, name = is_collection(data)
        if not QUALIFIED_NAME_RE.fullmatch(name):
            raise SchemaError("{} is not a valid TypeName".format(data))

    elif type_name == 'TargetPath':
        split = data.split('/')
        check_type('QualifiedName', split[0])
        for segment in split[1:]:
            check_type('SimpleIdentifier', segment)

    elif type_name == 'Boolean':
        if data not in ['true', 'false']:
            raise SchemaError("{} is not a valid Boolean".format(data))
    else:
        raise SchemaError("Type name {} is invalid".format(type_name))


#TODO: Flesh out these classes
class Type(object):
    """Base class for OData Types.
    """

    def __init__(self):
        super(Type, self).__init__()


class StructuredType(Type):
    """Base class for OData Structured Types.
    """

    def __init__(self):
        super(StructuredType, self).__init__()


class AbstractType(Type):
    """Base class for OData Abstract Types.

    This class verifies that the type passed in is actually an abstract type.

    Attributes:
        _types: Private list of OData abstract type classes.
        name: Name of the specific type this class represents.
        error_id: ID used if an error occurs with this class.
    """

    _types = ['PrimitiveType', 'ComplexType', 'EntityType', 'AnnotationPath', 'PropertyPath',
              'NavigationPropertyPath']

    def __init__(self, name):
        super(AbstractType, self).__init__()
        self.name = name
        self.error_id = name

        if self.name not in self._types:
            raise SchemaError("AbstractType {} is not defined".format(self.name))


class PrimitiveType(Type):
    """Base class for OData Primitive Types.

    This class acts as the interface for all primitive types, providing the capabilities for
    conversion and low level data checks.

    Attributes:
        _types: Private list of OData primitive types.
        name: Name of the specific type this class represents.
        error_id: ID used if an error occurs with this class.
    """

    _types = ['Binary', 'Boolean', 'Byte', 'Date', 'DateTimeOffset', 'Decimal', 'Double',
              'Duration', 'Guid', 'Int16', 'Int32', 'Int64', 'SByte', 'Single', 'Stream', 'String',
              'TimeOfDay', 'Geography', 'GeographyPoint', 'GeographyLineString',
              'GeographyPolygon', 'GeographyMultiPoint', 'GeographyMultiLineString',
              'GeographyMultiPolygon', 'GeographyCollection', 'Geometry', 'GeometryPoint',
              'GeometryLineString', 'GeometryPolygon', 'GeometryMultiPoint',
              'GeometryMultiLineString', 'GeometryMultiPolygon', 'GeometryCollection', 'Untyped']

    def __init__(self, name):
        super(PrimitiveType, self).__init__()
        self.name = name
        self.error_id = name

        if self.name not in self._types:
            raise SchemaError("PrimitiveType {} is not defined".format(self.name))

    def get_primitive_type(self):
        """Gets the primitive type of this element.

        Defined differently for each class, since this is a primitive type class return itself

        Returns:
            A class instance which is the primitive type of the element
        """

        return self

    def is_comparable(self, other_type):
        """Checks whether this type is comparable to another type.

        Looks for compatibility between this type and the passed in type and if they can be compared
        returns true.

        Args:
            other_type: the other type to be compared to this type

        Returns:
            A boolean indicating whether the other type is comparable to this type
        """

        other_type = other_type.get_primitive_type()

        ret_val = False

        if self.name == 'Untyped' or other_type.name == 'Untyped':
            ret_val = True
        elif all(item in ['Binary'] for item in [self.name, other_type.name]):
            ret_val = True
        elif all(item in ['Boolean'] for item in [self.name, other_type.name]):
            ret_val = True
        elif all(
                item in ['Byte', 'Decimal', 'Double', 'Int16', 'Int32', 'Int64', 'SByte', 'Single']
                for item in [self.name, other_type.name]):
            ret_val = True
        elif all(item in ['Date'] for item in [self.name, other_type.name]):
            ret_val = True
        elif all(item in ['DateTimeOffset'] for item in [self.name, other_type.name]):
            ret_val = True
        elif all(item in ['Duration'] for item in [self.name, other_type.name]):
            ret_val = True
        elif all(item in ['Guid'] for item in [self.name, other_type.name]):
            ret_val = True
        elif all(item in ['Stream'] for item in [self.name, other_type.name]):
            ret_val = True
        elif all(item in ['String'] for item in [self.name, other_type.name]):
            ret_val = True
        elif all(item in ['TimeOfDay'] for item in [self.name, other_type.name]):
            ret_val = True
        else:
            ret_val = False

        return ret_val

    def check_max_length_valid(self, attrib):
        """Checks whether the MaxLength attribute is valid for this type.

        The MaxLength attribute is only applicable for certain types, this function validates that.

        Args:
            attrib: The raw attributes to look for MaxLength in.

        Returns:
            None

        Raises:
            SchemaError: if MaxLength is applied but not applicable.
        """

        if self.name not in ['Binary', 'Stream', 'String']:
            if 'MaxLength' in attrib:
                raise SchemaError("MaxLength facet not applicable to {}".format(self.name))

    def check_precision_valid(self, attrib):
        """Checks whether the Precision attribute is valid for this type.

        The Precision attribute is only applicable for certain types, this function validates that.

        Args:
            attrib: The raw attributes to look for Precision in.

        Returns:
            None

        Raises:
            SchemaError: if Precision is applied but not applicable.
        """

        if self.name not in ['DateTimeOffset', 'Decimal', 'Duration', 'TimeOfDay']:
            if 'Precision' in attrib:
                raise SchemaError("Precision facet not applicable to {}".format(self.name))

    def check_precision_value(self, value):
        """Checks whether a value is a valid precision value.

        Looks at the curent type and the value passed in to determine validity.

        Args:
            value: value to check.

        Returns:
            None

        Raises:
            SchemaError: If value passed in is not a valid precision value for this type.
        """

        if self.name == 'Decimal':
            if value <= 0:
                raise SchemaError("Precision value {} invalid (> 0 for decimal)".format(value))
        else:
            if (value < 0) or (value > 12):
                raise SchemaError(
                    "Precision value {} invalid (0 <= value <= 12 for temporal)".format(value))

    def check_scale_valid(self, attrib):
        """Checks whether the Scale attribute is valid for this type.

        The Scale attribute is only applicable for certain types, this function validates that.

        Args:
            attrib: The raw attributes to look for Scale in.

        Returns:
            None

        Raises:
            SchemaError: if Scale is applied but not applicable.
        """

        if self.name not in ['Decimal']:
            if 'Scale' in attrib:
                raise SchemaError("Scale facet not applicable to {}".format(self.name))

    def get_scale_default(self):
        """Get the default scale precision value.

        Returns the default scale value for this type.

        Args:
            None

        Returns:
            A decimal if this type has a valid default, None if not.
        """

        if self.name == 'Decimal':
            return 0
        else:
            return None

    def check_unicode_valid(self, attrib):
        """Checks whether the Unicode attribute is valid for this type.

        The Unicode attribute is only applicable for certain types, this function validates that.

        Args:
            attrib: The raw attributes to look for Unicode in.

        Returns:
            None

        Raises:
            SchemaError: if Unicode is applied but not applicable.
        """

        if self.name not in ['String']:
            if 'Unicode' in attrib:
                raise SchemaError("Unicode facet not applicable to {}".format(self.name))

    def get_unicode_default(self):
        """Get the default unicode attribure value.

        Returns the default unicode value for this type.

        Args:
            None

        Returns:
            A boolean if this type has a valid default, None if not.
        """

        if self.name == 'String':
            return True
        else:
            return None

    def check_srid_valid(self, attrib):
        """Checks whether the Srid attribute is valid for this type.

        The Srid attribute is only applicable for certain types, this function validates that.

        Args:
            attrib: The raw attributes to look for Srid in.

        Returns:
            None

        Raises:
            SchemaError: if Srid is applied but not applicable.
        """

        if self.name not in ['Geometry', 'Geography']:
            if 'SRID' in attrib:
                raise SchemaError("SRID facet not applicable to {}".format(self.name))

    def get_srid_default(self):
        """Get the default srid attribute value.

        Returns the default srid value for this type.

        Args:
            None

        Returns:
            A decimal if this type has a valid default, None if not.
        """

        if self.name == 'Geometry':
            return 0
        elif self.name == 'Geography':
            return 4326
        else:
            return None

    def convert(self, data):
        """Converts a string to data.

        Takes advantage of python data types to convert the data from a string to more usable
        forms.

        Args:
            data: raw data

        Returns:
            Converted data based on this type.
        """

        if self.name in ['Int', 'Int16', 'Int32', 'Int64']:
            ret_value = int(data)
        elif self.name == 'Binary':
            try:
                ret_value = base64.b64decode(data, altchars=['-', '_'], validate=True)
            except Exception as error:
                raise SchemaError("Data {} is invalid ({})".format(data, error))
        elif self.name == 'Boolean':
            if data not in ['true', 'false']:
                raise SchemaError("Invalid data for Boolean {}".format(data))
            ret_value = (data == 'true')
        elif self.name == 'Date':
            year, month, day = data.split('-')
            ret_value = {'year': int(year), 'month': int(month), 'day': int(day)}
        elif self.name == 'DateTimeOffset':
            values = data.split('T')

            if len(values) != 2:
                raise SchemaError('Data {} does not have the format Date T Time'.format(data))

            date = values[0]

            date_split = date.split('-')
            if len(date_split) != 3:
                raise SchemaError("Data {} does not have 3 elements".format(date_split))

            year = int(date_split[0])
            month = int(date_split[1])
            day = int(date_split[2])

            if year > 9999:
                raise SchemaError("Year {} is invalid (0000-9999)".format(year))

            if month > 12 or month == 0:
                raise SchemaError("Month {} is invalid (1-12)".format(month))

            if day == 0:
                raise SchemaError("Day {} is invalid (!= 0)".format(day))

            if month in [4, 6, 9, 11]:
                if day > 30:
                    raise SchemaError("Day {} invalid for month {} (1-30)".format(day, month))
            elif month == 2 and ((year % 4 != 0) or ((year % 100 == 0) and (year % 400 != 0))):
                if day > 28:
                    raise SchemaError("Day {} invalid for month {} and year {} (1-28)".format(
                        day, month, year))
            elif month == 2 and ((year % 400 == 0) or (year % 4 == 0)) and (year % 100 != 0):
                if day > 29:
                    raise SchemaError("Day {} invalid for month {} and year {} (1-29)".format(
                        day, month, year))
            else:
                if day > 31:
                    raise SchemaError("Day {} invalid for month {} (1-31)".format(day, month))

            time = values[1]

            if 'Z' in time:
                if time[-1] != 'Z':
                    raise SchemaError('Time {} has invalid format'.format(time))
                offset_hr = 0
                offset_min = 0
                time = time.replace('Z', '')
            elif '-' in time:
                tmp = time.split('-')
                time = tmp[0]
                offset = tmp[1]
                offset_split = offset.split(':')
                if len(offset_split) != 2:
                    raise SchemaError("Offset {} has invalid format (HH:MM)".format(offset))
                offset_hr = -1 * int(offset_split[0])
                offset_min = int(offset_split[1])
            elif '+' in time:
                tmp = time.split('+')
                time = tmp[0]
                offset = tmp[1]
                offset_split = offset.split(':')
                if len(offset_split) != 2:
                    raise SchemaError("Offset {} has invalid format (HH:MM)".format(offset))
                offset_hr = int(offset_split[0])
                offset_min = int(offset_split[1])
            else:
                raise SchemaError("Invalid offset format")

            time_split = time.split('.')
            time = time_split[0]
            if len(time_split) == 2:
                frac_second = int(time_split[1])
            else:
                frac_second = 0

            time_split = time.split(':')
            if len(time_split) < 2:
                raise SchemaError("Time {} does not have at least 2 elements".format(time_split))

            hour = int(time_split[0])
            minute = int(time_split[1])
            if len(time_split) == 3:
                second = int(time_split[2])
            else:
                second = 0

            time_zone = TimeZone(offset_hr, offset_min)
            ret_value = datetime.datetime(
                year, month, day, hour, minute, second, frac_second, time_zone)
        elif self.name == 'Decimal':
            ret_value = float(data)
        elif self.name == 'Duration':
            if data[0] == '-':
                sign = -1
            else:
                sign = 1

            duration = data.split('P')[1]

            year = 0
            month = 0
            day = 0
            hour = 0
            minute = 0
            second = 0
            frac_second = 0
            keys = [('Y', year), ('M', month), ('D', day), ('H', hour), ('M', minute)]

            for key in keys:
                if key[0] in duration:
                    split = duration.split(key[0])
                    if len(split) != 2:
                        raise SchemaError('Invalid {} tag in duration {}'.format(key[0], duration))
                    key[1] = int(split[0])
                    duration = split[1]

            if 'S' in duration:
                split = duration.split('S')
                if len(split) != 1:
                    raise SchemaError('Invalid S tag in duration {}'.format(duration))

                split = split[0].split('.')
                second = int(split[0])
                if len(split) > 1:
                    frac_second = int(split[1])

            #TODO:convert to timedelta and take sign into account
            ret_value = datetime.datetime(year, month, day, hour, minute, second, frac_second)
        elif self.name == 'EnumMember':
            #TODO: convert to enum member
            ret_value = data
        elif self.name == 'Float':
            ret_value = float(data)
        elif self.name == 'Guid':
            split = data.split('-')

            if len(split) != 5:
                raise SchemaError('data {} invalid (8dig-4dig-4dig-4dig-12-dig)'.format(data))

            if (len(split[0]) != 8 or len(split[1]) != 4 or len(split[2]) != 4 or
                len(split[3]) != 4 or len(split[4]) != 12):
                raise SchemaError('data {} invalid (8dig-4dig-4dig-4dig-12-dig)'.format(data))

            ret_value = data
        elif self.name == 'String':
            ret_value = str(data)
        elif self.name == 'TimeOfDay':
            split = data.split('.')
            frac_second = 0
            if len(split) > 1:
                frac_second = int(split[1])
                data = split[0]
            split = data.split(':')
            hour = 0
            minute = 0
            second = 0
            try:
                hour = int(split[0])
                minute = int(split[1])
                second = int(split[2])
            except Exception:
                pass

            #TODO: convert to time
            ret_value = {'hour': hour,
                         'minute': minute,
                         'second': second,
                         'frac_second': frac_second}
        else:
            raise SchemaError("Cant convert {}".format(self.name))
        return ret_value

class Element(object):
    """Base class of all OData element classes.

    Attributes:
        _constant_expression_list: A list of all of the valid ConstantExpression names.
        _dynamic_expression_list: A list of all of the valid DynamicExpression names.
        annotation: An array of Annotation elements that are appplied to this element.
        raw_data: The raw xml data for this element.
        error_id: The identifier that will be used when an error occurs in this element or any of
                  its children element to locate the error.
        parent: The parent element for this element.
        children: An array of children elements.
        expressions: An array of constant and dynamic expressions applied to this element.
    """

    _constant_expression_list = ['Binary', 'Bool', 'Date', 'DateTimeOffset', 'Decimal', 'Duration',
                                 'EnumMember', 'Float', 'Guid', 'Int', 'String', 'TimeOfDay']

    _dynamic_expression_list = ['And', 'Or', 'Not', 'Eq', 'Ne', 'Gt', 'Lt', 'Le', 'AnnotationPath',
                                'Apply', 'Cast', 'Collection', 'If', 'IsOf', 'LabeledElement',
                                'LabeledElementReference', 'Null', 'NavigationPropertyPath',
                                'Path', 'PropertyPath', 'Record', 'PropertyValue', 'UrlRef']

    def __init__(self, data, parent):
        self.annotation = []
        self.raw_data = data
        self.error_id = None
        self.parent = parent
        self.children = []
        self.expressions = []
        try:
            self.parse()
        except SchemaError as error:
            if self.error_id:
                raise SchemaError("{}:{}->{}".format(
                    type(self).__name__, self.error_id, error.message))
            else:
                raise SchemaError("{}->{}".format(type(self).__name__, error.message))
        except Exception:
            raise

    def _get_annotations(self):
        """Parses all Annotation elements applied to this element.

        Searches for and parses all Annotation elements applied to this element. This includes
        checking to ensure a term and qualifier combo is not applied more than one time per element.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is duplication of term/qualifier combo
        """

        used_terms = []
        for annotation in self._get_elements('Annotation'):
            data = Annotation(annotation, self)
            if (data.term, data.qualifier) in used_terms:
                raise SchemaError(("Term {} and qualifier {} "
                                   "specified more than once").format(data.term, data.qualifier))
            used_terms.append((data.term, data.qualifier))
            self.annotation.append(data)
            self.children.append(data)

    def _get_elements(self, element_name):
        """Find all elements of given type for this element.

        Searches the raw data for an XML repentation of a child element of the type passed in. This
        function finds all instances of the passed in type.

        Args:
            element_name: CSDL name for the child element type to search for.

        Returns:
            A list of raw XML representations for the children elements of the passed in type.
        """

        return [element
                for element in self.raw_data
                if element.tag.endswith('}}{name}'.format(name=element_name))]

    def _get_required_attrib(self, attrib_name):
        """Looks for an attribute of this element that must be there.

        Args:
            attrib_name: Name of the required attribute to look for.

        Returns:
            The attribute value if found.

        Raises:
            SchemaError: if the attribute was not found.
        """

        if attrib_name not in self.raw_data.attrib:
            raise SchemaError("Required attribute {} is missing".format(attrib_name))
        else:
            return self.raw_data.attrib[attrib_name]

    def _get_expressions(self):
        """Finds all Expression children of this element.

        This looks for and parses all constant and dynamic expressions in both element and attribute
        form for this element.

        Args:
            None

        Returns:
            None
        """

        for data in self.raw_data:
            for key in self._constant_expression_list:
                if data.tag.endswith('}}{name}'.format(name=key)):
                    self.expressions.append(ConstantExpression(data, key, 'element', self))
            for key in self._dynamic_expression_list:
                if data.tag.endswith('}}{name}'.format(name=key)):
                    self.expressions.append(DynamicExpression(data, key, 'element', self))

        for key in self._constant_expression_list:
            if key in self.raw_data.attrib:
                self.expressions.append(
                    ConstantExpression(self.raw_data.attrib[key], key, 'attribute', self))

        for key in self._dynamic_expression_list:
            if key in self.raw_data.attrib:
                self.expressions.append(
                    DynamicExpression(self.raw_data.attrib[key], key, 'attribute', self))

        for element in self.expressions:
            self.children.append(element)

    def check_scope_iterate(self):
        """Iterator function for checking the scope.

        This function handles crawling through this element's children and calling the check scope
        iterator before checking the scope and annotations of this element.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in the check scope functions
        """

        for element in self.children:
            try:
                element.check_scope_iterate()
            except SchemaError as error:
                if self.error_id:
                    raise SchemaError("{}:{}->{}".format(
                        type(self).__name__, self.error_id, error.message))
                else:
                    raise SchemaError("{}->{}".format(type(self).__name__, error.message))
            except Exception:
                raise

        try:
            self.check_scope()
            self._check_annotations()
        except SchemaError as error:
            if self.error_id:
                raise SchemaError("{}:{}->{}".format(
                    type(self).__name__, self.error_id, error.message))
            else:
                raise SchemaError("{}->{}".format(type(self).__name__, error.message))
        except Exception:
            raise

    def check_scope(self):
        """Default function used to check scope errors.

        This default function will raise no errors and should be overridden in elements that could
        raise scope errors.

        Args:
            None

        Returns:
            True
        """

        return True

    def _check_annotations(self):
        """Checks for annotation errors.

        Specifically this checks that the annotations are not validating the term's applies_to
        parameter.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If an annotation is illegally applying a term to this element.
        """

        for annotation in self.annotation:
            term = self.find_in_scope(annotation.term)
            if term.applies_to:
                found_element = False
                for element in term.applies_to:
                    if type(self).__name__ == element:
                        found_element = True
                if not found_element:
                    raise SchemaError("Term {} does not apply to this type".format(term.name))

    def evaluate_expression(self, element):
        """Parses a passed in expression element.

        Takes in a raw XML form expression and parses and validates it.

        Args:
            element: Raw XML formatted expression to parse.

        Returns:
            An object for the parsed expression
        """

        for key in self._constant_expression_list:
            if element.tag.endswith('}}{}'.format(key)):
                return ConstantExpression(element, key, 'element', self)
        for key in self._dynamic_expression_list:
            if element.tag.endswith('}}{}'.format(key)):
                return DynamicExpression(element, key, 'element', self)

        return None

    def validate_path(self, path):
        """Validates an OData path.

        Breaks apart a path string and validates that they are all OData compliant and that they can
        all be traversed.

        Args:
            path: The path string to validate

        Returns:
            None

        Raises:
            SchemaError: If the path passed in is not valid or cannot be traversed.
        """

        segments = path.split('/')

        for segment in segments:
            try:
                check_type(segment, 'QualifiedName')
                #TODO: Ensure segment is a type in scope
                #TODO: If the instance identified by the preceding path part cannot be cast to the
                #      specified type, the path expression evaluates to the null value.
                continue
            except Exception:
                pass

            if segment[0] == '@':
                #TODO: check term and qualifier in scope
                term_cast = True
                split = segment[1:].split('#')

                term = split[0]
                check_type(term, 'QualifiedName')

                if len(split) == 2:
                    qualifier = split[1]
                    check_type(qualifier, 'SimpleIdentifier')

                #TODO: If the instance identified by the preceding path part has been annotated
                #      with that term (and if present, with that qualifier), the term cast evaluates
                #      to the value of that annotation, otherwise it evaluates to the null value.
                continue

            try:
                check_type(segment, 'SimpleIdentifier')
                #TODO: MUST be the name of a structural property or a navigation property of the
                #      instance identified by the preceding path part
                continue
            except Exception:
                pass

            #TODO: When used within an edm:Path expression, a path may contain at most one segment
            #      representing a multi-valued structural or navigation property. The result of the
            #      expression is the collection of instances resulting from applying the remaining
            #      path to each instance in the multi-valued property.

            if '@' in segment:
                #TODO: check term and qualifier in scope
                split = segment.split('@')
                nav_prop = split[0]

                split = split[1].split('#')
                term = split[0]
                check_type(term, 'QualifiedName')

                if len(split) == 2:
                    qualifier = split[1]
                    check_type(qualifier, 'SimpleIdentifier')

                #TODO:  If the instance identified by the preceding path part has been annotated
                #       with that term (and if present, with that qualifier), the term cast
                #       evaluates to the value of that annotation, otherwise it evaluates to the
                #       null value.
                continue

            ##!!!!!BREAK
            #TODO: check embedded annotations

            if segment == segments[-1]:
                #TODO: if second to last value is multi-values
                if segment == '$count':
                    continue
            raise SchemaError('Segment {} invalid'.format(segment))

    def find_in_scope(self, path):
        """Converts a path string to a parsed object.

        After the whole OData tree has been parsed, this function can navigate through it and return
        the parsed object the path points to. This can be used to pull out types or other
        relationships and operate on them.

        Args:
            path: Path pointing to the object to find.

        Returns:
            The object pointed to by the path.

        Raises:
            SchemaError: If there was an issue traversing the path to find the element.
        """

        initial_path = path
        _, path = is_collection(path)
        metadata = self.get_first_parent_of_type(MetaData)
        if metadata is None:
            raise SchemaError("Not able to get back to parent MetaData definition")

        split = path.split('.')
        namespace = '.'.join(split[:-1])
        path = split[-1]

        if namespace in metadata.namespaces['!Included Alias']:
            namespace = metadata.namespaces['!Included Alias'][namespace]
        if namespace in metadata.namespaces:
            target_element = metadata.namespaces[namespace]
        else:
            raise SchemaError("namespace {} not found in scope".format(namespace))

        if path:
            segments = path.split('/')
            for segment in segments:
                found = False
                for child in target_element.children:
                    if child.error_id == segment:
                        target_element = child
                        found = True
                        break
                if not found:
                    try:
                        new_target = target_element.find_in_scope(target_element.type)
                        for child in new_target.children:
                            if child.error_id == segment:
                                target_element = child
                                found = True
                                break
                    except Exception:
                        raise SchemaError("segment {} in path {} not found".format(segment,
                                                                                   initial_path))

                    if not found:
                        raise SchemaError("segment {} in path {} not found".format(segment,
                                                                                   initial_path))

        if hasattr(target_element, 'validate_type'):
            target_element.validate_type(self)

        return target_element

    def get_element_path_from_uri(self):
        """Builds up human readable path string.

        Build up a string that points to this element in a human readable format. This is used by
        logging and error reporting.

        Args:
            None

        Returns:
            String representing the path from the root metadata down to this element
        """

        if self.error_id:
            path = "{}:{}".format(type(self).__name__, self.error_id)
        else:
            path = "{}".format(type(self).__name__)

        pivot = self
        while pivot.parent != None:
            pivot = pivot.parent
            if pivot.error_id:
                path = "{}:{}->{}".format(type(pivot).__name__, pivot.error_id, path)
            else:
                path = "{}->{}".format(type(pivot).__name__, path)

        return path

    def get_first_parent_of_type(self, class_type):
        """Gets the first parent of this element of the given type.

        Takes in a string name or python class type and iteratively loops through this elements
        parents until an instance of the passed in type is found.

        Args:
            class_type: String representation or python class name for the class to look for

        Returns:
            Instance of the parent class or None if not found
        """

        if isinstance(class_type, str):
            if class_type == 'EntityType':
                class_type = EntityType
            elif class_type == 'ComplexType':
                class_type = ComplexType
            else:
                return None

        parent_instance = self
        while not isinstance(parent_instance, class_type):
            if parent_instance.parent is None:
                return None
            parent_instance = parent_instance.parent
        return parent_instance


class TimeZone(datetime.tzinfo):
    """Custom timezone class.

    Attributes:
        hour: Hour offset from UTC.
        minute: Minute offset from UTC.
    """

    def __init__(self, hour, minute):
        super(TimeZone, self).__init__(hour, minute)
        self.hour = hour
        self.minute = minute

    def utcoffset(self, dt):
        return datetime.timedelta(hours=self.hour, minutes=self.minute)

    def tzname(self, dt):
        return "UTC {}:{}".format(self.hour, self.minute)

    def dst(self, dt):
        return datetime.timedelta(0)

class EdmNamespace(object):
    """Spoofed namespace for Edm including primitive and abstract types.

    Attributes:
        _prim_types: List of all possible primitive types.
        _abstract_types: List of all possible abstract types.
        children: All children of the EDM Namespace.
        data_services: Self defined data DataServices.
    This will include all of the primitive types and any other built in data
    """

    _prim_types = ['Binary', 'Boolean', 'Byte', 'Date', 'DateTimeOffset', 'Decimal', 'Double',
                   'Duration', 'Guid', 'Int16', 'Int32', 'Int64', 'SByte', 'Single', 'Stream',
                   'String', 'TimeOfDay', 'Geography', 'GeographyPoint', 'GeographyLineString',
                   'GeographyPolygon', 'GeographyMultiPoint', 'GeographyMultiLineString',
                   'GeographyMultiPolygon', 'GeographyCollection', 'Geometry', 'GeometryPoint',
                   'GeometryLineString', 'GeometryPolygon', 'GeometryMultiPoint',
                   'GeometryMultiLineString', 'GeometryMultiPolygon', 'GeometryCollection',
                   'Untyped']

    _abstract_types = ['PrimitiveType', 'ComplexType', 'EntityType', 'AnnotationPath',
                       'PropertyPath', 'NavigationPropertyPath']


    def __init__(self):
        self.children = []
        self.data_services = {}

        for type_name in self._prim_types:
            data = PrimitiveType(type_name)
            self.children.append(data)
            self.data_services[type_name] = data

        for type_name in self._abstract_types:
            data = AbstractType(type_name)
            self.children.append(data)
            self.data_services[type_name] = data

class MetaData(Element):
    """Class defining OData MetaData Elements.

    Derived from:
    Element edmx.Edmx
    Root of a CSDL document
    OData Version 4.0 Part 3 Section 3.1

    Attributes:
        parent: Parent element if applicable.
        uri: URI where this metadata document is located.
        error_id: ID used to name this metadata if an error occurs while processing it.
        data: Raw XML data of this metadata document.
        namespaces: Dictionary of the parsed namespaces referenced or defined in this document.
        annotation: Array of Annotation elements applied to this metadata document.
        references: Array of Reference elements applied to this metadata document.
        children: Array of all direct children elements of this metadata document.
    """

    def __init__(self, uri):

        self.parent = None
        self.uri = uri
        self.error_id = uri
        if self.uri.lower().startswith('http'):
            req = urllib.request.Request(self.uri)
            response = urllib.request.urlopen(req)
            self.data = response.read()
            self.raw_data = ET.fromstring(self.data)
        else:
            self.data = ET.parse(self.uri)
            self.raw_data = self.data.getroot()
        # Start with a ! to ensure it does not overlap with a possible namespace name
        self.namespaces = {'!Included Alias': {}}
        self.annotation = []
        self.references = []
        self.children = []

        try:
            self.parse_meta_data()
        except SchemaError as error:
            print("{}:{}->{}".format(type(self).__name__, self.uri, error.message))
            sys.exit(0)
        except Exception:
            raise

        try:
            self.check_scope_iterate()
        except SchemaError as error:
            print(error.message)
            sys.exit(0)
        except Exception:
            raise

    def parse_meta_data(self):
        """Parse this metadata document.

        Walks though all children and parses them.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there is an error parsing the metadata.
         """

        self.version = self._get_required_attrib('Version')

        if self.version != '4.0':
            raise SchemaError("Version {} is not 4.0".format(self.version))

        # Parse the DataServices
        defined_data_services = self._get_elements('DataServices')
        if len(defined_data_services) != 1:
            raise SchemaError("defines {} DataServices, not the required 1".format(len(
                defined_data_services)))

        self.data_services = DataServices(defined_data_services[0], self)
        self.children.append(self.data_services)

        self.namespaces = self.data_services.get_namespaces()

        self.namespaces['!Included Alias'] = self.data_services.get_aliases()
        self.namespaces['Edm'] = EdmNamespace()

        global_namespaces[self.uri] = self

        # Parse the included namespaces
        for reference in self._get_elements('Reference'):
            self.references.append(Reference(reference, self))
        for reference in self.references:
            self.children.append(reference)
            reference.generate_reference_dictionary(self.namespaces)


class DataServices(Element):
    """Class defining an OData DataServices Element.

    Element edm.DataServices
    Data services for a CSDL document
    OData Version 4.0 Part 3 Section 3.2

    Attributes:
        schemas: Array of schemas defined underneath this DataServices element
    """

    def __init__(self, data, parent):
        self.schemas = []

        super(DataServices, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        defined_schemas = self._get_elements('Schema')
        if len(defined_schemas) == 0:
            raise SchemaError("must contain at least 1 Schema, contains {}".format(len(
                defined_schemas)))

        for schema in defined_schemas:
            self.schemas.append(Schema(schema, self))

        schema_names = []
        for schema in self.schemas:
            if schema.name in schema_names:
                raise SchemaError("Schema name {} is not unique within this document".format(
                    schema.name))

            schema_names.append(schema.name)
            self.children.append(schema)

    def get_namespaces(self):
        """Get all child namespaces of this element.

        Creates a dictionary of schema elements indexed by their names.

        Args:
            None

        Returns:
            A dictionary of all child namespaces of this element.
        """

        namespace_dict = {}
        for schema in self.schemas:
            namespace_dict[schema.name] = schema
        return namespace_dict

    def get_namespace_list(self):
        """Get a detailed list of all child namespaces of this element.

        Provides a dictionary for each child namespace which breaks out the name, alias and link to
        the actual schema object.

        Args:
            None

        Returns:
            A list of all schemas broken out in dictionary form.
        """

        data = []
        for schema in self.schemas:
            data.append({'name': schema.name, 'alias': schema.alias, 'data services': schema})
        return data

    def get_aliases(self):
        """Gets a dictionary of all child aliases.

        Loops through the child schemas and validates that the aliases are legal. Also checks to see
        whether or not an alias is used more than one time.

        Args:
            None

        Returns:
            Dictionary mapping the alias name to the namespace name.
        """

        alias_dict = {}
        for schema in self.schemas:
            if schema.alias:
                if schema.alias in alias_dict.keys():
                    raise SchemaError("{}:{}->Alias {} used more than one time".format(
                        type(self).__name__, self.error_id, schema.alias))
                alias_dict[schema.alias] = schema.name

        return alias_dict


class Reference(Element):
    """Class defining an OData Reference element.

    Element edmx.Reference
    Specifies external CSDL documents referenced by the referencing document
    OData Version 4.0 Part 3 Section 3.3

    Attributes:
        includes: Array of child Include elements.
        annotation_includes: Array of child IncludeAnnotation elements.
        uri: The URI of the metadata file this Reference points to.
    """

    def __init__(self, data, parent):
        self.includes = []
        self.annotation_includes = []
        self.uri = None

        super(Reference, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.uri = self._get_required_attrib('Uri')
        self.error_id = self.uri

        defined_includes = self._get_elements('Include')
        defined_include_annotations = self._get_elements('IncludeAnnotations')

        if (len(defined_includes) == 0) and (len(defined_include_annotations) == 0):
            raise SchemaError("No Include or IncludeAnnotations children, need at least one")

        for include in defined_includes:
            data = Include(include, self)
            self.includes.append(data)
            self.children.append(data)

        for include_annotation in defined_include_annotations:
            data = IncludeAnnotations(include_annotation, self)
            self.annotation_includes.append(data)
            self.children.append(data)

        self._get_annotations()

    def generate_reference_dictionary(self, ref_dict):
        """Builds up a dictionary of the namespaces included.

        Called by a higher level object on multiple instances of referece elements to build up a
        list of all of the namespaces that are in the scope of that document.

        Args:
            ref_dict: The dictionary to add the namespaces this element references to.

        Returns:
            None

        Raises:
            SchemaErrror: If there are namespace/alias collisions or the included namespace is not
                          in the file specified.
        """

        for include in self.includes:
            include_found = False

            # Check if this namespace has already been included
            if include.namespace in ref_dict.keys():
                raise SchemaError("{}:{}->Namespace {} has already been included".format(
                    type(self).__name__, self.error_id, include.namespace))

            # Parse the CSDL document if we have not already
            if self.uri not in global_namespaces:
                MetaData(self.uri)

            # Search for the included namespace in the CSDL document
            for schema in global_namespaces[self.uri].data_services.get_namespace_list():
                if include.namespace == schema['name']:
                    include_found = True
                    ref_dict[include.namespace] = schema['data services']
                    if include.alias:
                        if include.alias in ref_dict['!Included Alias'].keys():
                            raise SchemaError("{}:{}->Alias {} used more than one time".format(
                                type(self).__name__, self.error_id, include.alias))
                        ref_dict['!Included Alias'][include.alias] = include.namespace
            if not include_found:
                raise SchemaError("{}:{}->Namespace {} not found in file specified".format(
                    type(self).__name__, self.error_id, include.namespace))
#TODO: loop through IncludeAnnotations and import annotations as appropriate


class Include(Element):
    """Class defining an OData Inlcude Element

    Element edmx.Include
    Specify the schemas to include from the target document
    OData Version 4.0 Part 3 Section 3.4

    Attributes:
        namespace: Namespace value for the namespace that is being included.
        alias: Optional SimpleIdentifier which can be used to alias the included namespace.
    """

    def __init__(self, data, parent):
        self.namespace = None
        self.alias = None

        super(Include, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.namespace = self._get_required_attrib('Namespace')
        self.error_id = self.namespace
        check_type(self.namespace, 'Namespace')

        if 'Alias' in self.raw_data.attrib:
            self.alias = self.raw_data.attrib['Alias']
            check_type(self.alias, 'SimpleIdentifier')
            if self.alias in ['Edm', 'odata', 'System', 'Transient']:
                raise SchemaError("Alias value {} is reserved".format(self.alias))
        else:
            self.alias = None


class IncludeAnnotations(Element):
    """Class defining an OData IncludeAnnotations element.

    Element edmx.IncludeAnnotations
    Specify the annotations to include from the target document
    OData Version 4.0 Part 3 Section 3.5

    Attributes:
        term_namespace: A Namespace value indicating the namespace to import terms from.
        qualifier: Optional SimpleIdentifier which can be used to restrict the terms imported by
                   only including terms with the specified Qualifier value.
        target_namespace: Optional Namespace value which can be used to restrict the terms imported
                          by only including terms applied to an element of the target_namespace.
    """

    def __init__(self, data, parent):
        self.term_namespace = None
        self.qualifier = None
        self.target_namespace = None

        super(IncludeAnnotations, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.term_namespace = self.raw_data.attrib['TermNamespace']
        self.error_id = self.term_namespace
        check_type(self.term_namespace, 'Namespace')

        self.qualifier = self.raw_data.attrib.get('Qualifier', None)
        check_type(self.term_namespace, 'SimpleIdentifier')

        self.target_namespace = self.raw_data.attrib.get('TargetNamespace', None)
        check_type(self.target_namespace, 'Namespace')

    #TODO:Write method to actually import the annotations


class Schema(Element):
    """Class defining an OData Schema Element.

    Element edm.Schema
    Schema for a namespace in a Data services for a CSDL document
    OData Version 4.0 Part 3 Section 5.1

    Attributes:
        data_services: Dictionary exposing the children elements of this schema.
        name: Namespace string uniquely identifying this Schema within the metadata document.
        alias: SimpleIdentifier string aliasing the namespace. This allows nominal types to be
               qualified with a short string rather than a long namespace.
    """

    def __init__(self, data, parent):
        self.data_services = {}
        self.name = None
        self.alias = None

        super(Schema, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self.raw_data.attrib['Namespace']
        self.error_id = self.name
        if self.name in ['Edm', 'odata', 'System', 'Transient']:
            raise SchemaError("Namespace {} is reserved".format(self.name))
        check_type(self.name, 'Namespace')

        if 'Alias' in self.raw_data.attrib:
            self.alias = self.raw_data.attrib['Alias']
            if self.alias in ['Edm', 'odata', 'System', 'Transient']:
                raise SchemaError("Alias {} is reserved".format(self.alias))
            check_type(self.alias, 'SimpleIdentifier')
        else:
            self.alias = None

        for data_type in self._get_elements('TypeDefinition'):
            data = TypeDefinition(data_type, self)
            if data.name in self.data_services.keys():
                raise SchemaError("{}:{}->Name {} is already defined in schema".format(
                    type(data).__name__, data.error_id, data.name))
            self.data_services[data.name] = data
            self.children.append(data)

        for enum_type in self._get_elements('EnumType'):
            data = EnumType(enum_type, self)
            if data.name in self.data_services.keys():
                raise SchemaError("{}:{}->Name {} is already defined in schema".format(
                    type(data).__name__, data.error_id, data.name))
            self.data_services[data.name] = data
            self.children.append(data)

        for complex_type in self._get_elements('ComplexType'):
            data = ComplexType(complex_type, self)
            if data.name in self.data_services.keys():
                raise SchemaError("{}:{}->Name {} is already defined in schema".format(
                    type(data).__name__, data.error_id, data.name))
            self.data_services[data.name] = data
            self.children.append(data)

        for term in self._get_elements('Term'):
            data = Term(term, self)
            if data.name in self.data_services.keys():
                raise SchemaError("{}:{}->Name {} is already defined in schema".format(
                    type(data).__name__, data.error_id, data.name))
            self.data_services[data.name] = data
            self.children.append(data)

        for entity_type in self._get_elements('EntityType'):
            data = EntityType(entity_type, self)
            if data.name in self.data_services.keys():
                raise SchemaError("{}:{}->Name {} is already defined in schema".format(
                    type(data).__name__, data.error_id, data.name))
            self.data_services[data.name] = data
            self.children.append(data)

        for entity_container in self._get_elements('EntityContainer'):
            data = EntityContainer(entity_container, self)
            if data.name in self.data_services.keys():
                raise SchemaError("{}:{}->Name {} is already defined in schema".format(
                    type(data).__name__, data.error_id, data.name))
            self.data_services[data.name] = data
            self.children.append(data)

        self.data_services['!Annotations'] = []
        for annotations in self._get_elements('Annotations'):
            data = Annotations(annotations, self)
            self.data_services['!Annotations'].append(data)
            self.children.append(data)

        bound_actions = {}
        for action in self._get_elements('Action'):
            data = Action(action, self)
            self.children.append(data)
            if data.is_bound:
                if data.name in bound_actions:
                    if data.binding_parameter_type in bound_actions[data.name]:
                        raise SchemaError(
                            ("{}:{}-> combination of action name a binding parameter "
                             "is not unique in namespace").format(
                                 type(data).__name__, data.error_id))
                    else:
                        bound_actions[data.name].append(data.binding_parameter_type)
                else:
                    bound_actions[data.name] = [data.binding_parameter_type]

                if '!Bound Actions' in self.data_services:
                    self.data_services['!Bound Actions'].append(data)
                else:
                    self.data_services['!Bound Actions'] = [data]
            else:
                if data.name in self.data_services.keys():
                    raise SchemaError("{}:{}->Name {} is already defined in schema".format(
                        type(data).__name__, data.error_id, data.name))
                self.data_services[data.name] = data

        function_checks = {'Bound': {}, 'Unbound': {}}
        for function in self._get_elements('Function'):
            data = Function(function, self)
            self.children.append(data)
            if data.is_bound:
                if data.name in function_checks['Bound']:
                    if data.binding_parameter_type in function_checks['Bound'][data.name]:
                        unordered_names = collections.Counter([x.name for x in data.param_list[1:]
                                                              ])
                        if unordered_names in function_checks['Bound'][
                                data.name][data.binding_parameter_type][
                                    'Non-Binding Names']:
                            raise SchemaError(("{}:{}-> function name, binding parameter and "
                                               "unordered set of non-binding param names is not "
                                               "unique").format(
                                                   type(data).__name__, data.error_id))

                        param_types = [x.type for x in data.param_list]
                        if param_types in function_checks['Bound'][data.name][
                                data.binding_parameter_type]['Param Types']:
                            raise SchemaError(
                                ("{}:{}-> function name, binding parameter and "
                                 "ordered set of parameter types is not unique").format(
                                     type(data).__name__, data.error_id))

                        if data.return_type.type != function_checks['Bound'][
                                data.name][data.binding_parameter_type][
                                    'Ret Type']:
                            raise SchemaError(
                                ("{}:{}-> Does not have same return type as other "
                                 "functions with same name and bind param type").format(
                                     type(data).__name__, data.error_id))

                        function_checks['Bound'][data.name][
                            data.binding_parameter_type][
                                'Non-Binding Names'].append(unordered_names)
                        function_checks['Bound'][data.name][
                            data.binding_parameter_type]['Param Types'].append(param_types)

                    else:
                        function_checks['Bound'][data.name][
                            data.binding_parameter_type] = {
                                'Non-Binding Names':
                                [collections.Counter([x.name for x in data.param_list[1:]])],
                                'Param Types': [[x.type for x in data.param_list]],
                                'Ret Type': data.return_type.type
                            }
                else:
                    function_checks['Bound'][data.name] = {
                        data.binding_parameter_type:
                        {'Non-Binding Names':
                         [collections.Counter([x.name for x in data.param_list[1:]])],
                         'Param Types': [[x.type for x in data.param_list]],
                         'Ret Type': data.return_type.type}
                    }

                if '!Bound Functions' in self.data_services:
                    self.data_services['!Bound Functions'].append(data)
                else:
                    self.data_services['!Bound Functions'] = [data]

            else:
                if data.name in function_checks['Unbound']:
                    param_names = collections.Counter([x.name for x in data.param_list])
                    if param_names in function_checks['Unbound'][data.name]['Param Names']:
                        raise SchemaError(("{}:{}-> function name and unordered set of parameter "
                                           "names is not unique").format(
                                               type(data).__name__, data.error_id))

                    param_types = [x.type for x in data.param_list]
                    if param_types in function_checks['Unbound'][data.name]['Param Types']:
                        raise SchemaError(("{}:{}-> function name and ordered set of parameter "
                                           "types is not unique").format(
                                               type(data).__name__, data.error_id))

                    if data.return_type.type != function_checks['Unbound'][data.name]['Ret Type']:
                        raise SchemaError(("{}:{}-> Does not have same return type as other "
                                           "functions with same name").format(
                                               type(data).__name__, data.error_id))

                    function_checks['Unbound'][data.name]['Param Names'].append(param_names)
                    function_checks['Unbound'][data.name]['Param Types'].append(param_types)

                else:
                    function_checks['Unbound'][data.name] = {
                        'Param Names': [collections.Counter([x.name for x in data.param_list])],
                        'Param Types': [[x.type for x in data.param_list]],
                        'Ret Type': data.return_type.type
                    }

                if '!UnBound Functions' in self.data_services:
                    self.data_services['!UnBound Functions'].append(data)
                else:
                    self.data_services['!UnBound Functions'] = [data]

        self._get_annotations()


class Property(Element):
    """Class defining an OData Property Element.

    Element edm.Property
    Defines a structural property.
    OData Version 4.0 Part 3 Section 6

    Attributes:
        name: SimpleIdentifier attribute used to reference, serialize or deserialize this property.
              Must be unique within the set of Properties and NavigationProperties in the containing
              structural type.
        type: QualifiedName of a primitive, complex or enumeration type in scope, or a colelction of
              one of these types. Indicates the type of this property.
        nullable: Optional boolean specifying whether a value is required for this property.
        max_length: Optional integer specifying the maximum length the value for this property can
                    be.
        precision: Optional integer specifying the max number of digits in the properties value for
                   a decimal property or the max number of decimal places in the seconds portion of
                   a temporal property.
        scale: Optional integer or 'variable' value specifying the number of digits allowed to the
               right of the decimal point.
        unicode: Optional boolean for string properties specifying that the property is encoded in
                 unicode, false indicates the property is encoded with ASCII.
        srid: Optional value for geometry or geography properties specifying the spatial reference
              system applied.
        default_value: Optional value for a primitive or enumeration property defining the default
                       value of the property if the property is not explicitly represented in an
                       annotation ot the body of a POST or PUT request.
    """

    def __init__(self, data, parent):
        self.name = None
        self.type = None
        self.nullable = True
        self.max_length = None
        self.precision = None
        self.scale = None
        self.unicode = None
        self.srid = None
        self.default_value = None

        super(Property, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        self.type = self.raw_data.attrib['Type']
        _, type_name = is_collection(self.type)
        check_type(type_name, 'QualifiedName')

        if 'Nullable' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['Nullable'], 'Boolean')
            self.nullable = (self.raw_data.attrib['Nullable'] == 'true')

        if 'MaxLength' in self.raw_data.attrib:
            self.max_length = self.raw_data.attrib['MaxLength']
            if self.max_length != 'max':
                try:
                    self.max_length = int(self.max_length)
                except Exception:
                    raise SchemaError("MaxLength facet {} is not valid (int or 'max')".format(
                        self.max_length))

                if self.max_length <= 0:
                    raise SchemaError("MaxLength facet {} is not a positive integer".format(
                        self.max_length))

        if 'Precision' in self.raw_data.attrib:
            try:
                self.precision = int(self.raw_data.attrib['Precision'])
            except Exception:
                raise SchemaError("Precision facet {} is not valid (int)".format(self.precision))

        if 'Scale' in self.raw_data.attrib:
            self.scale = self.raw_data.attrib['Scale']
            if self.scale != 'variable':
                try:
                    self.scale = int(self.raw_data.attrib['Scale'])
                except Exception:
                    raise SchemaError("Scale facet {} is not valid (int or 'variable')".format(
                        self.scale))

                if self.scale < 0:
                    raise SchemaError("Scale facet {} is not a non-negative integer".format(
                        self.scale))

        if self.precision != None:
            if self.scale > self.precision:
                raise SchemaError("Scale facet {} > Precision facet {}".format(self.scale,
                                                                               self.precision))

        if 'Unicode' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['Unicode'], 'Boolean')
            self.unicode = (self.raw_data.attrib['Unicode'] == 'true')

        if 'SRID' in self.raw_data.attrib:
            self.srid = self.raw_data.attrib['SRID']
            if self.srid != 'variable':
                try:
                    self.srid = int(self.srid)
                except Exception:
                    raise SchemaError("SRID facet {} is not valid (int or 'variable')".format(
                        self.srid))

                if self.srid < 0:
                    raise SchemaError("SRID facet {} is not a non-negative integer".format(
                        self.srid))

        if 'DefaultValue' in self.raw_data.attrib:
            self.default_value = self.raw_data.attrib['DefaultValue']

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        _, data = is_collection(self.type)
        type_element = self.find_in_scope(data)

        if not (isinstance(type_element, PrimitiveType) or isinstance(type_element, AbstractType) or
                isinstance(type_element, EnumType) or isinstance(type_element, ComplexType)):
            raise SchemaError("Type must be PrimitiveType, ComplexType, AbstractType or EnumType in scope")

        if isinstance(type_element, PrimitiveType):
            base_prim_type = type_element.get_primitive_type()
            base_prim_type.check_max_length_valid(self.raw_data.attrib)
            base_prim_type.check_precision_valid(self.raw_data.attrib)
            base_prim_type.check_scale_valid(self.raw_data.attrib)
            base_prim_type.check_unicode_valid(self.raw_data.attrib)
            base_prim_type.check_srid_valid(self.raw_data.attrib)

            if self.srid is None:
                self.srid = base_prim_type.get_srid_default()
            if self.scale is None:
                self.scale = base_prim_type.get_scale_default()
            if self.unicode is None:
                self.unicode = base_prim_type.get_unicode_default()

            if self.precision != None:
                base_prim_type.check_precision_value(self.precision)
            if self.default_value != None:
                self.default_value = base_prim_type.convert(self.default_value)

        else:
            if (self.max_length != None or self.precision != None or self.scale != None or
                    self.unicode != None or self.srid != None):
                raise SchemaError("Facet specified for non primitive type")

        #TODO: This is not an odata fault, but more of a redfish one
        #if isinstance(type_element, TypeDefinition):
        #    annotations = self.annotation + type_element.annotation
        #else:
        #    annotations = self.annotation
        #
        #permissions_found = False
        #for annotation in annotations:
        #    if annotation.term.endswith('.Permissions'):
        #        permissions_found = True
        #
        #
        #metadata = self.parent
        #schema = None
        #while not isinstance(metadata, MetaData):
        #    if isinstance(metadata, Schema):
        #        schema = metadata
        #    metadata = metadata.parent
        #
        #         if not permissions_found:
        #             print(
        #                 "{}->{}->{}->{}".format(
        #                     metadata.uri, schema.name, self.parent.name, self.name))

class NavigationProperty(Element):
    """Class defining an OData NavigationProperty element.

    Element edm.NavigationProperty
    Allows navigation to related entities.
    OData Version 4.0 Part 3 Section 7.1

    Attributes:
        ref_constraints: Array of child ReferentialConstraint element.
        name: SimpleIdentifier that can be used when navigating from the structured type declaring
              the navigation property to the related entity type.
        nullable: Optional boolean indicating whether a navaigation target is required.
        type: Points to an entity type or a collectino of an entity typ[e declared in the same
              document or a referenced document. Describes the type of what this navigation property
              points to.
        partner: Optional navigation property path pointing to a parter navigation property pointing
                 back to this element.
        contains_target: Optional boolean indicating whether this is a Containment Navigation
                         Property or not.
        on_delete: Link to the child OnDelete element if defined.
    """

    def __init__(self, data, parent):
        self.ref_constraints = []
        self.name = None
        self.nullable = True
        self.type = None
        self.partner = None
        self.contains_target = False
        self.on_delete = None

        super(NavigationProperty, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        self.type = self._get_required_attrib('Type')
        collection, _ = is_collection(self.type)

        if 'Nullable' in self.raw_data.attrib:
            if collection:
                raise SchemaError("Collection type cannot specify Nullable attribute")
            check_type(self.raw_data.attrib['Nullable'], 'Boolean')
            self.nullable = (self.raw_data.attrib['Nullable'] == 'true')

        if 'Partner' in self.raw_data.attrib:
            if isinstance(self.parent, ComplexType):
                raise SchemaError("Cannot specify Partner when parent is ComplexType")
            self.partner = self.raw_data.attrib['Partner']

        if 'ContainsTarget' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['ContainsTarget'], 'Boolean')
            self.contains_target = (self.raw_data.attrib['ContainsTarget'] == 'true')

        ref_constraints = self._get_elements('ReferentialConstraint')

        if collection and (len(ref_constraints) > 0):
            raise SchemaError("ReferentialConstraint not allowed when type is a collection")

        for ref_constraint in ref_constraints:
            data = ReferentialConstraint(ref_constraint, self)
            self.ref_constraints.append(data)
            self.children.append(data)

        on_delete = self._get_elements('OnDelete')

        if len(on_delete) > 1:
            raise SchemaError("Defined more than one OnDelete element")
        elif len(on_delete) == 1:
            self.on_delete = OnDelete(on_delete[0], self)
            self.children.append(self.on_delete)

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        _, data = is_collection(self.type)
        type_element = self.find_in_scope(data)

        if not (isinstance(type_element, EntityType) or
                (isinstance(type_element, AbstractType) and type_element.name == 'EntityType')):
            raise SchemaError("Type must be EntityType in scope or abstract EntityType")

        if self.partner:
            partner_path = '/'.join([data, self.partner])

            split_path = partner_path.split('/')
            for i in range(1, len(split_path)):
                tmp_path = '/'.join(split_path[:(-1 * i)])
                step_element = self.find_in_scope(tmp_path)
                if isinstance(step_element, NavigationProperty):
                    raise SchemaError(
                        "Cannot traverse NavigationProperty {} while finding partner".format(
                            step_element.name))

            partner = self.find_in_scope(partner_path)

            if not isinstance(partner, NavigationProperty):
                raise SchemaError("Partner {} must point to a NavigationProperty".format(
                    self.partner))

            _, partner_data = is_collection(partner.type)
            partner_target = partner.find_in_scope(partner_data)

            self.parent.check_scope()
            if partner_target not in self.parent.base_type_chain:
                raise SchemaError(
                    "Partner type must be containing entity type or one of its parent entity types")

            if partner.partner:
                if partner.find_in_scope('/'.join([partner_data, partner.partner])) != self:
                    raise SchemaError(
                        "Partner NavigationProperty specifies invalid partner, can only specify"
                        " bi-directional partners or none")

class ReferentialConstraint(Element):
    """Class defining an OData ReferentialConstraint Element.

    Element edm.ReferentialConstraint
    Asserts that the dependent property (containing nav) must have the same value as the principal
    property (target of nav)
    OData Version 4.0 Part 3 Section 7.2

    Attributes:
        property: Specifies the property that takes part in the referential constraint on the
                  dependent entity type.
        referenced_property: Specified the corresponding property of the principal entity type.
    """

    def __init__(self, data, parent):
        self.property = None
        self.referenced_property = None

        super(ReferentialConstraint, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.property = self._get_required_attrib('Property')
        self.referenced_property = self._get_required_attrib('ReferencedProperty')

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        parent_nav_prop = self.parent
        parent_entity = parent_nav_prop.parent

        dependent_property = None
        for prop in parent_entity.children:
            if isinstance(prop, Property):
                if prop.name == self.property:
                    dependent_property = prop

        if not dependent_property:
            raise SchemaError("Dependent property {} not found".format(self.property))

        target = self.find_in_scope(parent_nav_prop.type)

        principal_property = None
        for prop in target.children:
            if isinstance(prop, Property):
                if prop.name == self.referenced_property:
                    principal_property = prop

        if not principal_property:
            raise SchemaError("Principal property {} not found".format(self.referenced_property))

        if principal_property.type != dependent_property.type:
            raise SchemaError(
                "Principal property type {} does not match dependent property type {}".format(
                    principal_property.type, dependent_property.type))

        if parent_nav_prop.nullable or principal_property.nullable:
            if not dependent_property.nullable:
                raise SchemaError(
                    "If parent NavigationProperty or principal property is nullable then dependent"
                    " property nust be nullable")
        else:
            if dependent_property.nullable:
                raise SchemaError(
                    "If parent NavigationProperty and principal property are not nullable then"
                    " dependent property must be marked as not nullable")


class OnDelete(Element):
    """Class defining an OData OnDelete element.

    Element edm.OnDelete
    Descrbes the action the service will take on related entities when the entity on which the
    navigation property is defined is deleted
    OData Version 4.0 Part 3 Section 7.3

    Attributes:
        action: Defines the action the service will take on related entities when the entity on
                which the navigation property is defined is deleted.
    """

    def __init__(self, data, parent):
        self.action = None

        super(OnDelete, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.action = self._get_required_attrib('Action')
        if self.action not in ['Cascade', 'None', 'SetNull', 'SetDefault']:
            raise SchemaError(
                "Action {} is not valid ('Cascade', 'None', 'SetNull', 'SetDefault')".format(
                    self.action))

        self._get_annotations()


class EntityType(Element, StructuredType):
    """Class defining an OData EntityType element.

    Element edm.EntityType
    Represents an entity type in the entity model
    OData Version 4.0 Part 3 Section 8.1

    Attributes:
        name: Unique SimpleIdentifier within its namespace identifying this entity type.
        defined_properties: List of directly defined Property element.
        defined_nav_properties: List of directly defined NavigationProperty elements.
        base_type: Optional QualifiedName pointing to another entity type this type inherits from.
        abstract: Optional boolean indicating whether this is an abstract entity type (no key
                  required) or not.
        open_type: Optional boolean indicating whether this is an open entity type (allows clients
                   to add properties to instances of this type) or not.
        has_stream: Optional boolean indicating whether this type is a media entity or not.
        key: link to defined Key element if applicable.
        base_type_chain: List of all entity types in the base type chain for this element.
        properties: List of all properties applied to this type (defined and inherited).
        nav_properties: List of all navigation properties applied to this type (defined and
                        inherited).
    """

    def __init__(self, data, parent):
        self.name = None
        self.defined_properties = []
        self.defined_nav_properties = []
        self.base_type = None
        self.abstract = None
        self.open_type = False
        self.has_stream = False
        self.key = None
        self.base_type_chain = [self]
        self.properties = []
        self.nav_properties = []

        super(EntityType, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        if 'BaseType' in self.raw_data.attrib:
            self.base_type = self.raw_data.attrib['BaseType']
            check_type(self.base_type, 'QualifiedName')

        if 'Abstract' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['Abstract'], 'Boolean')
            self.abstract = (self.raw_data.attrib['Abstract'] == 'true')

        if 'OpenType' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['OpenType'], 'Boolean')
            self.open_type = (self.raw_data.attrib['OpenType'] == 'true')

        if 'HasStream' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['HasStream'], 'Boolean')
            self.has_stream = (self.raw_data.attrib['HasStream'] == 'true')

        prop_names = []
        for prop in self._get_elements('Property'):
            data = Property(prop, self)
            if data.name in prop_names:
                raise SchemaError("Property name {} not unique".format(data.name))
            if data.name == self.name:
                raise SchemaError("Property cannot have same name as declaring entity type")
            prop_names.append(data.name)
            self.defined_properties.append(data)
            self.children.append(data)

        for nav_prop in self._get_elements('NavigationProperty'):
            data = NavigationProperty(nav_prop, self)
            if data.name in prop_names:
                raise SchemaError("NavigationProperty name {} not unique".format(data.name))
            if data.name == self.name:
                raise SchemaError(
                    "NavigationProperty cannot have same name as declaring entity type")
            prop_names.append(data.name)
            self.defined_nav_properties.append(data)
            self.children.append(data)

        keys = self._get_elements('Key')

        if keys:
            if len(keys) > 1:
                raise SchemaError("more than one Key element")
            self.key = Key(keys[0], self)
            self.children.append(self.key)

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        if self.base_type:
            base_type = self.find_in_scope(self.base_type)

            # Section 4.5
            if isinstance(base_type, AbstractType) and (
                    base_type.name in ['EntityType', 'ComplexType']):
                raise SchemaError(
                    "Abstract {} cannot be used as the base type of an EntityType".format(
                        base_type.name))

        self.base_type_chain = [self]

        current_element = self
        while current_element.base_type:
            new_element = current_element.find_in_scope(current_element.base_type)
            if new_element in self.base_type_chain:
                raise SchemaError("Inheritance cycle introduced with base type {}->{}".format(
                    current_element.name, new_element.name))
            self.base_type_chain.append(new_element)
            current_element = new_element

        key_count = 0
        for element in self.base_type_chain:
            if element.key:
                key_count += 1

        if not self.abstract:
            if key_count != 1:
                raise SchemaError(
                    "Non abstract EntityType needs one key between self and base types,"
                    " has {}".format(key_count))
        else:
            if key_count > 1:
                raise SchemaError(
                    "Cannot have more than one key between self and base types, has {}".format(
                        key_count))

        if self.abstract:
            for element in self.base_type_chain:
                if not element.abstract:
                    raise SchemaError(
                        "Abstract EntityType must not inherit from a non-abstract EntityType")

        for element in self.base_type_chain:
            if element.open_type:
                if not self.open_type:
                    raise SchemaError("EntityType derived from an open entity type must be open")

        self.properties = copy.copy(self.defined_properties)
        self.nav_properties = copy.copy(self.defined_nav_properties)
        prop_names = [x.name for x in self.properties] + [x.name for x in self.nav_properties]
        for element in self.base_type_chain:
            if element != self:
                for prop in element.defined_properties:
                    if prop.name in prop_names:
                        raise SchemaError("BaseType {} property name {} not unique".format(
                            element.name, prop.name))
                    prop_names.append(prop.name)
                    self.properties.append(prop)
                for prop in element.defined_nav_properties:
                    if prop.name in prop_names:
                        raise SchemaError(
                            "BaseType {} navigation property name {} not unique".format(
                                element.name, prop.name))
                    prop_names.append(prop.name)
                    self.nav_properties.append(prop)

class Key(Element):
    """Class defining an OData Key element.

    Element edm.Key
    Identifies an entity within an entity set
    OData Version 4.0 Part 3 Section 8.2

    Attributes:
        property_refs: List of all child PropertyRef elements.
    """

    def __init__(self, data, parent):
        self.property_refs = []

        super(Key, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        prop_refs = self._get_elements('PropertyRef')
        if len(prop_refs) < 1:
            raise SchemaError("Needs at least one PropertyRef")
        for prop_ref in prop_refs:
            data = PropertyRef(prop_ref, self)
            self.property_refs.append(data)
            self.children.append(data)


class PropertyRef(Element):
    """Class defining an OData PropertyRef element.

    Element edm.PropertyRef
    Provides an edm:Key with a reference to a property
    OData Version 4.0 Part 3 Section 8.3

    Attributes:
        name: Path expression resolving to a primitive property of the entity type itself or to a
              primitive property of a complex property of the entity type.
        alias: If path is a member of a complex type this mandatory field presents an alias to the
               path.
    """

    def __init__(self, data, parent):
        self.name = None
        self.alias = None

        super(PropertyRef, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name

        if 'Alias' in self.raw_data.attrib:
            self.alias = self.raw_data.attrib['Alias']
            check_type(self.alias, 'SimpleIdentifier')

            for prop_ref in self.parent.property_refs:
                if prop_ref.alias == self.alias:
                    raise SchemaError("Alias {} is not unique".format(self.alias))
        else:
            self.alias = None

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        entity_type = self.get_first_parent_of_type(EntityType)

        if entity_type is None:
            raise SchemaError("Could not find parent entity type")

        schema = self.get_first_parent_of_type(Schema)
        if schema is None:
            raise SchemaError("Could not find parent schema")

        path = schema.name + '.' + entity_type.name + '/' + self.name

        param = self.find_in_scope(path)

        if not isinstance(param, Property):
            raise SchemaError("Path does not resolve to Property")

        if param.nullable:
            raise SchemaError("Property making up key must be non-nullable")

        _, type_name = is_collection(param.type)

        local_type = self.find_in_scope(type_name)
        prim_type = local_type.get_primitive_type()

        if prim_type.name not in ['Boolean', 'Byte', 'Date', 'DateTimeOffset', 'Decimal', 'Guid',
                                  'Int16', 'Int32', 'Int64', 'SByte', 'String', 'TimeOfDay']:
            raise SchemaError(
                "Property base type does not resolve to proper type (Boolean, Byte, Date, "
                "DateTimeOffset, Decimal, Guid, Int16, Int32, Int64, SByte, String, TimeOfDay)")

        traversed_complex_type = False
        target_entity = entity_type
        for segment in self.name.split('/'):
            found_child = None
            for child in target_entity.children:
                if child.error_id == segment:
                    found_child = child
            if found_child is None:
                raise SchemaError("Error finding segment {}".format(segment))

            target_entity = self.find_in_scope(found_child.type)
            if isinstance(target_entity, ComplexType):
                traversed_complex_type = True

        if traversed_complex_type:
            if not self.alias:
                raise SchemaError(
                    "Alias is required for key property when it is part of complex type")
        else:
            if self.alias:
                raise SchemaError(
                    "Alias must not be specified if property is not part of complex type")


class ComplexType(Element, StructuredType):
    """Class defining an OData ComplexType element.

    Element edm.ComplexType
    Represents a complex type in an entity model.
    OData Version 4.0 Part 3 Section 9.1

    Attributes:
        defined_properties: List of all properties defined by this element.
        defined_nav_properties: List of all navigation properties defined by this element.
        name: Unique SimpleIdentier identifying the complex type within the namespace.
        base_type: Optional QualifiedName indicating the base complex type this type inherits from.
        abstract: Optional boolean indicating that this is abstract (cannot be instantiated) or not.
        open_type: Optional boolean indicating that this is an open type (client can add properties
                   dynamically) or not.
        properties: List of all properties applied to this element (defined or inherited).
        nav_properties: List of all navigation properies applied to this element (defined or
                        inherited).
    """

    def __init__(self, data, parent):
        self.defined_properties = []
        self.defined_nav_properties = []
        self.name = None
        self.base_type = None
        self.abstract = False
        self.open_type = False
        self.properties = []
        self.nav_properties = []

        super(ComplexType, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        if 'BaseType' in self.raw_data.attrib:
            self.base_type = self.raw_data.attrib['BaseType']
            check_type(self.base_type, 'QualifiedName')

        if 'Abstract' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['Abstract'], 'Boolean')
            self.abstract = (self.raw_data.attrib['Abstract'] == 'true')

        if 'OpenType' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['OpenType'], 'Boolean')
            self.open_type = (self.raw_data.attrib['OpenType'] == 'true')

        for prop in self._get_elements('Property'):
            data = Property(prop, self)
            self.defined_properties.append(data)
            self.children.append(data)

        for nav_prop in self._get_elements('NavigationProperty'):
            data = NavigationProperty(nav_prop, self)
            self.defined_nav_properties.append(data)
            self.children.append(data)

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        if self.base_type:
            base_type = self.find_in_scope(self.base_type)

            # Section 4.5
            if isinstance(base_type, AbstractType) and (
                    base_type.name in ['EntityType', 'ComplexType']):
                raise SchemaError(
                    "Abstract {} cannot be used as the base type of a ComplexType".format(
                        base_type.name))

            if not isinstance(base_type, ComplexType):
                raise SchemaError("BaseType must be a ComplexType")

            #Test for an inheritance loop
            found_types = [base_type]
            tmp_type = base_type
            while tmp_type.base_type:
                tmp_type.find_in_scope(tmp_type.base_type)
                if tmp_type in found_types:
                    raise SchemaError("BaseType introduces inheritance loop at {}".format(
                        tmp_type.error_id))
                found_types.append(tmp_type)

            for base_type in found_types:
                if base_type.open_type and not self.open_type:
                    raise SchemaError("ComplexType derived from open type must be an open type")

            self.properties = copy.copy(self.defined_properties)
            self.nav_properties = copy.copy(self.defined_nav_properties)
            prop_names = [x.name for x in self.properties] + [x.name for x in self.nav_properties]
            for element in found_types:
                if element != self:
                    for prop in element.defined_properties:
                        if prop.name in prop_names:
                            raise SchemaError("BaseType {} property name {} not unique".format(
                                element.name, prop.name))
                        prop_names.append(prop.name)
                        self.properties.append(prop)
                    for prop in element.defined_nav_properties:
                        if prop.name in prop_names:
                            raise SchemaError(
                                "BaseType {} navigation property name {} not unique".format(
                                    element.name, prop.name))
                        prop_names.append(prop.name)
                        self.nav_properties.append(prop)
        else:

            self.properties = copy.copy(self.defined_properties)
            self.nav_properties = copy.copy(self.defined_nav_properties)

class EnumType(Element, PrimitiveType):
    """Class defining an OData EnumType element.

    Element edm.EnumType
    Represents an enumeration type in an entity model
    OData Version 4.0 Part 3 Section 10.1

    Attributes:
        members: List of all child Member element.
        name: Unique SimpleIdentifier within its namespace identifying the element.
        underlying_type: Optional primitive type indicating the underlying type of this element.
        is_flags: Optional boolean indicating that this type allows multiple members to be selected
                  simultaneously.
        needs_value: Boolean indicating whether all child members need values specified or not.
    """

    def __init__(self, data, parent):
        self.members = []
        self.name = None
        self.underlying_type = 'Edm.Int32'
        self.is_flags = False
        self.needs_value = False

        super(EnumType, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        if 'UnderlyingType' in self.raw_data.attrib:
            self.underlying_type = self.raw_data.attrib['UnderlyingType']

        if self.underlying_type not in ['Edm.Byte', 'Edm.SByte', 'Edm.Int16', 'Edm.Int32',
                                        'Edm.Int64']:
            raise SchemaError("Underlying type {} not valid".format(self.underlying_type))

        if 'IsFlags' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['IsFlags'], 'Boolean')
            self.is_flags = (self.raw_data.attrib['IsFlags'] == 'true')

        if self.is_flags:
            self.needs_value = True
        else:
            self.needs_value = ('Value' in self.raw_data[0].attrib)

        for member in self._get_elements('Member'):
            data = Member(member, self)
            self.members.append(data)
            self.children.append(data)

        self._get_annotations()

    def convert(self, data):
        self.check_scope()

        values = data.split(',')
        if not self.is_flags and (len(values) > 1):
            raise SchemaError("Can only specify multiple values when IsFlags is set ({})".format(
                data))

        value = 0
        for field in values:
            field_found = False
            for member in self.members:
                if field == member.name:
                    field_found = True
                    value |= member.value
            if not field_found:
                raise SchemaError("Value {} invalid for EnumType".format(field))

        underlying_type = self.find_in_scope(self.underlying_type).get_primitive_type()
        return underlying_type.convert(value)

    def get_primitive_type(self):
        """Gets the primitive type of this element.

        Defined differently for each class, since this is not a primitive type call
        get_primitive_type on its underlying type

        Returns:
            A class instance which is the primitive type of the element
        """

        underlying_type = self.find_in_scope(self.underlying_type)
        return underlying_type.get_primitive_type()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        underlying_type = self.find_in_scope(self.underlying_type)
        if not isinstance(underlying_type, PrimitiveType):
            raise SchemaError("UnderlyingType must be a PrimitiveType")

        index = 0
        for member in self.members:
            if self.needs_value:
                member.check_needs_value()
                member.value = underlying_type.convert(member.value)
            else:
                member.value = underlying_type.convert(index)
                index += 1

class Member(Element):
    """Class defining an OData Member element.

    Element edm.Member
    Defines the discrete options for the enumeration type
    OData Version 4.0 Part 3 Section 10.2

    Attributes:
        name: SimpleIdentifier identifying the member, unique within an enumerated type.
        value: Optional valid value for the underlying type of the enumerated type to be used for
               this member.
    """

    def __init__(self, data, parent):
        self.name = None
        self.value = None

        super(Member, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        self.value = self.raw_data.attrib.get('Value', None)

        self._get_annotations()

    def check_needs_value(self):
        """Check that this member has a value attribute.

        This is a class method instead of a higher level check so the error string could correctly
        point to this member.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If member does not have a value.
        """

        if self.value is None:
            raise SchemaError("{}:{}->Has no value where needed".format(
                type(self).__name__, self.error_id))


class TypeDefinition(Element, PrimitiveType):
    """Class defining an OData TypeDefinition element.

    Element edm.TypeDefinition
    Defines a specialization of one of the primitive types
    OData Version 4.0 Part 3 Section 11.1

    Attributes:
        name: SimpleIdentifier identifying the element, unique within its namespace.
        underlying_type: QualifiedName of a primitive type to be used as the underlying type for
                         this element.
        max_length: Optional integer specifying the maximum length the value for this element can
                    be.
        precision: Optional integer specifying the max number of digits in the element's value for
                   a decimal element or the max number of decimal places in the seconds portion of
                   a temporal element.
        scale: Optional integer or 'variable' value specifying the number of digits allowed to the
               right of the decimal point.
        unicode: Optional boolean for string elements specifying that the property is encoded in
                 unicode, false indicates the element is encoded with ASCII.
        srid: Optional value for geometry or geography element specifying the spatial reference
              system applied.
    """

    def __init__(self, data, parent):
        self.name = None
        self.underlying_type = None
        self.max_length = None
        self.precision = None
        self.scale = None
        self.unicode = None
        self.srid = None

        super(TypeDefinition, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        self.underlying_type = self._get_required_attrib('UnderlyingType')

        if 'MaxLength' in self.raw_data.attrib:
            self.max_length = self.raw_data.attrib['MaxLength']
            if self.max_length != 'max':
                try:
                    self.max_length = int(self.max_length)
                except Exception:
                    raise SchemaError("MaxLength facet {} is not valid (int or 'max')".format(
                        self.max_length))

                if self.max_length <= 0:
                    raise SchemaError("MaxLength facet {} is not a positive integer".format(
                        self.max_length))

        if 'Precision' in self.raw_data.attrib:
            try:
                self.precision = int(self.raw_data.attrib['Precision'])
            except Exception:
                raise SchemaError("Precision facet {} is not valid (int)".format(self.precision))

        if 'Scale' in self.raw_data.attrib:
            self.scale = self.raw_data.attrib['Scale']
            if self.scale != 'variable':
                try:
                    self.scale = int(self.raw_data.attrib['Scale'])
                except Exception:
                    raise SchemaError("Scale facet {} is not valid (int or 'variable')".format(
                        self.scale))

                if self.scale < 0:
                    raise SchemaError("Scale facet {} is not a non-negative integer".format(
                        self.scale))

        if (self.precision != None) and (self.scale != None):
            if self.scale > self.precision:
                raise SchemaError("Scale facet {} > Precision facet {}".format(self.scale,
                                                                               self.precision))

        if 'Unicode' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['Unicode'], 'Boolean')
            self.unicode = (self.raw_data.attrib['Unicode'] == 'true')

        if 'SRID' in self.raw_data.attrib:
            self.srid = self.raw_data.attrib['SRID']
            if self.srid != 'variable':
                try:
                    self.srid = int(self.srid)
                except Exception:
                    raise SchemaError("SRID facet {} is not valid (int or 'variable')".format(
                        self.srid))

                if self.srid < 0:
                    raise SchemaError("SRID facet {} is not a non-negative integer".format(
                        self.srid))

        self._get_annotations()

    def get_primitive_type(self):
        """Gets the primitive type of this element.

        Defined differently for each class, since this is not a primitive type call
        get_primitive_type on its underlying type

        Returns:
            A class instance which is the primitive type of the element
        """

        underlying_type = self.find_in_scope(self.underlying_type)
        return underlying_type.get_primitive_type()

    def validate_type(self, element):
        """Validates this TypeDefinition against an element where it is applied.

        Ensures that facets and annotations are not applied to both an element and its underlying
        TypeDefinition.

        Args:
            element: OData element that this TypeDefinition is applied to.

        Returns:
            None

        Raises:
            SchemaError: If there is an error applying a facet or annotation.
        """

        self.check_scope()

        if 'MaxLength' in self.raw_data.attrib:
            if 'MaxLength' in element.raw_data.attrib:
                raise SchemaError("Facet MaxLength already specified in TypeDefinition")

        if 'Unicode' in self.raw_data.attrib:
            if 'Unicode' in element.raw_data.attrib:
                raise SchemaError("Facet Unicode already specified in TypeDefinition")

        if 'Precision' in self.raw_data.attrib:
            if 'Precicion' in element.raw_data.attrib:
                raise SchemaError("Facet Precision already specified in TypeDefinition")

        if 'Scale' in self.raw_data.attrib:
            if 'Scale' in element.raw_data.attrib:
                raise SchemaError("Facet Scale already specified in TypeDefinition")

        if 'SRID' in self.raw_data.attrib:
            if 'SRID' in element.raw_data.attrib:
                raise SchemaError("Facet SRID already specified in TypeDefinition")

        terms = []
        for annotation in self.annotation:
            terms.append(annotation.term)

#        for annotation in element.annotation:
#            if annotation.term in terms:
#                raise SchemaError("Term {} already applied to TypeDefinition".format(
#                    annotation.term))

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        underlying_type = self.find_in_scope(self.underlying_type)
        if type(underlying_type) != PrimitiveType:
            raise SchemaError("UnderlyingType is not a PrimitiveType")

        base_prim_type = underlying_type.get_primitive_type()
        base_prim_type.check_max_length_valid(self.raw_data.attrib)
        base_prim_type.check_precision_valid(self.raw_data.attrib)
        base_prim_type.check_scale_valid(self.raw_data.attrib)
        base_prim_type.check_unicode_valid(self.raw_data.attrib)
        base_prim_type.check_srid_valid(self.raw_data.attrib)

        if self.srid is None:
            self.srid = base_prim_type.get_srid_default()
        if self.scale is None:
            self.scale = base_prim_type.get_scale_default()
        if self.unicode is None:
            self.unicode = base_prim_type.get_unicode_default()

        if self.precision != None:
            base_prim_type.check_precision_value(self.precision)

class Action(Element):
    """Class defining an OData Action element.

    Element edm.Action
    Represents an action in an entity model
    OData Version 4.0 Part 3 Section 12.1

    Attributes:
        name: SimpleIdentifier used to identify this element.
        is_bound: Optional boolean indicating that this element is bound (invoked by apppending a
                  segment containing the qualified action name to a segment of the appropriate
                  binding parameter type within the resource path) or not (invoked through an action
                  import).
        entity_set_path: Optional value indicating the determination of the entity set for the
                         return type is contingent on the binding parameter.
        params: Dictionary of all children parameter elements applied to this action.
        binding_parameter_type: Link to this action's binding parameter type if applicable.
        return_type: Link to this action's return type if applicable.
    """

    def __init__(self, data, parent):
        self.name = None
        self.is_bound = False
        self.entity_set_path = None
        self.params = {}
        self.binding_parameter_type = None
        self.return_type = None

        super(Action, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        if 'IsBound' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['IsBound'], 'Boolean')
            self.is_bound = (self.raw_data.attrib['IsBound'] == 'true')

        if 'EntitySetPath' in self.raw_data.attrib:
            self.entity_set_path = self.raw_data.attrib['EntitySetPath']
            #TODO: nav segments and qualified names

        params = self._get_elements('Parameter')
        if self.is_bound and (len(params) < 1):
            raise SchemaError("Bound action does not specify at least one parameter")

        for param in params:
            parsed_param = Parameter(param, self)
            self.children.append(parsed_param)
            if self.is_bound and (param == params[0]):
                self.binding_parameter_type = parsed_param.type
            if parsed_param.name in self.params:
                raise SchemaError("Parameter name {} is not unique".format(parsed_param.name))
            self.params[parsed_param.name] = parsed_param

        return_type = self._get_elements('ReturnType')
        if len(return_type) == 0:
            self.return_type = None
        elif len(return_type) == 1:
            self.return_type = ReturnType(return_type[0], self)
            self.children.append(self.return_type)
        else:
            raise SchemaError("Action specifies more than one ReturnType")

        self._get_annotations()


class Function(Element):
    """Class defining an OData Function element.

    Element edm.Function
    Represents a function in an entity model
    OData Version 4.0 Part 3 Section 12.2

    Attributes:
        name: SimpleIdentifier identifying this element.
        is_bound: Optional boolean indicating whether this element is bound (invoked by appending a
                  segment containing the qualified function name to a segment of the appropriate
                  binding parameter type) or not (invoked as static functions through a function
                  import).
        is_composable: Optional boolean indicating whether this element is composable (can be
                       invoked with additional path segments) or not.
        params: Dictionary of all child parameter elements applied to this function.
        param_list: List of all children parameter elements.
        return_type: Link to the return type of this function if applicable.
        binding_parameter_type: Link to the binding parameter type for this function if applicable.
    """

    def __init__(self, data, parent):
        self.name = None
        self.is_bound = False
        self.is_composable = False
        self.params = {}
        self.param_list = []
        self.return_type = None
        self.binding_parameter_type = None

        super(Function, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        if 'IsBound' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['IsBound'], 'Boolean')
            self.is_bound = (self.raw_data.attrib['IsBound'] == 'true')

        if 'IsComposable' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['IsComposable'], 'Boolean')
            self.is_composable = (self.raw_data.attrib['IsComposable'] == 'true')

        #TODO: entity set path

        params = self._get_elements('Parameter')
        if self.is_bound and (len(params) < 1):
            raise SchemaError("Bound action does not specify at least one parameter")

        for param in params:
            parsed_param = Parameter(param, self)
            self.children.append(parsed_param)
            if self.is_bound and (param == params[0]):
                self.binding_parameter_type = parsed_param.type
            if parsed_param.name in self.params:
                raise SchemaError("Parameter name {} is not unique".format(parsed_param.name))
            self.params[parsed_param.name] = parsed_param
            self.param_list.append(parsed_param)

        return_type = self._get_elements('ReturnType')
        if len(return_type) != 1:
            raise SchemaError("Function does not specify one ReturnType")

        self.return_type = ReturnType(return_type[0], self)
        self.children.append(self.return_type)

        self._get_annotations()


class ReturnType(Element):
    """Class defining an OData ReturnType element.

    Element edm.ReturnType
    Represents a return type expected from an action or function
    OData Version 4.0 Part 3 Section 12.3

    Attributes:
        type: Specifies the type of the result returned by the function or action.
        nullable: Optional boolean indicating whether the function or action may return null or not.
        max_length: Optional integer specifying the maximum length the value for this element can
                    be.
        precision: Optional integer specifying the max number of digits in the element's value for
                   a decimal element or the max number of decimal places in the seconds portion of
                   a temporal element.
        scale: Optional integer or 'variable' value specifying the number of digits allowed to the
               right of the decimal point.
        srid: Optional value for geometry or geography element specifying the spatial reference
              system applied.
    """

    def __init__(self, data, parent):
        self.type = None
        self.nullable = True
        self.max_length = None
        self.precision = None
        self.scale = None
        self.srid = None

        super(ReturnType, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.type = self._get_required_attrib('Type')
        self.error_id = self.type
        check_type(self.type, 'TypeName')

        if 'Nullable' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['Nullable'], 'Boolean')
            self.nullable = (self.raw_data.attrib['Nullable'] == 'true')

        if 'MaxLength' in self.raw_data.attrib:
            self.max_length = self.raw_data.attrib['MaxLength']
            if self.max_length != 'max':
                try:
                    self.max_length = int(self.max_length)
                except Exception:
                    raise SchemaError("MaxLength facet {} is not valid (int or 'max')".format(
                        self.max_length))

                if self.max_length <= 0:
                    raise SchemaError("MaxLength facet {} is not a positive integer".format(
                        self.max_length))

        if 'Precision' in self.raw_data.attrib:
            try:
                self.precision = int(self.raw_data.attrib['Precision'])
            except Exception:
                raise SchemaError("Precision facet {} is not valid (int)".format(self.precision))

        if 'Scale' in self.raw_data.attrib:
            self.scale = self.raw_data.attrib['Scale']
            if self.scale != 'variable':
                try:
                    self.scale = int(self.raw_data.attrib['Scale'])
                except Exception:
                    raise SchemaError("Scale facet {} is not valid (int or 'variable')".format(
                        self.scale))

                if self.scale < 0:
                    raise SchemaError("Scale facet {} is not a non-negative integer".format(
                        self.scale))

        if (self.precision != None) and (self.scale != None):
            if self.scale > self.precision:
                raise SchemaError("Scale facet {} > Precision facet {}".format(self.scale,
                                                                               self.precision))

        if 'SRID' in self.raw_data.attrib:
            self.srid = self.raw_data.attrib['SRID']
            if self.srid != 'variable':
                try:
                    self.srid = int(self.srid)
                except Exception:
                    raise SchemaError("SRID facet {} is not valid (int or 'variable')".format(
                        self.srid))

                if self.srid < 0:
                    raise SchemaError("SRID facet {} is not a non-negative integer".format(
                        self.srid))

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        _, data = is_collection(self.type)
        type_element = self.find_in_scope(data)

        if not (isinstance(type_element, PrimitiveType) or isinstance(type_element, AbstractType)
                or isinstance(type_element, TypeDefinition) or
                isinstance(type_element, ComplexType) or isinstance(type_element, EnumType) or
                isinstance(type_element, EntityType)):
            raise SchemaError(
                "TypeName {} is not PrimitiveType, AbstractType, TypeDefinition, ComplexType, "
                "EnumType or EntityType".format(self.type))

        if isinstance(type_element, PrimitiveType):
            prim_type = type_element.get_primitive_type()
            prim_type.check_max_length_valid(self.raw_data.attrib)
            prim_type.check_precision_valid(self.raw_data.attrib)
            prim_type.check_scale_valid(self.raw_data.attrib)
            prim_type.check_srid_valid(self.raw_data.attrib)

            if self.srid is None:
                self.srid = prim_type.get_srid_default()
            if self.scale is None:
                self.scale = prim_type.get_scale_default()

            if self.precision != None:
                prim_type.check_precision_value(self.precision)

        else:
            if (self.max_length != None) or (self.precision != None) or (
                    self.scale != None) or (self.srid != None):
                raise SchemaError("Not PrimitiveType and facets are specified")


class Parameter(Element):
    """Class defining an OData Parameter element.

    Element edm.Parameter
    Allows one or more parameters to be passed to a function or action
    OData Version 4.0 Part 3 Section 12.4

    Attributes:
        name: SimpleIdentifier identifying this element, must be unique within its parent element.
        type: TypeName indicating the type of the value that can be passed to this element.
        nullable: Optional boolean indicating whether this element accepts a null value.
        max_length: Optional integer specifying the maximum length the value for this element can
                    be.
        precision: Optional integer specifying the max number of digits in the element's value for
                   a decimal element or the max number of decimal places in the seconds portion of
                   a temporal element.
        scale: Optional integer or 'variable' value specifying the number of digits allowed to the
               right of the decimal point.
        srid: Optional value for geometry or geography element specifying the spatial reference
              system applied.
    """

    def __init__(self, data, parent):
        self.name = None
        self.type = None
        self.nullable = True
        self.max_length = None
        self.precision = None
        self.scale = None
        self.srid = None

        super(Parameter, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        self.type = self._get_required_attrib('Type')
        check_type(self.type, 'TypeName')

        collection, _ = is_collection(self.type)

        if 'Nullable' in self.raw_data.attrib:
            if collection:
                raise SchemaError("Collection type does not allow nullable attruibute")
            check_type(self.raw_data.attrib['Nullable'], 'Boolean')
            self.nullable = (self.raw_data.attrib['Nullable'] == 'true')

        if 'MaxLength' in self.raw_data.attrib:
            self.max_length = self.raw_data.attrib['MaxLength']
            if self.max_length != 'max':
                try:
                    self.max_length = int(self.max_length)
                except Exception:
                    raise SchemaError("MaxLength facet {} is not valid (int or 'max')".format(
                        self.max_length))

                if self.max_length <= 0:
                    raise SchemaError("MaxLength facet {} is not a positive integer".format(
                        self.max_length))

        if 'Precision' in self.raw_data.attrib:
            try:
                self.precision = int(self.raw_data.attrib['Precision'])
            except Exception:
                raise SchemaError("Precision facet {} is not valid (int)".format(self.precision))

        if 'Scale' in self.raw_data.attrib:
            self.scale = self.raw_data.attrib['Scale']
            if self.scale != 'variable':
                try:
                    self.scale = int(self.raw_data.attrib['Scale'])
                except Exception:
                    raise SchemaError("Scale facet {} is not valid (int or 'variable')".format(
                        self.scale))

                if self.scale < 0:
                    raise SchemaError("Scale facet {} is not a non-negative integer".format(
                        self.scale))

        if (self.precision != None) and (self.scale != None):
            if self.scale > self.precision:
                raise SchemaError("Scale facet {} > Precision facet {}".format(self.scale,
                                                                               self.precision))

        if 'SRID' in self.raw_data.attrib:
            self.srid = self.raw_data.attrib['SRID']
            if self.srid != 'variable':
                try:
                    self.srid = int(self.srid)
                except Exception:
                    raise SchemaError("SRID facet {} is not valid (int or 'variable')".format(
                        self.srid))

                if self.srid < 0:
                    raise SchemaError("SRID facet {} is not a non-negative integer".format(
                        self.srid))

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        _, data = is_collection(self.type)
        type_element = self.find_in_scope(data)

        if not (isinstance(type_element, PrimitiveType) or isinstance(type_element, AbstractType)
                or isinstance(type_element, TypeDefinition) or
                isinstance(type_element, ComplexType) or isinstance(type_element, EnumType) or
                isinstance(type_element, EntityType)):
            raise SchemaError(
                "TypeName {} is not PrimitiveType, AbstractType, TypeDefinition, ComplexType, "
                "EnumType or EntityType".format(self.type))

        if isinstance(type_element, PrimitiveType):
            prim_type = type_element.get_primitive_type()
            prim_type.check_max_length_valid(self.raw_data.attrib)
            prim_type.check_precision_valid(self.raw_data.attrib)
            prim_type.check_scale_valid(self.raw_data.attrib)
            prim_type.check_srid_valid(self.raw_data.attrib)

            if self.srid is None:
                self.srid = prim_type.get_srid_default()
            if self.scale is None:
                self.scale = prim_type.get_scale_default()

            if self.precision != None:
                prim_type.check_precision_value(self.precision)

        else:
            if (self.max_length != None) or (self.precision != None) or (
                    self.scale != None) or (self.srid != None):
                raise SchemaError("Not PrimitiveType and facets are specified")


class EntityContainer(Element):
    """Class defining an OData EntityContainer element.

    Element edm.EntityContainer
    Corresponds to a virtual or physical data store
    OData Version 4.0 Part 3 Section 13.1

    Attributes:
        name: Unique SimpleIdentifier used to identify this element.
        data_services: Dictionary of all data services exposed in this element.
        extends: Optional QualifiedName of an entity conatiner extended by this element.
    """

    def __init__(self, data, parent):
        self.name = None
        self.data_services = {}
        self.extends = None

        super(EntityContainer, self).__init__(data, parent)

    #TODO: Make sure each odata service exposes one of these, put in redfish section

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        for entity_set in self._get_elements('EntitySet'):
            data = EntitySet(entity_set, self)
            if data.name in self.data_services:
                raise SchemaError("{}:{}->Name {} is already defined in EntityContainer".format(
                    type(data).__name__, data.error_id, data.name))
            self.data_services[data.name] = data
            self.children.append(data)

        for singleton in self._get_elements('Singleton'):
            data = Singleton(singleton, self)
            if data.name in self.data_services:
                raise SchemaError("{}:{}->Name {} is already defined in EntityContainer".format(
                    type(data).__name__, data.error_id, data.name))
            self.data_services[data.name] = data
            self.children.append(data)

        for action_import in self._get_elements('ActionImport'):
            data = ActionImport(action_import, self)
            if data.name in self.data_services:
                raise SchemaError("{}:{}->Name {} is already defined in EntityContainer".format(
                    type(data).__name__, data.error_id, data.name))
            self.data_services[data.name] = data
            self.children.append(data)

        for function_import in self._get_elements('FunctionImport'):
            data = FunctionImport(function_import, self)
            if data.name in self.data_services:
                raise SchemaError("{}:{}->Name {} is already defined in EntityContainer".format(
                    type(data).__name__, data.error_id, data.name))
            self.data_services[data.name] = data
            self.children.append(data)

        if 'Extends' in self.raw_data.attrib:
            self.extends = self.raw_data.attrib['Extends']
            check_type(self.extends, 'QualifiedName')

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        if self.extends:
            extends = self.find_in_scope(self.extends)
            if not isinstance(extends, EntityContainer):
                raise SchemaError("Does not extend an EntityContainer")


class EntitySet(Element):
    """Class defining an OData EntitySet element.

    Element edm.EntitySet
    Represents an entity set in an entity model
    OData Version 4.0 Part 3 Section 13.2

    Attributes:
        name: SimpleIdentifier used to indentify this element.
        entity_type: QualifiedName of an entity type in scope. This element must only contain
                     instances of the entity type specified or its subtypes.
        include_in_service_document: Optional boolean indicating whether is is advertised in the
                                     service document or not.
        nav_prop_bindings: List of all child navigation property bindings.
    """

    def __init__(self, data, parent):
        self.name = None
        self.entity_type = None
        self.include_in_service_document = True
        self.nav_prop_bindings = []

        super(EntitySet, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        self.entity_type = self._get_required_attrib('EntityType')
        check_type(self.entity_type, 'QualifiedName')

        if 'IncludeInServiceDocument' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['IncludeInServiceDocument'], 'Boolean')
            self.include_in_service_document = (
                self.raw_data.attrib['IncludeInServiceDocument'] == 'true')

        for nav_prop_binding in self._get_elements('NavigationPropertyBinding'):
            data = NavigationPropertyBinding(nav_prop_binding, self)
            self.nav_prop_bindings.append(data)
            self.children.append(data)

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        entity_type = self.find_in_scope(self.entity_type)
        if not isinstance(entity_type, EntityType):
            raise SchemaError("attribute EntityType does not resolve to an EntityType element")

        # Section 4.5
        if isinstance(type, AbstractType) and type.name == 'EntityType':
            raise SchemaError("Abstract EntityType cannot be used as the type of an EntitySet")


class Singleton(Element):
    """Class defining an OData Singleton element.

    Element edm.Singleton
    Represents a single entity in an entity model
    OData Version 4.0 Part 3 Section 13.3

    Attributes:
        name: SimpleIdentifier used to identify this element.
        type: QualifiedName of an entity type in scope. This element must reference an instance of
              the entity type.
        nav_prop_bindings: List of all child navigation property bindings.
    """

    def __init__(self, data, parent):
        self.name = None
        self.type = None
        self.nav_prop_bindings = []

        super(Singleton, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        self.type = self._get_required_attrib('Type')
        check_type(self.type, 'QualifiedName')
        #TODO: must reference an instance of the entity type specified by the type attrib

        for nav_prop_binding in self._get_elements('NavigationPropertyBinding'):
            data = NavigationPropertyBinding(nav_prop_binding, self)
            self.nav_prop_bindings.append(data)
            self.children.append(data)

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        my_type = self.find_in_scope(self.type)
        if not isinstance(my_type, Type):
            raise SchemaError("Type attribute does not resolve to a Type")

        # Section 4.5
        if isinstance(my_type, AbstractType) and my_type.name == 'EntityType':
            raise SchemaError("Abstract EntityType cannot be used as the type of a Singleton")


class NavigationPropertyBinding(Element):
    """Class defining an OData NavigaionPropertyBinding element.

    Element edm.NavigationPropertyBinding
    Should provide one of these for each NavigationProperty of its entity type
    OData Version 4.0 Part 3 Section 13.4

    Attribures:
        path: QualifiedName describing the path leading to the navigation property.
        target: SimpleIdentifier that names the entity set that contains the related instances
                targeted by the navigation property indicated in the path.
    """

    def __init__(self, data, parent):
        self.path = None
        self.target = None

        super(NavigationPropertyBinding, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.path = self._get_required_attrib('Path')
        self.error_id = self.path
        #TODO: this
        #         check_type(self.path, 'QualifiedName')
        #TODO: this cant end with a constinment property (see 7.1.5)

        self.target = self._get_required_attrib('Target')

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        if isinstance(self.parent, Singleton):
            entity_type = self.parent.type
        elif isinstance(self.parent, EntitySet):
            entity_type = self.parent.entity_type
        else:
            raise SchemaError("Parent should be EntitySet or Singleton")

        nav_prop = self.find_in_scope(entity_type + '/' + self.path)

        if not isinstance(nav_prop, NavigationProperty):
            raise SchemaError("Must resolve to a NavigationProperty")

        try:
            check_type(self.target, 'SimpleIdentifier')

            my_container = self.get_first_parent_of_type(EntityContainer)
            if my_container is None:
                raise SchemaError("Could not find parent EntityContainer")

            target = None
            for child in my_container.children:
                if child.error_id == self.target:
                    target = child

            if not target or not (isinstance(target, EntitySet) or isinstance(target, Singleton)):
                raise SchemaError(
                    "Target must resolve to an EntitySet or Singleton in the same EntityContainer")
        except Exception:
            try:
                check_type(self.target, 'TargetPath')
                target = self.find_in_scope(self.target)

                if not isinstance(target, EntitySet) and not isinstance(target, Singleton):
                    raise SchemaError("Target must resolve to an EntitySet or Singleton in scope")
            except Exception:
                raise SchemaError("Target {} is neither SimpleIdentifier or TargetPath".format(
                    self.target))


class ActionImport(Element):
    """Class defining an OData ActionImport element.

    Element edm.ActionImport
    Allows exposing an unbound action as a top-level element in an entity container
    OData Version 4.0 Part 3 Section 13.5

    Attributes:
        name: SimpleIdentifier identifying the element.
        action: QualifiedName resolving to an unbound Action element in scope.
        entity_set: Optional SimpleIdentifier naming the entity set to which the returned entities
                    belong.
    """

    def __init__(self, data, parent):
        self.name = None
        self.action = None
        self.entity_set = None

        super(ActionImport, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        self.action = self._get_required_attrib('Action')
        check_type(self.action, 'QualifiedName')

        if 'EntitySet' in self.raw_data.attrib:
            self.entity_set = self.raw_data.attrib['EntitySet']

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        action = self.find_in_scope(self.action)
        if not isinstance(action, Action):
            raise SchemaError("Action attribute does not resolve to an Action element")
        if action.is_bound:
            raise SchemaError("Action must not be bound")

        if action.return_type:
            _, type_name = is_collection(action.return_type.type)
            ret_type = action.find_in_scope(type_name)

            if isinstance(ret_type, EntityType):
                if self.entity_set:
                    entity_set = self.find_in_scope(self.entity_set)
                    if not isinstance(entity_set, EntitySet):
                        raise SchemaError("EntitySet attribute must resolve to an EntitySet")

                    try:
                        check_type(self.entity_type, 'SimpleIdentifier')
                        my_entity_container = self.get_first_parent_of_type(EntityContainer)
                        if my_entity_container is None:
                            raise SchemaError("Could not find parent EntityContainer")

                        target_entity_container = entity_set.get_first_parent_of_type(
                            EntityContainer)
                        if target_entity_container is None:
                            raise SchemaError("Could not find target parent EntityContainer")

                        if my_entity_container != target_entity_container:
                            raise SchemaError(
                                "When EntitySet is a SimpleIdentifier, must resolve to an entity "
                                "set in the same entity container")

                    except Exception:
                        try:
                            check_type(self.entity_type, 'TargetPath')
                        except Exception:
                            raise SchemaError(
                                "EntitySet attribute must be SimpleIdentifier or TargetPath")
            else:
                if self.entity_set:
                    raise SchemaError(
                        "EntitySet attribute only allowed when return type is entity or collection "
                        "of entities")


class FunctionImport(Element):
    """Class defining an OData FunctionImport element.

    Element edm.FunctionImport
    allows exposing an unbound function as a top-level entity in an entity container
    OData Version 4.0 Part 3 Section 13.6

    Attributes:
        name: SimpleIdentifier used to identify this element.
        function: QualifiedName that resolves to the name of an unbound Function element in scope.
        entity_set: Optional SimpleIdentifier naming the entity set to which the returned entities
                    belong.
        include_in_service_document: Optional boolean indicating whether this element is advertised
                                     in the service document.
    """

    def __init__(self, data, parent):
        self.name = None
        self.function = None
        self.entity_set = None
        self.include_in_service_document = False

        super(FunctionImport, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        self.function = self._get_required_attrib('Function')
        check_type(self.function, 'QualifiedName')

        if 'EntitySet' in self.raw_data.attrib:
            self.entity_set = self.raw_data.attrib['EntitySet']

        if 'IncludeInServiceDocument' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['IncludeInServiceDocument'], 'Boolean')
            self.include_in_service_document = (
                self.raw_data.attrib['IncludeInServiceDocument'] == 'true')

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        function = self.find_in_scope(self.function)
        if not isinstance(function, Function):
            raise SchemaError("Function attribute does not resolve to a Function element")
        if function.is_bound:
            raise SchemaError("Function must not be bound")

        _, type_name = is_collection(function.return_type.type)
        ret_type = function.find_in_scope(type_name)

        if isinstance(ret_type, EntityType):
            if self.entity_set:
                entity_set = self.find_in_scope(self.entity_set)
                if not isinstance(entity_set, EntitySet):
                    raise SchemaError("EntitySet attribute must resolve to an EntitySet")

                try:
                    check_type(self.entity_type, 'SimpleIdentifier')
                    my_entity_container = self.get_first_parent_of_type(EntityContainer)
                    if my_entity_container is None:
                        raise SchemaError("Cannot find parent EntityContainer")

                    target_entity_container = entity_set.get_first_parent_of_type(EntityContainer)
                    if target_entity_container is None:
                        raise SchemaError("Cannot find target parent EntityContainer")

                    if my_entity_container != target_entity_container:
                        raise SchemaError(
                            "When EntitySet is a SimpleIdentifier, must resolve to an entity set in"
                            " the same entity container")

                except Exception:
                    try:
                        check_type(self.entity_type, 'TargetPath')
                    except Exception:
                        raise SchemaError(
                            "EntitySet attribute must be SimpleIdentifier or TargetPath")
        else:
            if self.entity_set:
                raise SchemaError(
                    "EntitySet attribute only allowed when return type is entity or collection of "
                    "entities")


class Term(Element):
    """Class defining an OData Term element.

    Element edm.Term
    Allows annotating a CSDL element or OData resource representation with additional data
    OData Version 4.0 Part 3 Section 14.1

    Attributes:
        name: SimpleIdentifier used to identify this element.
        type: TypeName indicating the type of value must be returned by the expression contained in
              an annotation using the term.
        base_term: Optional QualifiedName of a term in scope to be applied alongside this term.
        default_value: Optional value determining the value of the term when applied without an
                       expression.
        applies_to: List of CSDL element names this term can be applied to.
        nullable: Optional boolean indicating whether this element accepts a null value.
        max_length: Optional integer specifying the maximum length the value for this element can
                    be.
        precision: Optional integer specifying the max number of digits in the element's value for
                   a decimal element or the max number of decimal places in the seconds portion of
                   a temporal element.
        scale: Optional integer or 'variable' value specifying the number of digits allowed to the
               right of the decimal point.
        srid: Optional value for geometry or geography element specifying the spatial reference
              system applied.
    """

    def __init__(self, data, parent):
        self.name = None
        self.type = None
        self.base_term = None
        self.default_value = None
        self.applies_to = []
        self.nullable = True
        self.max_length = None
        self.precision = None
        self.scale = None
        self.srid = None

        super(Term, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.name = self._get_required_attrib('Name')
        self.error_id = self.name
        check_type(self.name, 'SimpleIdentifier')

        self.type = self._get_required_attrib('Type')
        check_type(self.type, 'TypeName')

        if 'BaseTerm' in self.raw_data.attrib:
            self.base_term = self.raw_data.attrib['BaseTerm']
            check_type(self.base_term, 'QualifiedName')

        if 'DefaultValue' in self.raw_data.attrib:
            self.default_value = self.raw_data.attrib['DefaultValue']

        if 'AppliesTo' in self.raw_data.attrib:
            self.applies_to = self.raw_data.attrib['AppliesTo'].split(" ")
            for name in self.applies_to:
                if name not in CSDL_NAMES:
                    raise SchemaError("AppliesTo {} is not a CSDL name".format(name))

        if 'Nullable' in self.raw_data.attrib:
            check_type(self.raw_data.attrib['Nullable'], 'Boolean')
            self.nullable = (self.raw_data.attrib['Nullable'] == 'true')

        if 'MaxLength' in self.raw_data.attrib:
            self.max_length = self.raw_data.attrib['MaxLength']
            if self.max_length != 'max':
                try:
                    self.max_length = int(self.max_length)
                except Exception:
                    raise SchemaError("MaxLength facet {} is not valid (int or 'max')".format(
                        self.max_length))

                if self.max_length <= 0:
                    raise SchemaError("MaxLength facet {} is not a positive integer".format(
                        self.max_length))

        if 'Precision' in self.raw_data.attrib:
            try:
                self.precision = int(self.raw_data.attrib['Precision'])
            except Exception:
                raise SchemaError("Precision facet {} is not valid (int)".format(self.precision))

        if 'Scale' in self.raw_data.attrib:
            self.scale = self.raw_data.attrib['Scale']
            if self.scale != 'variable':
                try:
                    self.scale = int(self.raw_data.attrib['Scale'])
                except Exception:
                    raise SchemaError("Scale facet {} is not valid (int or 'variable')".format(
                        self.scale))

                if self.scale < 0:
                    raise SchemaError("Scale facet {} is not a non-negative integer".format(
                        self.scale))

        if (self.precision != None) and (self.scale != None):
            if self.scale > self.precision:
                raise SchemaError("Scale facet {} > Precision facet {}".format(self.scale,
                                                                               self.precision))

        if 'SRID' in self.raw_data.attrib:
            self.srid = self.raw_data.attrib['SRID']
            if self.srid != 'variable':
                try:
                    self.srid = int(self.srid)
                except Exception:
                    raise SchemaError("SRID facet {} is not valid (int or 'variable')".format(
                        self.srid))

                if self.srid < 0:
                    raise SchemaError("SRID facet {} is not a non-negative integer".format(
                        self.srid))

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        if self.base_term:
            base_term = self.find_in_scope(self.base_term)
            if not isinstance(base_term, Term):
                raise SchemaError("BaseTerm attribute does not resolve to a Term in scope")

        _, data = is_collection(self.type)
        type_element = self.find_in_scope(data)

        if not (isinstance(type_element, PrimitiveType) or isinstance(type_element, AbstractType)
                or isinstance(type_element, TypeDefinition) or
                isinstance(type_element, ComplexType) or isinstance(type_element, EnumType) or
                isinstance(type_element, EntityType)):
            raise SchemaError(
                "TypeName {} is not PrimitiveType, AbstractType, TypeDefinition, ComplexType, "
                "EnumType or EntityType".format(self.type))

        if isinstance(type_element, PrimitiveType):
            prim_type = type_element.get_primitive_type()
            prim_type.check_max_length_valid(self.raw_data.attrib)
            prim_type.check_precision_valid(self.raw_data.attrib)
            prim_type.check_scale_valid(self.raw_data.attrib)
            prim_type.check_srid_valid(self.raw_data.attrib)

            if self.srid is None:
                self.srid = prim_type.get_srid_default()
            if self.scale is None:
                self.scale = prim_type.get_scale_default()

            if self.precision != None:
                prim_type.check_precision_value(self.precision)

            if self.default_value != None:
                self.default_value = prim_type.convert(self.default_value)
        else:
            if (self.max_length != None) or (self.precision != None) or (
                    self.scale != None) or (self.srid != None):
                raise SchemaError("Not PrimitiveType and facets are specified")


class Annotations(Element):
    """Class defining an OData Annotations element.

    Element edm.Annotations
    Used to apply a group of annotations to a single model element
    OData Version 4.0 Part 3 Section 14.2

    Attributes:
        target: TargetPath resolving to a model element in the entity model to apply children
                annotation elements to.
        qualifier: Optional SimpleIdentifier allowing for authors to conditionally apply an
                   annotation.
    """

    def __init__(self, data, parent):
        self.target = None
        self.qualifier = None

        super(Annotations, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.target = self._get_required_attrib('Target')
        self.error_id = self.target
        check_type(self.target, 'TargetPath')

        if 'Qualifier' in self.raw_data.attrib:
            self.qualifier = self.raw_data.attrib['Qualifier']
            check_type(self.qualifier, 'SimpleIdentifier')

        self._get_annotations()

        if self.qualifier:
            for annotation in self.annotation:
                if annotation.qualifier:
                    raise SchemaError(
                        "Children Annotation cannot specify qualifier when Annotations does")


class Annotation(Element):
    """Class defining an OData Annotation element.

    Element edm.Annotation
    Applies a term to a model element and defines how to calculate a value for the term application
    OData Version 4.0 Part 3 Section 14.3

    Attributes:
        term: QualifiedName of a term in scope to be applied.
        qualifier: Optional SimpleIdentifier used to conditionally apply the annotation.
    """

    def __init__(self, data, parent):
        self.term = None
        self.qualifier = None

        super(Annotation, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.term = self._get_required_attrib('Term')
        self.error_id = self.term
        check_type(self.term, 'QualifiedName')

        if 'Qualifier' in self.raw_data.attrib:
            self.qualifier = self.raw_data.attrib['Qualifier']
            check_type(self.qualifier, 'SimpleIdentifier')

        self._get_expressions()

        self._get_annotations()

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        term = self.find_in_scope(self.term)
        if not isinstance(term, Term):
            raise SchemaError("Term attribute does not evaluate to a Term element")


class ConstantExpression(Element):
    """Class defining all OData ConstantExpression elements.

    Constant Expression
    Allow assigning a Constant value to an applied term
    OData Version 4.0 Part 3 Section 14.4

    Attributes:
        name: Name of the constant expression this element is.
        data_type: String indicating whether this expression was defined in attribute or element
                   notation.
        type: Underlying type that this expression evaluates to.
        value: Value that this expression evaluuates to.
    """

    def __init__(self, data, name, data_type, parent):
        self.name = name
        self.data_type = data_type
        self.type = None
        self.value = None

        super(ConstantExpression, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.error_id = self.name
        if self.data_type == 'attribute':
            data = self.raw_data
        else:
            data = self.raw_data.text

        if self.name == 'Binary':
            self.type = 'Edm.Binary'
            try:
                self.value = base64.b64decode(data, altchars=['-', '_'], validate=True)
            except Exception as error:
                raise SchemaError("Data {} is invalid ({})".format(data, error))

        elif self.name == 'Bool':
            self.type = 'Edm.Boolean'
            if data == 'true':
                self.value = True
            elif data == 'false':
                self.value = False
            else:
                raise SchemaError("Data {} is not true or false".format(data))

        elif self.name == 'Date':
            self.type = 'Edm.Date'
            values = data.split('-')
            if len(values) != 3:
                raise SchemaError("Data {} does not have 3 elements".format(data))

            year = int(values[0])
            month = int(values[1])
            day = int(values[2])

            if year > 9999:
                raise SchemaError("Year {} is invalid (0000-9999)".format(year))

            if month > 12 or month == 0:
                raise SchemaError("Month {} is invalid (1-12)".format(month))

            if day == 0:
                raise SchemaError("Day {} is invalid (!= 0)".format(day))

            if month in [4, 6, 9, 11]:
                if day > 30:
                    raise SchemaError("Day {} invalid for month {} (1-30)".format(day, month))
            elif month == 2 and ((year % 4 != 0) or ((year % 100 == 0) and (year % 400 != 0))):
                if day > 28:
                    raise SchemaError("Day {} invalid for month {} and year {} (1-28)".format(
                        day, month, year))
            elif month == 2 and ((year % 400 == 0) or (year % 4 == 0)) and (year % 100 != 0):
                if day > 29:
                    raise SchemaError("Day {} invalid for month {} and year {} (1-29)".format(
                        day, month, year))
            else:
                if day > 31:
                    raise SchemaError("Day {} invalid for month {} (1-31)".format(day, month))

            self.value = datetime.date(year, month, day)

        elif self.name == 'DateTimeOffset':
            self.type = 'Edm.DateTimeOffset'
            values = data.split('T')

            if len(values) != 2:
                raise SchemaError('Data {} does not have the format Date T Time'.format(data))

            date = values[0]

            date_split = date.split('-')
            if len(date_split) != 3:
                raise SchemaError("Data {} does not have 3 elements".format(date_split))

            year = int(date_split[0])
            month = int(date_split[1])
            day = int(date_split[2])

            if year > 9999:
                raise SchemaError("Year {} is invalid (0000-9999)".format(year))

            if month > 12 or month == 0:
                raise SchemaError("Month {} is invalid (1-12)".format(month))

            if day == 0:
                raise SchemaError("Day {} is invalid (!= 0)".format(day))

            if month in [4, 6, 9, 11]:
                if day > 30:
                    raise SchemaError("Day {} invalid for month {} (1-30)".format(day, month))
            elif month == 2 and ((year % 4 != 0) or ((year % 100 == 0) and (year % 400 != 0))):
                if day > 28:
                    raise SchemaError("Day {} invalid for month {} and year {} (1-28)".format(
                        day, month, year))
            elif month == 2 and ((year % 400 == 0) or (year % 4 == 0)) and (year % 100 != 0):
                if day > 29:
                    raise SchemaError("Day {} invalid for month {} and year {} (1-29)".format(
                        day, month, year))
            else:
                if day > 31:
                    raise SchemaError("Day {} invalid for month {} (1-31)".format(day, month))

            time = values[1]

            if 'Z' in time:
                if time[-1] != 'Z':
                    raise SchemaError('Time {} has invalid format'.format(time))
                offset_hr = 0
                offset_min = 0
                time = time.replace('Z', '')
            elif '-' in time:
                tmp = time.split('-')
                time = tmp[0]
                offset = tmp[1]
                offset_split = offset.split(':')
                if len(offset_split) != 2:
                    raise SchemaError("Offset {} has invalid format (HH:MM)".format(offset))
                offset_hr = -1 * int(offset_split[0])
                offset_min = int(offset_split[1])
            elif '+' in time:
                tmp = time.split('+')
                time = tmp[0]
                offset = tmp[1]
                offset_split = offset.split(':')
                if len(offset_split) != 2:
                    raise SchemaError("Offset {} has invalid format (HH:MM)".format(offset))
                offset_hr = int(offset_split[0])
                offset_min = int(offset_split[1])
            else:
                raise SchemaError("Invalid offset format")

            time_split = time.split('.')
            time = time_split[0]
            if len(time_split) == 2:
                frac_second = int(time_split[1])
            else:
                frac_second = 0

            time_split = time.split(':')
            if len(time_split) < 2:
                raise SchemaError("Time {} does not have at least 2 elements".format(time_split))

            hour = int(time_split[0])
            minute = int(time_split[1])
            if len(time_split) == 3:
                second = int(time_split[2])
            else:
                second = 0

            tz = TimeZone(offset_hr, offset_min)
            self.value = datetime.datetime(year, month, day, hour, minute, second, frac_second, tz)

        elif self.name == 'Decimal':
            self.type = 'Edm.Decimal'
            try:
                self.value = float(data)
            except Exception as error:
                raise SchemaError("data {} is invalid ({})".format(data, error))

        elif self.name == 'Duration':
            self.type = 'Edm.Duration'
            if data[0] == '-':
                sign = -1
            else:
                sign = 1

            duration = data.split('P')[1]

            year = 0
            month = 0
            day = 0
            hour = 0
            minute = 0
            second = 0
            frac_second = 0
            keys = [('Y', year), ('M', month), ('D', day), ('H', hour), ('M', minute)]

            for key in keys:
                if key[0] in duration:
                    split = duration.split(key[0])
                    if len(split) != 2:
                        raise SchemaError('Invalid {} tag in duration {}'.format(key[0], duration))
                    key[1] = int(split[0])
                    duration = split[1]

            if 'S' in duration:
                split = duration.split('S')
                if len(split) != 1:
                    raise SchemaError('Invalid S tag in duration {}'.format(duration))

                split = split[0].split('.')
                second = int(split[0])
                if len(split) > 1:
                    frac_second = int(split[1])

            #TODO: convert to timedelta and take sign into account
            self.value = datetime.datetime(year, month, day, hour, minute, second, frac_second)

        elif self.name == 'EnumMember':
            values = data.split()
            self.value = values

        elif self.name == 'Float':
            self.type = 'Edm.Double'
            try:
                self.value = float(data)
            except Exception as error:
                raise SchemaError("data {} is invalid ({})".format(data, error))

        elif self.name == 'Guid':
            self.type = 'Edm.Guid'

            split = data.split('-')

            if len(split) != 5:
                raise SchemaError('data {} invalid (8dig-4dig-4dig-4dig-12-dig)'.format(data))

            if (len(split[0]) != 8 or len(split[1]) != 4 or len(split[2]) != 4 or
                len(split[3]) != 4 or len(split[4]) != 12):
                raise SchemaError('data {} invalid (8dig-4dig-4dig-4dig-12-dig)'.format(data))

            self.value = data

        elif self.name == 'Int':
            self.type = 'Edm.Int64'
            try:
                self.value = int(data)
            except Exception as error:
                raise SchemaError("data {} is invalid ({})".format(data, error))

        elif self.name == 'String':
            self.type = 'Edm.String'
            self.value = data

        elif self.name == 'TimeOfDay':
            self.type = 'Edm.TimeOfDay'

            if '.' in data:
                split = data.split('.')
                frac_second = int(split[1])
                time = split[0]
            else:
                frac_second = 0
                time = data

            time_split = time.split(':')

            if len(time_split) < 2:
                raise SchemaError("Data {} is invalid (HH:MM:SS.frac_second)".format(data))

            hour = int(time_split[0])
            minute = int(time_split[1])

            if len(time_split) > 2:
                second = int(time_split[2])
            else:
                second = 0

            if (hour > 23) or (minute > 59) or (second > 59):
                raise SchemaError("Data values are over limit (H>23, M>59, S>59)")

            self.value = datetime.time(hour, minute, second, frac_second)

        else:
            raise SchemaError("Unknown constant expression type")

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        if self.name == 'EnumMember':
            local_val = 0
            enum_type = None
            for value in self.value:
                member = self.find_in_scope(value)
                member.parent.check_scope()
                local_val |= member.value
                if not enum_type:
                    enum_type = member.parent
                else:
                    if member.parent != enum_type:
                        raise SchemaError("All members must be of the same enumerated type")

            if len(self.value) > 1 and not enum_type.is_flags:
                raise SchemaError("EnumType must have is flags set to use multiple values")

            self.type = self.find_in_scope(enum_type.underlying_type)
            self.value = local_val
        else:
            self.type = self.find_in_scope(self.type)

    def evaluate(self):
        """Expression method to return calculated value.

        For a constant expression this method is simple since the value is already calculated when
        the element was parsed.

        Args:
            None

        Returns:
            Value of the parsed expression.
        """

        return self.value


class DynamicExpression(Element):
    """Class defining all OData DynamicExpression elements

    Dynamic Expression
    Allow assigning a calculated value to an applied term
    OData Version 4.0 Part 3 Section 14.5

    Attributes:
        name: Name of the constant expression this element is.
        data_type: String indicating whether this expression was defined in attribute or element
                   notation.
        type: Underlying type that this expression evaluates to.
        function: For an Apply expression, the function to apply.
        value: Value that this expression evaluates to.
        property: For a PropertyValue element, the property to assign the value to
        data: For a LabeledElementReference, the name of a labeled element expression in scope.
        target_name: For a LabeledElement, a link to the named expression.
        target_term: For a Record, target term if type has to be defived from context.
    """

    def __init__(self, data, name, data_type, parent):
        self.name = name
        self.data_type = data_type
        self.type = None
        self.target_type = None
        self.function = None
        self.value = None
        self.property = None
        self.data = None
        self.target_name = None
        self.target_term = None

        super(DynamicExpression, self).__init__(data, parent)

    def parse(self):
        """Parse and validate this element.

        Verifies all attributes for this element as well as kicks off parsing and validating of all
        children elements. Performs basic OData format and data validation.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: if there was an issue parsing or validating this element.
        """

        self.error_id = self.name

        if self.data_type == 'attribute':
            data = self.raw_data
        else:
            data = self.raw_data.text

        if (self.name not in ['AnnotationPath', 'NavigationPropertyPath', 'Path', 'PropertyPath',
                              'UrlRef']) and self.data_type == 'attribute':
            raise SchemaError("{} does not support attribute notation".format(self.name))

        if self.name == 'And':
            self.type = 'Edm.Boolean'
            self._get_expressions()
            if len(self.expressions) != 2:
                raise SchemaError("needs two inputs, provided {}".format(len(self.expressions)))

            if (self.expressions[0].type != 'Edm.Boolean') or (
                    self.expressions[1].type != 'Edm.Boolean'):
                raise SchemaError("Logical inputs do not evaluate to boolean values")

            self._get_annotations()

        elif self.name == 'Or':
            self.type = 'Edm.Boolean'
            self._get_expressions()
            if len(self.expressions) != 2:
                raise SchemaError("needs two inputs, provided {}".format(len(self.expressions)))

            if (self.expressions[0].type != 'Edm.Boolean') or (
                    self.expressions[1].type != 'Edm.Boolean'):
                raise SchemaError("Logical inputs do not evaluate to boolean values")

            self._get_annotations()

        elif self.name == 'Not':
            self.type = 'Edm.Boolean'
            self._get_expressions()
            if len(self.expressions) != 1:
                raise SchemaError("needs one input, provided {}".format(len(self.expressions)))

            if self.expressions[0].type != 'Edm.Boolean':
                raise SchemaError("Logical inputs do not evaluate to boolean values")

            self._get_annotations()

        elif self.name == 'Eq':
            self.type = 'Edm.Boolean'
            self._get_expressions()
            if len(self.expressions) != 2:
                raise SchemaError("needs two inputs, provided {}".format(len(self.expressions)))

            self._get_annotations()

        elif self.name == 'Ne':
            self.type = 'Edm.Boolean'
            self._get_expressions()
            if len(self.expressions) != 2:
                raise SchemaError("needs two inputs, provided {}".format(len(self.expressions)))

            self._get_annotations()

        elif self.name == 'Gt':
            self.type = 'Edm.Boolean'
            self._get_expressions()
            if len(self.expressions) != 2:
                raise SchemaError("needs two inputs, provided {}".format(len(self.expressions)))

            self._get_annotations()

        elif self.name == 'Ge':
            self.type = 'Edm.Boolean'
            self._get_expressions()
            if len(self.expressions) != 2:
                raise SchemaError("needs two inputs, provided {}".format(len(self.expressions)))

            self._get_annotations()

        elif self.name == 'Lt':
            self.type = 'Edm.Boolean'
            self._get_expressions()
            if len(self.expressions) != 2:
                raise SchemaError("needs two inputs, provided {}".format(len(self.expressions)))

            self._get_annotations()

        elif self.name == 'Le':
            self.type = 'Edm.Boolean'
            self._get_expressions()
            if len(self.expressions) != 2:
                raise SchemaError("needs two inputs, provided {}".format(len(self.expressions)))

            self._get_annotations()

        elif self.name == 'AnnotationPath':
            #TODO: types
            self.value = data
            #TODO: Check this

        elif self.name == 'Apply':
            self.function = self._get_required_attrib('Function')
            check_type(self.function, 'QualifiedName')

            if self.function == 'odata.concat':
                self.type = 'Edm.String'
                for element in self.raw_data:
                    expression = self.evaluate_expression(element)
                    if not expression:
                        raise SchemaError("Child is not constant or dynamic expression")

            elif self.function == 'odata.fillUriTemplate':
                self.type = 'Edm.String'
                self.value = data
                #TODO: RFC6570

            elif self.function == 'odata.uriEncode':
                self.type = 'Edm.String'
                self.value = data
                #TODO: encode this. Strings are single quote escaped

            else:
                raise SchemaError("Function {} is unknown".format(self.function))

            self._get_annotations()

        elif self.name == 'Cast':
            self.type = self._get_required_attrib('Type')
            check_type(self.type, 'TypeName')
            #TODO: check that this is a  built-in primitive or abstract type, a type definition,
            #      complex type, enumeration type, or entity type, or a collection of one of these
            #      types in scope

            #TODO: convert this to the real type

            self._get_expressions()
            if len(self.expressions) != 1:
                raise SchemaError("needs one input, provided {}".format(len(self.expressions)))

            self._get_annotations()

        elif self.name == 'Collection':
            self._get_expressions()

        elif self.name == 'If':
            self._get_expressions()

            if self.expressions[0].type.name != 'Edm.Boolean':
                raise SchemaError("First element does not evaluate to boolean")

            if isinstance(self.parent, DynamicExpression) and self.parent.name == 'Collection':
                if len(self.expressions) not in [2, 3]:
                    raise SchemaError("Needs two or three inputs, provided {}".format(len(
                        self.expressions)))
            else:
                if len(self.expressions) != 3:
                    raise SchemaError("needs three inputs, provided {}".format(len(
                        self.expressions)))

            self._get_annotations()

        elif self.name == 'IsOf':
            self.type = 'Edm.Boolean'

            self.target_type = self._get_required_attrib('type')
            check_type(self.target_type, 'TypeName')

            self._get_expressions()

            if len(self.expressions) != 1:
                raise SchemaError("needs one input, provided {}".format(len(self.expressions)))

            self._get_annotations()

        elif self.name == 'LabeledElement':
            self.type = 'Edm.String'
            self._get_expressions()

            if len(self.expressions) != 1:
                raise SchemaError("Requires one child expression")

            self.target_name = self._get_required_attrib('Name')
            check_type(self.name, 'SimpleIdentifier')

            self._get_annotations()

        elif self.name == 'LabeledElementReference':
            self.type = 'Edm.String'
            check_type(data, 'QualifiedName')

            self.data = data

        elif self.name == 'Null':
            self.type = 'Untyped'
            self._get_expressions()

            if len(self.expressions) != 0:
                raise SchemaError("Cannot have any expressions")

            #TODO: check that there are no elements

            self._get_annotations()

        elif self.name == 'NavigationPropertyPath':
            #TODO: change this type
            self.type = 'Edm.String'
            #TODO: this

        elif self.name == 'Path':
            #TODO: change this type
            self.type = 'Edm.String'
            self.validate_path(data)

        elif self.name == 'PropertyPath':
            #TODO: change this type
            self.type = 'Edm.String'
            #TODO: this

        elif self.name == 'Record':
            self._get_expressions()

            for expression in self.expressions:
                if expression.name != 'PropertyValue':
                    raise SchemaError("Need only PropertyValues, provided {}".format(type(
                        element)))

            self.target_term = None
            if 'type' in self.raw_data.attrib:
                self.type = self.raw_data.attrib['type']
                check_type(self.type, 'QualifiedName')
            else:
                if (isinstance(self.parent, DynamicExpression) and
                        self.parent.name == 'Collection'):
                    if isinstance(self.parent.parent, Annotation):
                        self.target_term = self.parent.parent.term
                    else:
                        self.type = self.parent.parent.type
                else:
                    self.type = self.parent.type

            self._get_annotations()

        elif self.name == 'PropertyValue':
            self.property = self._get_required_attrib('Property')
            check_type(self.property, "SimpleIdentifier")

            self._get_expressions()
            if len(self.expressions) != 1:
                raise SchemaError("Need one expression, given {}".format(len(self.expressions)))

            self._get_annotations()

        elif self.name == 'UrlRef':
            self.type = 'Edm.String'

            self._get_expressions()
            if len(self.expressions) != 1:
                raise SchemaError("Need one expression, given {}".format(len(self.expressions)))

            if self.expressions[0].type != 'Edm.String':
                raise SchemaError("Child expression must be a string")

            self._get_annotations()

        else:
            raise SchemaError("Unknown DynamicExpression {}".format(self.name))

    def check_scope(self):
        """Function for checking the scope of this element.

        This function is called once the basic crawling has been done on the entire metadata
        document. It performs checks which validate scope dependancies or require other elemets to
        be parsed already.

        Args:
            None

        Returns:
            None

        Raises:
            SchemaError: If there is an error in validating the scope or related elements of this
                         element.
        """

        if self.name == 'Eq':
            if not self.expressions[0].type.is_comparable(self.expressions[1].type):
                raise SchemaError("Child expressions are not comparable values")

        elif self.name == 'Ne':
            if not self.expressions[0].type.is_comparable(self.expressions[1].type):
                raise SchemaError("Child expressions are not comparable values")

        elif self.name == 'Gt':
            if not self.expressions[0].type.is_comparable(self.expressions[1].type):
                raise SchemaError("Child expressions are not comparable values")

        elif self.name == 'Ge':
            if not self.expressions[0].type.is_comparable(self.expressions[1].type):
                raise SchemaError("Child expressions are not comparable values")

        elif self.name == 'Lt':
            if not self.expressions[0].type.is_comparable(self.expressions[1].type):
                raise SchemaError("Child expressions are not comparable values")

        elif self.name == 'Le':
            if not self.expressions[0].type.is_comparable(self.expressions[1].type):
                raise SchemaError("Child expressions are not comparable values")

#         elif self.name == 'AnnotationPath':
#             #TODO: types
#             self.value = data
#             #TODO: Check this
#
#         elif self.name == 'Apply':
#             self.function = self._get_required_attrib('Function')
#             check_type(self.function, 'QualifiedName')
#
#             if self.function == 'odata.concat':
#                 self.type = 'Edm.String'
#                 for element in self.raw_data:
#                     expression = self.evaluate_expression(element)
#                     if not expression:
#                         raise SchemaError("Child is not constant or dynamic expression")
#
#             elif self.function == 'odata.fillUriTemplate':
#                 self.type = 'Edm.String'
#                 self.value = data
#                 #TODO: RFC6570
#
#             elif self.function == 'odata.uriEncode':
#                 self.type = 'Edm.String'
#                 self.value = data
#                 #TODO: encode this. Strings are single quote escaped
#
#             else:
#                 raise SchemaError("Function {} is unknown".format(self.function))
#
#             self._get_annotations()
#
#         elif self.name == 'Cast':
#             self.type = self._get_required_attrib('Type')
#             check_type(self.type, 'TypeName')
#             #TODO: check that this is a  built-in primitive or abstract type, a type definition,
#             #      complex type, enumeration type, or entity type, or a collection of one of these
#             #      types in scope
#
#             #TODO: convert this to the real type
#
#             self._get_expressions()
#             if len(self.expressions) != 1:
#                 raise SchemaError("needs one input, provided {}".format(len(self.expressions)))
#
#             self._get_annotations()
#
        elif self.name == 'Collection':
            type_names = []
            for expression in self.expressions:
                type_names.append(expression.type)

            if len(set(type_names)) > 1:
                raise SchemaError("children are different types")

            self.type = 'Collection(' + type_names[0] + ')'

        elif self.name == 'IsOf':
            _, type_name = is_collection(self.target_type)
            target_type = self.find_in_scope(type_name)

            if not (isinstance(target_type, PrimitiveType) or isinstance(target_type, AbstractType)
                    or isinstance(target_type, TypeDefinition) or
                    isinstance(target_type, ComplexType) or isinstance(target_type, EnumType) or
                    isinstance(target_type, EntityType)):
                raise SchemaError(
                    "TargetType is not Primitive, Abstract, Complex, Enum or Entity type,  a type "
                    "definition or a collection of one of these types")

        elif self.name == 'LabeledElement':
            schema = self.get_first_parent_of_type(Schema)
            if schema is None:
                raise SchemaError("Cannot find parent Schema")

            if self.target_name in schema.data_services.keys():
                raise SchemaError("Name must be unique in schema")

            schema.data_services[self.target_name] = self

            self.type = self.elements[0].type

        elif self.name == 'LabeledElementReference':
            target = self.find_in_scope(self.data)

            if not (isinstance(target, DynamicExpression) and target.name == 'LabeledElement'):
                raise SchemaError("Must point to a LabeledElement")

            self.type = target.type

#
#         elif self.name == 'Null':
#             self.type = 'Untyped'
#             self._get_expressions()
#
#             if len(self.expressions) != 0:
#                 raise SchemaError("Cannot have any expressions")
#
#             #TODO: check that there are no elements
#
#             self._get_annotations()
#
#         elif self.name == 'NavigationPropertyPath':
#             #TODO: change this type
#             self.type = 'Edm.String'
#             #TODO: this
#             i=0
#
#         elif self.name == 'Path':
#             #TODO: change this type
#             self.type = 'Edm.String'
#             self.validate_path(data)
#
#         elif self.name == 'PropertyPath':
#             #TODO: change this type
#             self.type = 'Edm.String'
#             #TODO: this
#             i=0
#
        elif self.name == 'Record':

            if self.target_term:
                term = self.find_in_scope(self.target_term)
                self.type = term.type

            my_type = self.find_in_scope(self.type)

            if not (isinstance(my_type, EntityType) or isinstance(my_type, ComplexType)):
                raise SchemaError("Type must resolve to Complex or Entity Type in scope")

            my_type.check_scope()

            checked_prop_values = []

            for prop in my_type.properties:
                if not prop.nullable and not prop.default_value:
                    found = False
                    for prop_value in self.expressions:
                        if prop_value.property == prop.name:
                            checked_prop_values.append(prop_value)
                            prop_value.type = prop.type
                            found = True
                    if not found:
                        raise SchemaError("Property {} needs a PropertyValue expression".format(
                            prop.name))

            for prop in my_type.nav_properties:
                if not prop.nullable:
                    found = False
                    for prop_value in self.expressions:
                        if prop_value.property == prop.name:
                            checked_prop_values.append(prop_value)
                            prop_value.type = prop.type
                            found = True
                    if not found:
                        raise SchemaError(
                            "NavigationProperty {} needs a PropertyValue expression".format(
                                prop.name))

            for prop_value in self.expressions:
                if prop_value not in checked_prop_values:
                    found = False
                    for prop in my_type.properties + my_type.nav_properties:
                        if prop_value.property == prop.name:
                            found = True
                            prop_value.type = prop.type
                    if not found:
                        raise SchemaError("PropertyValue {} not found in type".format(
                            prop_value.property))

    def evaluate(self):
        """Expression method to return calculated value.

        Dynamic expressions are evaluated when the method is called since these can change on the
        client side.

        Args:
            None

        Returns:
            Value of the parsed expression.
        """

        if self.data_type == 'attribute':
            data = self.raw_data
        else:
            data = self.raw_data.text

        if self.name == 'And':
            val0 = self.expressions[0].evaluate()
            val1 = self.expressions[1].evaluate()
            value = val0 and val1

        elif self.name == 'Or':
            val0 = self.expressions[0].evaluate()
            val1 = self.expressions[1].evaluate()
            value = val0 or val1

        elif self.name == 'Not':
            val0 = self.expressions[0].evaluate()
            value = not val0

        elif self.name == 'Eq':
            val0 = self.expressions[0].evaluate()
            val1 = self.expressions[1].evaluate()
            value = val0 == val1

        elif self.name == 'Ne':
            val0 = self.expressions[0].evaluate()
            val1 = self.expressions[1].evaluate()
            value = val0 != val1

        elif self.name == 'Gt':
            val0 = self.expressions[0].evaluate()
            val1 = self.expressions[1].evaluate()

            value = val0.value > val1.value

        elif self.name == 'Ge':
            val0 = self.expressions[0].evaluate()
            val1 = self.expressions[1].evaluate()

            value = val0.value >= val1.value

        elif self.name == 'Lt':
            val0 = self.expressions[0].evaluate()
            val1 = self.expressions[1].evaluate()

            value = val0.value < val1.value

        elif self.name == 'Le':
            val0 = self.expressions[0].evaluate()
            val1 = self.expressions[1].evaluate()

            value = val0.value <= val1.value

        elif self.name == 'AnnotationPath':
            #TODO: This
            value = data

        elif self.name == 'Apply':
            self.function = self._get_required_attrib('Function')
            check_type(self.function, 'QualifiedName')

            if self.function == 'odata.concat':
                self.type = 'Edm.String'
                value = ''
                for element in self.raw_data:
                    expression = self.evaluate_expression(element)
                    value += str(expression.value)

            elif self.function == 'odata.fillUriTemplate':
                self.type = 'Edm.String'
                value = data
                #TODO: RFC6570

            elif self.function == 'odata.uriEncode':
                self.type = 'Edm.String'
                value = data
                #TODO: encode this. Strings are single quote escaped

            else:
                raise SchemaError("Function {} is unknown".format(self.function))

        elif self.name == 'Cast':
            #TODO: do the cast
            value = self.expressions[0].evaluate()

        elif self.name == 'Collection':
            value = []
            for expression in self.expressions:
                value.append(expression.evaluate())

        elif self.name == 'If':
            value = data
            #CRAIG work here
        else:
            #TODO: remove this once all dynamics are in
            value = None

        return value

def main():
    """Main function for the OData Validator.

    Provides a user interface for this tool.

    Args:
        None

    Returns:
        None
    """

    parser = argparse.ArgumentParser(description="OData Validation Tool")
    parser.add_argument(
        "MetaData",
        help="Path to the metadata to be parsed, could be a url (start with http), file or folder")

    args = parser.parse_args()

    if args.MetaData.lower().startswith('http') or os.path.isfile(args.MetaData):
        #Metadata points to a uri or a local file
        MetaData(args.MetaData)
    elif os.path.isdir(args.MetaData):
        #Metadata points to a directory
        for f_name in os.listdir(args.MetaData):
            MetaData(args.MetaData+f_name)
    else:
        #Metadata is unknown
        print("Can not locate metadata")

    print("No errors in MetaData")

if __name__ == "__main__":
    main()

