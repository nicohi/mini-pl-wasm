function showDetails(mem) {
    var buf = mem.buffer;
    var memEl = document.getElementById('mem');
    var pagesEl = document.getElementById('pages');
    var firstIntEl = document.getElementById('firstint');
    var firstBytesEl = document.getElementById('firstbytes');

    memEl.innerText=buf.byteLength;
    pagesEl.innerText=buf.byteLength / 65536;

    var i32 = new Uint32Array(buf);
    var u8 = new Uint8Array(buf);

    firstIntEl.innerText=i32[0];
    firstBytesEl.innerText= "[" + u8[0] + "," + u8[1] + "," +
        u8[2] + "," + u8[3] + "]";
};

function consoleLogString(offset, length) {
  var bytes = new Uint8Array(memory.buffer, offset, length);
  var string = new TextDecoder('utf8').decode(bytes);
  console.log(string);
};

var mp = 0;
var mem = 0;
function malloc(size) {
  
}

function toString(x) {
  return(x+"");
};

var importObject = {
    imports: {
      console: {
        log: consoleLogString
      }
    }
};

fetch('wasmlib.wasm').then(response =>
    response.arrayBuffer()
).then(bytes =>
    WebAssembly.instantiate(bytes, importObject)

).then(result => {
    // result.instance.exports.exported_func()

    var mem = result.instance.exports.memory;

    var button = document.getElementById("expand");
    button.onclick = function() {
        try {
            mem.grow(1);
            showDetails(mem);
        } catch(re) {
            alert("You cannot grow the Memory any more!");
            console.log(re);
        };
    };
    var button2 = document.getElementById("logmem");
    button2.onclick = function() {
        try {
          result.instance.exports.print(0,10);
        } catch(re) {
            console.log(re);
        };
    };
    showDetails(mem);
});
