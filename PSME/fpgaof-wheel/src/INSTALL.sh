#!/usr/bin/env bash

OPAE_DIR="/etc/opae"
sudo -H pip install FPGAoF_Wheel*

if [ ! -d "$OPAE_DIR" ]; then
  mkdir "$OPAE_DIR"
fi

sudo touch "$OPAE_DIR/discovery_service.cfg"
sudo chmod ugo+rw "$OPAE_DIR/discovery_service.cfg"