FROM jupyter/base-notebook:python-3.7.6

LABEL maintainer="alexandre.abadie@inria.fr"

USER root

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        add-apt-key \
        bsdmainutils \
        build-essential \
        curl \
        git \
        g++-multilib \
        iproute2 \
        mosquitto-clients \
        net-tools \
        openssh-client \
        socat \
        unzip \
        vim \
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

RUN curl -L -o /usr/local/bin/gosu "https://github.com/tianon/gosu/releases/download/1.4/gosu-$(dpkg --print-architecture)" \
    && curl -L -o /usr/local/bin/gosu.asc "https://github.com/tianon/gosu/releases/download/1.4/gosu-$(dpkg --print-architecture).asc" \
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
RUN python3 -m pip install --no-cache \
    aiocoap==0.3 \
    asynchttp==0.0.4 \
    asyncssh==2.2.0 \
    azure-iot-device==2.1.1 \
    cbor==1.0.0 \
    ed25519==1.5 \
    iotlabcli==3.1.1 \
    iotlabwscli==0.2.0 \
    ipympl==0.5.6 \
    ipywidgets==7.5.1 \
    jupyterlab==2.0.1 \
    matplotlib==3.2.1 \
    numpy==1.18.2 \
    paho-mqtt==1.5.0 \
    pandas==1.0.3 \
    pycayennelpp==1.3.0 \
    python-cayennelpp==0.0.4 \
    scapy==2.4.3 \
    seaborn==0.10.0 \
    scikit-learn==0.22.2 \
    scipy==1.4.1

# IoT-LAB CLI Tools
RUN python3 -m pip install -U --no-cache git+https://github.com/aabadie/cli-tools.git@ssh_key

# IoT-LAB Plot OML tools
RUN python3 -m pip install -U --no-cache git+https://github.com/iot-lab/oml-plot-tools.git@0.7.0

# Add Jupyterlab interactive extensions
RUN conda install -c conda-forge nodejs
RUN jupyter labextension install @jupyter-widgets/jupyterlab-manager
RUN jupyter labextension install jupyter-matplotlib
RUN jupyter nbextension enable --py widgetsnbextension

ADD bootstrap_jupyter.sh /bootstrap_jupyter.sh
RUN chmod +x /bootstrap_jupyter.sh

ADD entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

EXPOSE 8888

WORKDIR /shared

ENTRYPOINT ["/entrypoint.sh"]

CMD ["/bootstrap_jupyter.sh"]
