#!/bin/sh
set -eu

IMAGE_NAME="${IMAGE_NAME:-tasks-linux}"
DOCKERFILE="${DOCKERFILE:-linux_build.dockerfile}"

echo "Building ${DOCKERFILE}..."
docker build --no-cache -f "${DOCKERFILE}" -t "${IMAGE_NAME}" .

echo "Running linux tests in container..."
docker run --rm "${IMAGE_NAME}" /bin/sh -lc '
set -eu

if [ -x ./build/task ]; then
    exe=./build/task
elif [ -x ./build/tasks ]; then
    exe=./build/tasks
else
    exe="$(find ./build -maxdepth 2 -type f -executable -print -quit)"
fi

if [ -z "${exe}" ]; then
    echo "Executable not found in build/." >&2
    exit 1
fi

/bin/sh ./tests/test_cli.sh "${exe}"
'
