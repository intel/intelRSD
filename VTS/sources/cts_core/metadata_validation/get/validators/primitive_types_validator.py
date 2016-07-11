"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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

from cts_core.metadata.primitive_types_helpers.boolean_helper import BooleanHelper
from cts_core.metadata.primitive_types_helpers.floating_helpers import DecimalHelper, SingleHelper
from cts_core.metadata.primitive_types_helpers.floating_helpers import DoubleHelper
from cts_core.metadata.primitive_types_helpers.integer_helpers import ByteHelper, Int16Helper, Int32Helper, Int64Helper, \
    SByteHelper
from cts_core.metadata.primitive_types_helpers.text_helpers import DateHelper, DateTimeOffsetHelper, GuidHelper, StringHelper
from cts_core.metadata_validation.get.validators.validator import BaseTypeValidator

EDM = "Edm"

EDM_TYPES_DICTIONARY = {
            # ".".join([EDM, "Binaries"]): None,
            ".".join([EDM, "Boolean"]): BooleanHelper,
            ".".join([EDM, "Byte"]): ByteHelper,
            ".".join([EDM, "Date"]): DateHelper,
            ".".join([EDM, "DateTimeOffset"]): DateTimeOffsetHelper,
            ".".join([EDM, "Decimal"]): DecimalHelper,
            ".".join([EDM, "Double"]): DoubleHelper,
            # ".".join([EDM, "Duration"]): None,
            ".".join([EDM, "Guid"]): GuidHelper,
            ".".join([EDM, "Int16"]): Int16Helper,
            ".".join([EDM, "Int32"]): Int32Helper,
            ".".join([EDM, "Int64"]): Int64Helper,
            ".".join([EDM, "SByte"]): SByteHelper,
            ".".join([EDM, "Single"]): SingleHelper,
            ".".join([EDM, "Float"]): SingleHelper,
            # ".".join([EDM, "Stream"]): None,
            ".".join([EDM, "String"]): StringHelper,
            # ".".join([EDM, "TimeOfDay"]): None,
            # ".".join([EDM, "Geography"]): None,
            # ".".join([EDM, "GeographyPoint"]): None,
            # ".".join([EDM, "GeographyLineString"]): None,
            # ".".join([EDM, "GeographyPolygon"]): None,
            # ".".join([EDM, "GeographyMultiPoint"]): None,
            # ".".join([EDM, "GeographyMultiLineString"]): None,
            # ".".join([EDM, "GeographyMultiPolygon"]): None,
            # ".".join([EDM, "GeographyCollection"]): None,
            # ".".join([EDM, "Geometry"]): None,
            # ".".join([EDM, "GeometryPoint"]): None,
            # ".".join([EDM, "GeometryLineString"]): None,
            # ".".join([EDM, "GeometryPolygon"]): None,
            # ".".join([EDM, "GeometryMultiPoint"]): None,
            # ".".join([EDM, "GeometryMultiLineString"]): None,
            # ".".join([EDM, "GeometryMultiPolygon"]): None,
            # ".".join([EDM, "GeometryCollection"]): None,
        }


class PrimitiveTypeValidator(BaseTypeValidator):
    def validate(self, property_type, property_value):
        try:
            type_helper = EDM_TYPES_DICTIONARY[property_type]
        except KeyError:
            print "ERROR:: Unknown type %s of property %s" % (property_type, self.variable_path)
            return False

        try:
            if type_helper().validate(property_value):
                return True
        except Exception:
            pass

        print "ERROR:: Value of field %s does not match required type" % (self.variable_path)

        return False
