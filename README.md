# mini-pl-wasm
A webassembly compiler for a simple Pascal based programming language.
Extended from [mini-pl-interpreter](https://github.com/nicohi/mini-pl-interpreter).
Made for the course *Code generation (CSM14205)*. 

## Building
To build the project run:
`./build.sh`

To build the project run:
`./build.sh`
### Requirements
- cmake
- gcc
- (python3)

## Running in browser
`python -m http.server 8080`
or 
`python3 -m http.server 8080`

## Usage
Any valid minipl program can be interpreted with:
`./build/mini-pl [filename].`
For testing purposes, the user can use
`./build/mini-pl -s [filename]`
to run merely the scanner, or 
`./build/mini-pl -p [filename]`
to run the scanner+parser.
Both commands print a readable result.

Example programs are provided in `./test/`.
