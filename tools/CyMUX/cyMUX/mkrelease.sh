#!/usr/bin/env bash

###############################################################################
# Copyright (c) 2015, 2016 Intel Corporation
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
#
###############################################################################

# Source supporting routines and process any command line parameters
# mkrelease.sh
#
# Environment variables used (or set if passed via command line)
#
# Control variables:
# DEBUG     - If set, used as a file/device to output additional diagnostics as
#             running.  /dev/null is used if not set
#
# Boolean Control variables:
# FORCE     - if set to 1, override version checks and force update
# SKIP      - Do everything but stop short of actually doing the upgrade
#             Can be used with FORCE (to bypass the version check, but not do
#             the update)

# Set up control variables with default values if necessary
export DEBUG=${DEBUG:-/dev/null}
export FORCE=${FORCE:-0}
export SKIP=${SKIP:-0}
export CYMUX_VERSION=${CYMUX_VERSION:-0.0}

export FPATH=$(readlink -f $(dirname $0))       # Calculate canonical path (possibly from relative path)
export CURDIR=$(pwd)
TEMPDIR=$(mktemp -d)
trap "{ rm -rf $TEMPDIR; exit 255; }" SIGINT SIGTERM
ROOTDIR=$TEMPDIR/cyMUX
MASTER_RELNOTES=$FPATH/release-notes.txt

declare -a RESULTS


# Fetch the version information for this product from the release-notes.txt
# This will be the at the top of the file.

TARFILE=$CURDIR/cyMUX-${CYMUX_VERSION}.tar.gz

if [ ${FORCE:-0} -ne 1 ]; then
    if [ -e $TARFILE ]; then
        echo $TARFILE found...aborting
        exit 1
    fi
fi

declare -a cmds=(
    "mkdir -p $ROOTDIR"
    "cp -r $FPATH/cytool $ROOTDIR"
    "cp -r $FPATH/cyMUX $ROOTDIR"
    "cp -r $FPATH/cyMUX.service $ROOTDIR"
    "cd $TEMPDIR"
    "tar czvf $TARFILE cyMUX"
    )

for cmd in "${cmds[@]}"; do
    echo $cmd >> $DEBUG
    if ! $cmd >> $DEBUG; then
        echo Error executing "$cmd"...aborting
        exit 1
    fi
done


# Time to make the RPM
MASTER_SPEC_FILE=$FPATH/cyMUX.spec
SPEC_FILE=$TEMPDIR/SPECS/cyMUX.spec
unset BACKUP_MACRO
if [ -e $HOME/.rpmmacros ]; then
    BACKUP_MACRO=$(mktemp)
    echo "Backing up $HOME/.rpmmacros to $BACKUP_MACRO" >> $DEBUG
    mv $HOME/.rpmmacros $BACKUP_MACRO
    trap "{ rm -rf $TEMPDIR; mv $BACKUP_MACRO $HOME/.rpmmacros; exit 255; }" SIGINT SIGTERM
fi

echo "Making RPM build environment" >> $DEBUG
for d in BUILD RPMS SOURCES SPECS SRPMS ; do
    cmd="mkdir -p $TEMPDIR/$d"
    echo $cmd >> $DEBUG
    $cmd
done

echo "Creating .rpmmacros for build:" >> $DEBUG
echo "%_topdir $TEMPDIR" > $HOME/.rpmmacros
echo "%_tmppath /var/tmp" >> $HOME/.rpmmacros
cat $HOME/.rpmmacros >> $DEBUG


echo "Creating $SPEC_FILE from $MASTER_SPEC_FILE file" >> $DEBUG
sed -e 's/^Version:[ \t]*.*/Version:   '${CYMUX_VERSION}/ $MASTER_SPEC_FILE > $SPEC_FILE
if ! diff -wubBq $SPEC_FILE $MASTER_SPEC_FILE >/dev/null; then
    echo Updating Spec file $MASTER_SPEC_FILE with version ${CYMUX_VERSION} >> $DEBUG
    cp $SPEC_FILE $MASTER_SPEC_FILE
fi


echo "Building RPM" >> $DEBUG

declare -a cmds=(
    "mv $ROOTDIR $TEMPDIR/BUILD"
    "mv $TARFILE $TEMPDIR/SOURCES"
    "cp $MASTER_SPEC_FILE $SPEC_FILE"
    "rpmbuild -ba $SPEC_FILE"
    "cp $TEMPDIR/SRPMS/cyMUX-${CYMUX_VERSION}*.src.rpm $CURDIR"
    "cp $TEMPDIR/RPMS/*/cyMUX-${CYMUX_VERSION}*.rpm $CURDIR"
    )

for cmd in "${cmds[@]}"; do
    echo $cmd >> $DEBUG
    if ! $cmd >> $DEBUG 2>&1; then
        echo Error executing "$cmd"...aborting
        exit 1
    fi
done

if [ ! -z "$BACKUP_MACRO" ]; then
    echo "Restoring $HOME/.rpmmacros" >> $DEBUG
    rm $HOME/.rpmmacros
    mv $BACKUP_MACRO $HOME/.rpmmacros
fi

echo "Cleaning up rpm build" >> $DEBUG
declare -a cmds=(
    "cd $CURDIR"
    "rm -rf $TEMPDIR"
)

for cmd in "${cmds[@]}"; do
    echo $cmd >> $DEBUG
    if ! $cmd >> $DEBUG 2>&1; then
        echo Error executing "$cmd"...aborting
        exit 1
    fi
done

exit 0
