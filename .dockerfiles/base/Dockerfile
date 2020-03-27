FROM python:3.7-slim

LABEL maintainer="alexandre.abadie@inria.fr"

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        bsdmainutils \
        build-essential \
        ca-certificates \
        curl \
        git \
        g++-multilib \
        openssh-client \
        python3-dev \
        python3-pip \
        python3-setuptools \
        python3-wheel \
        unzip \
        wget \
        xxd \
        && \
    apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Toolchain
RUN mkdir -p /opt && \
    curl -L -o /opt/gcc-arm-none-eabi.tar.bz2 'https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2?revision=bc2c96c0-14b5-4bb4-9f18-bceb4050fee7?product=GNU%20Arm%20Embedded%20Toolchain,64-bit,,Linux,7-2018-q2-update' && \
    echo '299ebd3f1c2c90930d28ab82e5d8d6c0 */opt/gcc-arm-none-eabi.tar.bz2' | md5sum -c && \
    tar -C /opt -jxf /opt/gcc-arm-none-eabi.tar.bz2 && \
    rm -f /opt/gcc-arm-none-eabi.tar.bz2 && \
    rm -rf /opt/gcc-arm-none-eabi-*/share/doc

ENV PATH ${PATH}:/opt/gcc-arm-none-eabi-7-2018-q2-update/bin

# Python packages
RUN pip3 install --no-cache \
    notebook \
    cbor \
    ed25519 \
    iotlabcli==3.1.1 \
    iotlabwscli==0.2.0 \
    jupyterlab==2.0.1
