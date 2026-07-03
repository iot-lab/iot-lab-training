#!/bin/bash

: ${USER_ID:1000}

USERNAME=jovyan
HOME=/home/${USERNAME}

usermod -u ${USER_ID} ${USERNAME}

# Restore the Rust toolchain in the home directory when it is hidden by a
# volume mounted over /home/jovyan
[ -d ${HOME}/.cargo ] || cp -a /opt/rust-skel/.cargo ${HOME}/
[ -d ${HOME}/.rustup ] || cp -a /opt/rust-skel/.rustup ${HOME}/

chown -R ${USER_ID}:${USER_ID} ${HOME}

export HOME

exec /usr/local/bin/gosu ${USERNAME} /bin/bash "$@"
