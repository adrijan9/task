#!/bin/sh
set -eu

IMAGE_NAME="${IMAGE_NAME:-tasks-windows}"
DOCKERFILE="${DOCKERFILE:-windows_test.dockerfile}"

echo "Building ${DOCKERFILE}..."
docker build --platform=linux/amd64 --no-cache -f "${DOCKERFILE}" -t "${IMAGE_NAME}" .

echo "Running windows tests in container..."
docker run --platform=linux/amd64 --rm "${IMAGE_NAME}" /bin/sh -lc '
set -eu

if [ -f ./app.exe ]; then
    exe=./app.exe
else
    exe="$(find ./build-win -maxdepth 3 -type f -iname "*.exe" -print -quit)"
fi

if [ -z "${exe}" ]; then
    echo "Executable not found in build-win/." >&2
    exit 1
fi

wine_cmd=wine
if command -v wine64 >/dev/null 2>&1; then
    wine_cmd=wine64
fi

WINEPREFIX="/tmp/wine"
WINEARCH="win64"

wineboot_cmd=wineboot
if ! command -v wineboot >/dev/null 2>&1; then
    wineboot_cmd=wineboot64
fi

WINEDEBUG="-all" WINEPREFIX="${WINEPREFIX}" WINEARCH="${WINEARCH}" xvfb-run -a "${wineboot_cmd}" -u

cat > /tmp/run_wine.sh <<EOF
#!/bin/sh
set -eu
WINEDEBUG="\${WINEDEBUG:--all}" WINEPREFIX="\${WINEPREFIX:-${WINEPREFIX}}" WINEARCH="\${WINEARCH:-${WINEARCH}}" xvfb-run -a ${wine_cmd} "\$@"
EOF
chmod +x /tmp/run_wine.sh

/bin/sh ./tests/test_cli.sh /tmp/run_wine.sh "${exe}"
'
