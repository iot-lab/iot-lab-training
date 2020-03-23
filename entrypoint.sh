#!/bin/bash

: ${USER_ID:1000}

HOME=/shared
USER_NAME=user

groupadd -g ${USER_ID} ${USER_NAME}
useradd --shell /bin/bash -u ${USER_ID} -o -c "" -d ${HOME} -m ${USER_NAME} -g ${USER_ID}
chown -R ${USER_ID}:${USER_ID} /shared

export HOME

exec /usr/local/bin/gosu user /bin/bash "$@"
