FROM ubuntu:18.04

LABEL maintainer="alexandre.abadie@inria.fr"

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        add-apt-key \
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

# gosu
RUN export KEY=B42F6819007F00F88E364FD4036A9C25BF357DD4; \
    for server in $(shuf -e ha.pool.sks-keyservers.net \
                            hkp://p80.pool.sks-keyservers.net:80 \
                            keyserver.ubuntu.com \
                            hkp://keyserver.ubuntu.com:80 \
                            keyserver.pgp.com \
                            pgp.mit.edu) ; do \
        gpg --batch --keyserver "$server" --recv-keys $KEY && break || : ; \
    done;

RUN curl -o /usr/local/bin/gosu -SL "https://github.com/tianon/gosu/releases/download/1.4/gosu-$(dpkg --print-architecture)" \
    && curl -o /usr/local/bin/gosu.asc -SL "https://github.com/tianon/gosu/releases/download/1.4/gosu-$(dpkg --print-architecture).asc" \
    && gpg --verify /usr/local/bin/gosu.asc \
    && rm /usr/local/bin/gosu.asc \
    && chmod +x /usr/local/bin/gosu

# Toolchain
RUN mkdir -p /opt && \
    curl -L -o /opt/gcc-arm-none-eabi.tar.bz2 'https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2?revision=bc2c96c0-14b5-4bb4-9f18-bceb4050fee7?product=GNU%20Arm%20Embedded%20Toolchain,64-bit,,Linux,7-2018-q2-update' && \
    echo '299ebd3f1c2c90930d28ab82e5d8d6c0 */opt/gcc-arm-none-eabi.tar.bz2' | md5sum -c && \
    tar -C /opt -jxf /opt/gcc-arm-none-eabi.tar.bz2 && \
    rm -f /opt/gcc-arm-none-eabi.tar.bz2 && \
    rm -rf /opt/gcc-arm-none-eabi-*/share/doc

ENV PATH ${PATH}:/opt/gcc-arm-none-eabi-7-2018-q2-update/bin

# Python packages
RUN pip3 install \
    cbor \
    ed25519 \
    iotlabcli==3.1.0 \
    iotlabwscli==0.2.0 \
    jupyterlab==2.0.1

ADD bootstrap_jupyter.sh /bootstrap_jupyter.sh
RUN chmod +x /bootstrap_jupyter.sh

ADD entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

EXPOSE 8888

WORKDIR /shared

ENTRYPOINT ["/entrypoint.sh"]

CMD ["/bootstrap_jupyter.sh"]
