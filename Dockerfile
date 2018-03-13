FROM ubuntu:17.10

# Copy all the files over
COPY build/ /usr/src/cml/build
COPY build/lib /usr/src/cml/lib
COPY examples/data/ /usr/src/cml/examples/data/

WORKDIR /usr/src/cml
ENV LD_LIBRARY_PATH /usr/src/cml/lib

RUN ./build/bin/examples/sin_test

