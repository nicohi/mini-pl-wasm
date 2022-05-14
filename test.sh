#!/usr/bin/env bash

pushd src/wasmlib/
wat2wasm wasmlib.wat
python -m http.server 8080
popd
