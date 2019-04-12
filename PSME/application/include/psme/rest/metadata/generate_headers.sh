#!/bin/bash
# @copyright
# Copyright (c) 2015-2019 Intel Corporation
#
# @copyright
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# @copyright
#    http://www.apache.org/licenses/LICENSE-2.0
#
# @copyright
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

HEADER_DIR="include-generated/"
HEADER_EXT=".hpp"
HEADER_ANY="*.hpp"

XML_DIR="xml/"
XML_EXT=".xml"
XML_ANY="*.xml"

HG_PREFIX="PSME_REST_METADATA_"
HG_SUFFIX="_HPP"

RAW_STR_DELIMITER="Xy+Z*aF--"

MAIN_HEADER="$HEADER_DIR""generated_headers.hpp"
MAP_INITIALIZER="$HEADER_DIR""initialize_map.hpp"


# delete the generated headers dir
function CLEAN_HEADER {
    rm -rf $HEADER_DIR
}

#remove the dir and extension from an xml file name
function RAW_FILE {
    local name=${1//$XML_DIR/}
    name=${name/$XML_EXT/}
    echo ${name//"."/}
}

#remove the dir from and xml file name
function XML_FILE {
    echo ${1/$XML_DIR/}
}

# adds /redfish/v1/metadata to the links
function FIX_LINKS {
    sed -i 's/ Uri=\"/&\/redfish\/v1\/metadata\//g' $1
}

# return a corresponding header file name for a given xml file
function GET_HEADER_NAME {
    local name=$(RAW_FILE $1)
    # remove dots
    name=$(echo $name | sed "s/[.]//g")
    # add underscores
    name=$(echo $name | sed "s/\([a-zA-Z]\)\([A-Z]\)\([a-zA-Z]\)/\1_\2\3/g")
    # to lowercase
    echo "${name,,}"
}

# gets header guard for the current header file
function GET_GUARDS {
    local guard=${1/$HEADER_EXT/}
    guard=${guard/$HEADER_DIR/}
    # remove all '$'
    guard=$(echo $guard | sed "s/[$]//g")
    echo "$HG_PREFIX${guard^^}$HG_SUFFIX"
}

# adds include of $2 library to $1 file
function ADD_INCLUDE {
    printf "#include <$2>\n" >> $1
}

# adds the #ifndef end #define guards to the header
function ADD_GUARD_TOP {
    header_guard=$(GET_GUARDS $1)
    printf "#ifndef $header_guard\n#define $header_guard\n\n" >> $1
}

# adds openings of namespaces to the header
function OPEN_NAMESPACE {
    printf "\nnamespace psme {\nnamespace rest {\nnamespace metadata {\n\n" >> \
        $1
}

# adds header guard and namespaces to the header
function ADD_HEADER_TOP {
    ADD_GUARD_TOP $1
    ADD_INCLUDE $1 "string"
    OPEN_NAMESPACE $1
}

# closes the header guard with an #endif
function ADD_GUARD_BOTTOM {
    header_guard=$(GET_GUARDS $1)
    printf "\n#endif /* $header_guard */\n" >> $1
}

# closes the namespaces of the header
function CLOSE_NAMESPACE {
    printf "} /* metadata */\n} /* rest */\n} /* psme */\n\n" >> $1
}

# closes header guard and namespaces of the header
function ADD_HEADER_BOTTOM {
    CLOSE_NAMESPACE $1
    ADD_GUARD_BOTTOM $1
}

# run for each xml file
function CREATE_HEADER {
    # create the header file name with location
    HEADER_FILE="$HEADER_DIR$(GET_HEADER_NAME $1)$HEADER_EXT"
    #clear the file
    > $HEADER_FILE
    ADD_HEADER_TOP $HEADER_FILE
    # define the raw string
    printf "static const std::string $(RAW_FILE $1)_XML =\n" >> $HEADER_FILE
    printf "R\"$RAW_STR_DELIMITER(" >> $HEADER_FILE
    # append the xml to the header
    cat $1 >> $HEADER_FILE
    # fix the metadata links
    FIX_LINKS $HEADER_FILE
    # close the raw string
    printf ")$RAW_STR_DELIMITER\";\n\n" >> $HEADER_FILE
    ADD_HEADER_BOTTOM $HEADER_FILE

    echo "Created a header for $1"
}

###############################################################################
#                                                                             #
#                             RUNTIME STARTS HERE                             #
#                                                                             #
###############################################################################

# preparation
cd $DIR
CLEAN_HEADER
mkdir -p $HEADER_DIR


# copy the $metadata.xml file as cmake doesnt accept $-starting files
if [ -f "$XML_DIR"'$metadata'"$XML_EXT" ]; then
    echo "Copying \$metadata.xml -> metadata_root.xml"
    cp "$XML_DIR"'$metadata'"$XML_EXT" "$XML_DIR""metadata_root""$XML_EXT"
fi


# add the the top header guard to map initilizer
ADD_GUARD_TOP $MAP_INITIALIZER
ADD_INCLUDE $MAP_INITIALIZER "unordered_map"
# initializing the xml map
printf "\nstd::unordered_map<std::string, std::string> " >> $MAP_INITIALIZER
printf "psme::rest::metadata::MetadataManager::xml_map = {\n" >> $MAP_INITIALIZER
find $XML_DIR -type f -name "$XML_ANY" | while read file; do
    # ommit the $metadata file
    if [ "$(RAW_FILE $file)" != '$metadata' ]; then
        # generate the header file
        CREATE_HEADER $file
        # add map entry
        printf "\t{\"$(XML_FILE $file)\", $(RAW_FILE $file)_XML},\n"\
            >> $MAP_INITIALIZER
    fi
done
# xml_map definition close
printf "};\n" >> $MAP_INITIALIZER
# header guard close
ADD_GUARD_BOTTOM $MAP_INITIALIZER

echo "Generated metadata map initializer"


# create the header aggregating other headers
ADD_GUARD_TOP $MAIN_HEADER
find $HEADER_DIR -type f -name "$HEADER_ANY" | while read file; do
    if [ "$file" != "$MAP_INITIALIZER" ]; then
        #add entry for each header found
        printf "#include \"${file/$HEADER_DIR/}\"\n" >> $MAIN_HEADER
    fi
done
ADD_GUARD_BOTTOM $MAIN_HEADER

echo "Generated aggregating header file"
