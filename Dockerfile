FROM debian:bookworm

ARG OPENSSL_VER=bookworm
ARG CURL_VER=bookworm-backports

RUN apt-get update \
    && apt-get -y install crossbuild-essential-arm64 cmake curl python3 git \
    && apt-get clean \
    && rm -rf /var/cache/apt/archives/* \
    && rm -rf /var/lib/apt/lists/*

ENV CC=aarch64-linux-gnu-gcc
ENV CXX=aarch64-linux-gnu-g++
ENV AR=aarch64-linux-gnu-ar
ENV LD=aarch64-linux-gnu-ld

RUN cd /opt/ && curl -LO http://deb.debian.org/debian/pool/main/z/zlib/zlib_1.3.dfsg+really1.3.1.orig.tar.gz && tar -xzf zlib*.tar.gz && rm zlib*.tar.gz \
&& cd zlib* && ./configure && make && make install prefix=/usr/aarch64-linux-gnu && rm -rf /opt/zlib*

RUN cd /opt/ && curl -LO https://salsa.debian.org/debian/openssl/-/archive/debian/${OPENSSL_VER}/openssl-debian-${OPENSSL_VER}.tar.gz && tar -xzf openssl-*.tar.gz && rm openssl*.tar.gz \
&& cd openssl-* &&  ./Configure linux-aarch64 --prefix=/usr/aarch64-linux-gnu/ --openssldir=/usr/aarch64-linux-gnu/ \
&& make && make install_sw && rm -rf /opt/openssl*

RUN cd /opt/ && curl -LO https://github.com/besser82/libxcrypt/releases/download/v4.4.38/libxcrypt-4.4.38.tar.xz && tar -xf libxcrypt*.tar.xz && rm libxcrypt*.tar.xz \
&& cd libxcrypt-* &&  ./configure --prefix=/usr/aarch64-linux-gnu --host aarch64-linux-gnu \
&& make && make install && rm -rf /opt/libxcrypt*

COPY cross.cmake /opt/
ENV CMAKE_TOOLCHAIN_FILE=/opt/cross.cmake

RUN cd /opt/ && curl -LO https://salsa.debian.org/debian/curl/-/archive/debian/${CURL_VER}/curl-debian-${CURL_VER}.tar.gz && tar -xzf curl-*.tar.gz \
 && rm curl-*.tar.gz && cd curl-* && cmake -B build -DCURL_USE_LIBPSL=OFF && cmake --build build && cmake --install build --prefix /usr/aarch64-linux-gnu

WORKDIR /work
