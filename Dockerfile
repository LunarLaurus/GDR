FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    libsdl2-dev \
    libsdl2-mixer-dev \
    libsdl2-net-dev \
    libpng-dev \
    libfluidsynth-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY . .

RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . -j$(nproc)

FROM ubuntu:22.04
COPY --from=0 /build/src/goblin-doom /usr/local/bin/
COPY --from=0 /build/src/goblin-setup /usr/local/bin/
COPY --from=0 /build/src/goblin-server /usr/local/bin/

ENTRYPOINT ["goblin-doom"]
