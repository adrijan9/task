# syntax=docker/dockerfile:1
FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

# ---- deps ----
RUN dpkg --add-architecture i386 && apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    build-essential \
    cmake \
    pkg-config \
    mingw-w64 \
    wine64 \
    wine32 \
    wine64-tools \
    xvfb \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy everything into the image
COPY . .

# Clean any host build artifacts that might be copied in
RUN rm -rf build build-win CMakeCache.txt CMakeFiles || true

# ---- build Windows exe (CMake if available, otherwise simple mingw gcc) ----
RUN set -eux; \
    if [ -f CMakeLists.txt ]; then \
        cmake -S . -B build-win \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_SYSTEM_NAME=Windows \
            -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
            -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ ; \
        cmake --build build-win -j; \
    else \
        x86_64-w64-mingw32-gcc -O2 -Wall -Wextra -pedantic -std=c11 *.c -o app.exe; \
    fi

# ---- run ----
# Set APP to run a specific .exe (otherwise it will try app.exe then first .exe in build-win/)
ENV APP=""
CMD ["bash","-lc", "\
    if [ -n \"$APP\" ]; then exec wine64 \"$APP\"; fi; \
    if [ -f ./app.exe ]; then exec wine64 ./app.exe; fi; \
    exe=\"$(find ./build-win -maxdepth 3 -type f -iname '*.exe' -print -quit)\"; \
    if [ -n \"$exe\" ]; then exec wine64 \"$exe\"; fi; \
    echo \"No .exe found. Set APP, e.g.: docker run -e APP=build-win/Your.exe <image>\"; \
    exit 1 \
"]
