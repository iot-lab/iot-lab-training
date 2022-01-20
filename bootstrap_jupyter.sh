#!/usr/bin/env bash

RIOT_ENV_FILE=/opt/riot.source
if [ -f ${RIOT_ENV_FILE} ]; then
    source ${RIOT_ENV_FILE}
fi

export SHELL=/bin/bash

jupyter-lab --no-browser --ServerApp.ip 0.0.0.0 --ServerApp.token=iotlab
