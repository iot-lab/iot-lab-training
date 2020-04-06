#!/bin/bash

: ${USER_ID:1000}

USERNAME=jovyan
HOME=/home/${USERNAME}

usermod -u ${USER_ID} ${USERNAME}
chown -R ${USER_ID}:${USER_ID} ${HOME}

export HOME

exec /usr/local/bin/gosu ${USERNAME} /bin/bash "$@"
