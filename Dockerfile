FROM dockcross/linux-arm64
ENV DEFAULT_DOCKCROSS_IMAGE linux-arm64

ARG OPENSSL_VER=3.2.1
ARG CURL_VER=8.7.1

RUN cd /opt/ && curl -LO https://openssl.org/source/openssl-${OPENSSL_VER}.tar.gz && tar -xzf openssl-*.tar.gz && rm openssl*.tar.gz \
&& cd openssl-* &&  CC= AR= ./Configure linux-aarch64 --prefix=/usr/xcc/aarch64-unknown-linux-gnu --openssldir=/usr/xcc/aarch64-unknown-linux-gnu \
&& make && make install && rm -rf /opt/openssl*

RUN cd /opt/ && curl -LO https://curl.se/download/curl-${CURL_VER}.tar.gz && tar -xzf curl-*.tar.gz \
    && rm curl-*.tar.gz && cd curl-* && \
    ./configure --with-openssl=/usr/xcc/aarch64-unknown-linux-gnu/ --prefix=/usr/xcc/aarch64-unknown-linux-gnu \
    --target=aarch64-unknown-linux-gnu --host=aarch64-unknown-linux --build=aarch64-unknown-linux-gnu \
    && make && make install