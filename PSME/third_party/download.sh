#!/bin/sh

DIR=`dirname $0`/../cmake
WRONG=.wrong.txt
true >${WRONG}

# TODO replace grep with sed/awk
cat ${DIR}/Install*.cmake | sed 's/#.*$//g' | grep 'assure_package' | grep -v macro | sed 's/^ *assure_package *(//1;s/ *) *$//1' | sed 's/"//g' | while read package version link md5; do
    if [ -z "${link}" ]; then
        echo ${package} does not have a link 1>&2
        continue
    fi

    case ${link} in
        *.zip)
            ext=.zip
            ;;
        *.tar*)
            ext=.tar`echo ${link} | sed 's/^.*\\.tar//1'`
            ;;
        *.hpp)
            ext=.hpp
            ;;
        *)
            ext=""
            ;;
    esac

    echo "-- package=${package} version=${version} link=${link} md5=${md5} ext=${ext}"
    file=${package}-${version}${ext}
    wget "${link}" -O ${file}
    if [ ! -s ${file} ]; then
        echo ${file} is empty, download has failed 1>&2
        echo ${file} >>${WRONG}
    elif [ -n "${md5}" ]; then
        md5sum ${file} | while read sum checked; do
            if [ "${sum}" != "${md5}" ]; then
                echo ${file} has incorrect md5 sum: calculated ${sum}, expected ${md5} 1>&2
                echo ${file} >>${WRONG}
            fi
        done
    fi
    echo ""
done

if [ -s ${WRONG} ]; then
    echo There are troubles with:
    cat ${WRONG} | sed 's/^/    /1'
    rm ${WRONG}
    exit 1
else
    echo All third party packages are OK
    rm ${WRONG}
fi
