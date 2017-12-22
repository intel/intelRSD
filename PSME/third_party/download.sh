#!/bin/sh

DIR=`dirname $0`/../cmake

# TODO replace grep with sed/awk
grep 'assure_package(' ${DIR}/Install*.cmake | sed 's/^.*: *assure_package(//1;s/ *) *$//1' | sed 's/"//g' | while read package version link md5; do
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
        *)
            ext=""
            ;;
    esac
    echo package=${package} version=${version} link=${link} md5=${md5} ext=${ext}
    wget "${link}" -O ${package}-${version}${ext}
    if [ -n "${md5}" ]; then
        md5sum ${package}-${version}${ext} | while read sum file; do
            if [ "${sum}" != "${md5}" ]; then
                echo ${file} has incorrect md5 sum ${sum}, expected ${md5} 1>&2
                exit 1
            fi
        done
    fi
done
