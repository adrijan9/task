# syntax=docker/dockerfile:1
FROM ubuntu:22.04

# ---- deps ----
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    pkg-config \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy everything into the image
COPY . .

# ---- build (CMake if available, otherwise simple gcc) ----
RUN set -eux; \
    rm -rf build; \
    if [ -f CMakeLists.txt ]; then \
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DDEBUG=ON; \
        cmake --build build -j; \
    else \
        gcc -O2 -Wall -Wextra -pedantic -std=c11 *.c -o app; \
    fi

# ---- run ----
# If CMake, try to run the first executable found in build/ (or use APP env)
ENV APP=""
CMD bash -lc '\
    if [ -n "$APP" ]; then \
        exec "$APP"; \
    fi; \
    if [ -x ./app ]; then \
        exec ./app; \
    fi; \
    exe="$(find ./build -maxdepth 2 -type f -executable ! -name "*.so*" ! -name "CMakeFiles" -print -quit)"; \
    if [ -n "$exe" ]; then exec "$exe"; fi; \
    echo "No runnable executable found. Set APP env, e.g.: docker run -e APP=./build/yourbin <image>"; \
    exit 1'