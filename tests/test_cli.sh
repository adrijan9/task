#!/bin/sh
set -eu

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 /path/to/task"
    exit 2
fi

exe_input="$1"
if [ "${exe_input#"/"}" = "${exe_input}" ]; then
    exe_dir="$(cd "$(dirname "${exe_input}")" && pwd)"
    exe="${exe_dir}/$(basename "${exe_input}")"
else
    exe="${exe_input}"
fi

if [ ! -x "${exe}" ]; then
    echo "Executable not found or not executable: ${exe}"
    exit 2
fi

run_expect_fail() {
    status=0
    out="$("$@" 2>&1)" || status=$?
    if [ "${status}" -eq 0 ]; then
        echo "Expected failure but got success: $*" >&2
        exit 1
    fi
    printf '%s\n' "${out}"
}

tmp_dir="$(mktemp -d)"
cleanup() {
    rm -rf "${tmp_dir}"
}
trap cleanup EXIT

cd "${tmp_dir}"

echo "Test: help output"
"${exe}" -h | grep -q "Usage:"

echo "Test: create task"
"${exe}" -c "Extra task" >/dev/null
[ -d tasks ]

echo "Test: list tasks"
"${exe}" -a | grep -q "Task ID"
"${exe}" -a | grep -q "Extra task"

echo "Test: task id directory and TASK.md"
task_id="$("${exe}" -a | grep "Extra task" | head -n 1 | cut -d"|" -f2 | tr -d "[:space:]")"
[ -n "${task_id}" ]
[ -f "tasks/${task_id}/TASK.md" ]

echo "Test: preview task"
"${exe}" -p "${task_id}" | grep -q "STATUS: OPEN"
"${exe}" -p "${task_id}" | grep -q "Extra task"

echo "Test: delete task"
"${exe}" -d "${task_id}" >/dev/null
[ ! -d "tasks/${task_id}" ]
"${exe}" -a | grep -q "Task ID"
"${exe}" -a | grep -vq "Extra task"

echo "Test: delete invalid id"
out="$(run_expect_fail "${exe}" -d 00000000000000)"
printf '%s\n' "${out}" | grep -q "does not exists"

echo "Test: delete missing id"
out="$(run_expect_fail "${exe}" -d)"
printf '%s\n' "${out}" | grep -q "Usage: tasks -d"

echo "Test: preview invalid id"
out="$(run_expect_fail "${exe}" -p 00000000000000)"
printf '%s\n' "${out}" | grep -q "Error opening task file"

echo "Test: preview missing id"
out="$(run_expect_fail "${exe}" -p)"
printf '%s\n' "${out}" | grep -q "Usage: tasks -p"

echo "All tests passed."
