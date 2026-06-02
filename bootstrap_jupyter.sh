#!/usr/bin/env bash

RIOT_ENV_FILE=/opt/riot.source
if [ -f ${RIOT_ENV_FILE} ]; then
    source ${RIOT_ENV_FILE}
fi

export SHELL=/bin/bash

jupyter labextension enable jupyterlab_jupytext
jupyter labextension disable "@jupyterlab/apputils-extension:announcements"

jupyter-lab --no-browser --ServerApp.ip 0.0.0.0 --IdentityProvider.token=iotlab
