#!/usr/bin/env bash

if [ $# -eq 0 ]
  then
    echo "[ERROR] Provide path to mini-pl program as argument"
    echo "This script compiles a mini-pl program to WAT, runs wat2wasm, and serves the wasm env on localhost:8080/env.html"
    exit
fi

rm -r web
mkdir -p web
./build/mini-pl $1
pushd web
mv ../out.wat .
cp $OLDPWD/src/wasmlib/* .
wat2wasm out.wat
echo "Access wasm environment at http://localhost:8080/env.html"
python -m http.server 8080
popd
