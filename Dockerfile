FROM ubuntu:20.04 AS build

ARG VERSION=1.10.0

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        build-essential \
        wget 

RUN wget http://lloydrochester.com/code/as32-${VERSION}.tar.gz && \
    tar zxf as32-${VERSION}.tar.gz && \
    cd as32-${VERSION} && \
    ./configure && \
    make && \
    make install

FROM ubuntu:20.04

COPY --from=build /usr/local/bin/as32 /usr/local/bin/as32

ENTRYPOINT [ "/usr/local/bin/as32" ]