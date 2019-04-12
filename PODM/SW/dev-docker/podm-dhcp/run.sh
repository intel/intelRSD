#!/usr/bin/env bash
set -x
docker run --rm --env INTERFACES="eth0" -it podm-dhcp:0.1
set +x