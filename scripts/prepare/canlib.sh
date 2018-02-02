#!/bin/bash
ROOT="${0%/*}/../.."

cd "${ROOT}/lib/CANlib/generator"

PY_ARGS="main.py \"../../../can_spec_my18.yml\""

declare -a py_envs=("python3" "py" "python")

for py_pathenv in "${py_envs[@]}"
do
    if py_env=$(command -v "${py_pathenv}")
    then
        echo "Generating CANlib with Python 3 found in ${py_env}..."
        eval "${py_env} ${PY_ARGS}"
        exit $?
    fi
done

echo "No Python 3 interpreter found!"

exit 1
