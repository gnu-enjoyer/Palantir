FROM debian:bullseye AS base-image

RUN apt-get update
RUN useradd --system --create-home palantir

FROM gcc:11.2.0-bullseye as compile-image
RUN apt-get update
RUN apt-get install -y --no-install-recommends cmake pkg-config libssl-dev libhiredis-dev
RUN useradd --system --create-home palantir
USER palantir
WORKDIR /home/palantir

COPY --chown=palantir . .
RUN mkdir build
WORKDIR /home/palantir/build
RUN cmake ..
RUN make

FROM base-image AS production-image

USER palantir
WORKDIR /home/palantir

COPY --from=compile-image /home/palantir/build/Palantir /home/palantir/Palantir

ENTRYPOINT ["Palantir"]
