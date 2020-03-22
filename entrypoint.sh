#!/bin/bash

: ${USER_ID:1000}
: ${IOTLAB_LOGIN}

HOME=/shared
USER_NAME=user

groupadd -g ${USER_ID} ${USER_NAME}
useradd --shell /bin/bash -u ${USER_ID} -o -c "" -d ${HOME} -m ${USER_NAME} -g ${USER_ID}
chown -R ${USER_ID}:${USER_ID} /shared

export HOME

BASHRC=${HOME}/.bashrc
if [ ! -f ${BASHRC} ]; then
    cp /root/.bashrc ${BASHRC}
    echo "export IOTLAB_LOGIN=${IOTLAB_LOGIN}" >> ${BASHRC}
fi

exec /usr/local/bin/gosu user /bin/bash "$@"
