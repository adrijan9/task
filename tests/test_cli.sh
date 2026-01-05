#!/bin/sh
set -eu

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 [/path/to/runner] /path/to/task"
    exit 2
fi

if [ "$#" -ge 2 ]; then
    runner="$1"
    exe_input="$2"
else
    runner=""
    exe_input="$1"
fi
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
if [ -n "${runner}" ]; then
    "${runner}" "${exe}" -h | grep -q "Usage:"
else
    "${exe}" -h | grep -q "Usage:"
fi

echo "Test: create task"
if [ -n "${runner}" ]; then
    "${runner}" "${exe}" -c "Extra task" >/dev/null
else
    "${exe}" -c "Extra task" >/dev/null
fi
[ -d tasks ]

echo "Test: list tasks"
if [ -n "${runner}" ]; then
    "${runner}" "${exe}" -a | grep -q "Task ID"
    "${runner}" "${exe}" -a | grep -q "Extra task"
else
    "${exe}" -a | grep -q "Task ID"
    "${exe}" -a | grep -q "Extra task"
fi

echo "Test: task id directory and TASK.md"
if [ -n "${runner}" ]; then
    task_id="$("${runner}" "${exe}" -a | grep "Extra task" | head -n 1 | cut -d"|" -f2 | tr -d "[:space:]")"
else
    task_id="$("${exe}" -a | grep "Extra task" | head -n 1 | cut -d"|" -f2 | tr -d "[:space:]")"
fi
[ -n "${task_id}" ]
[ -f "tasks/${task_id}/TASK.md" ]

echo "Test: preview task"
if [ -n "${runner}" ]; then
    "${runner}" "${exe}" -p "${task_id}" | grep -q "STATUS: OPEN"
    "${runner}" "${exe}" -p "${task_id}" | grep -q "Extra task"
else
    "${exe}" -p "${task_id}" | grep -q "STATUS: OPEN"
    "${exe}" -p "${task_id}" | grep -q "Extra task"
fi

echo "Test: delete task"
if [ -n "${runner}" ]; then
    "${runner}" "${exe}" -d "${task_id}" >/dev/null
else
    "${exe}" -d "${task_id}" >/dev/null
fi
[ ! -d "tasks/${task_id}" ]
"${exe}" -a | grep -q "Task ID"
"${exe}" -a | grep -vq "Extra task"

echo "Test: delete invalid id"
if [ -n "${runner}" ]; then
    out="$(run_expect_fail "${runner}" "${exe}" -d 00000000000000)"
else
    out="$(run_expect_fail "${exe}" -d 00000000000000)"
fi
printf '%s\n' "${out}" | grep -q "does not exists"

echo "Test: delete missing id"
if [ -n "${runner}" ]; then
    out="$(run_expect_fail "${runner}" "${exe}" -d)"
else
    out="$(run_expect_fail "${exe}" -d)"
fi
printf '%s\n' "${out}" | grep -q "Usage: tasks -d"

echo "Test: preview invalid id"
if [ -n "${runner}" ]; then
    out="$(run_expect_fail "${runner}" "${exe}" -p 00000000000000)"
else
    out="$(run_expect_fail "${exe}" -p 00000000000000)"
fi
printf '%s\n' "${out}" | grep -q "Error opening task file"

echo "Test: preview missing id"
if [ -n "${runner}" ]; then
    out="$(run_expect_fail "${runner}" "${exe}" -p)"
else
    out="$(run_expect_fail "${exe}" -p)"
fi
printf '%s\n' "${out}" | grep -q "Usage: tasks -p"

echo "All tests passed."
