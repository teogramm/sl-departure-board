FROM dockcross/linux-arm64
ENV DEFAULT_DOCKCROSS_IMAGE linux-arm64

ARG OPENSSL_VER=bookworm
ARG CURL_VER=bookworm-backports

RUN cd /opt/ && curl -LO https://salsa.debian.org/debian/openssl/-/archive/debian/${OPENSSL_VER}/openssl-debian-${OPENSSL_VER}.tar.gz && tar -xzf openssl-*.tar.gz && rm openssl*.tar.gz \
&& cd openssl-* &&  CC= AR= ./Configure linux-aarch64 --prefix=/usr/xcc/aarch64-unknown-linux-gnu --openssldir=/usr/xcc/aarch64-unknown-linux-gnu \
&& make && make install_sw && rm -rf /opt/openssl*

RUN cd /opt/ && curl -LO https://salsa.debian.org/debian/curl/-/archive/debian/${CURL_VER}/curl-debian-${CURL_VER}.tar.gz && tar -xzf curl-*.tar.gz \
    && rm curl-*.tar.gz && cd curl-* && ./configure --help &&\
    ./configure --with-openssl=/usr/xcc/aarch64-unknown-linux-gnu/ --prefix=/usr/xcc/aarch64-unknown-linux-gnu \
    --target=aarch64-unknown-linux-gnu --host=aarch64-unknown-linux --build=aarch64-unknown-linux-gnu  --without-libpsl \
    --disable-alt-svc --disable-ares --disable-kerberos-auth --disable-negotiate-auth --disable-aws --disable-manual \
    --disable-mime --disable-netrc --disable-ntlm --disable-ntlm-wb --disable-progress-meter disable-tls-srp  \
    --disable-unix-sockets --enable-symbol-hiding --disable-ftp --disable-file --disable-ldap --disable-ldaps  \
    --disable-rtsp --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smb \
    --disable-gopher --disable-mqtt --disable-docs --disable-smtp --disable-smtps \
    && make && make install