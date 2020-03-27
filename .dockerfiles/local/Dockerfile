FROM aabadie/iot-lab-training-base:latest

LABEL maintainer="alexandre.abadie@inria.fr"

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        add-apt-key \
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

ADD run.sh /run.sh
RUN chmod +x /run.sh

ADD entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

EXPOSE 8888

WORKDIR /shared

ENTRYPOINT ["/entrypoint.sh"]

CMD ["/run.sh"]
