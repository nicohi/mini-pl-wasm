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

var mp = 0;
function malloc(size) {
  p = mp + 1;
  mp = mp + size() + 1;
  return p;
}

var memory = new WebAssembly.Memory({initial:10});
function consoleLogString(offset, length) {
  var bytes = new Uint8Array(memory.buffer, offset, length);
  var string = new TextDecoder('utf8').decode(bytes);
  console.log(string);
};


function toString(x) {
  return(x+"");
};

var importObject = {
    console: {
      log: consoleLogString
    },
    js: {
        memory: memory
    },
    math: {
        add: (a,b) => a+b,
        sub: (a,b) => a-b,
        mul: (a,b) => a*b,
        div: (a,b) => a/b,
        mod: (a,b) => a%b,
        eq: (a,b) => a==b,
        neq: (a,b) => a!=b,
        lt: (a,b) => a<b,
        gt: (a,b) => a>b,
        lte: (a,b) => a<=b,
        gte: (a,b) => a>=b,
        not: (a) => !a,
        or: (a) => a || b,
        and: (a) => a && b
    }
};

fetch('out.wasm').then(response =>
    response.arrayBuffer()
).then(bytes =>
    WebAssembly.instantiate(bytes, importObject)

).then(result => {
    // result.instance.exports.exported_func()

    //memory = result.instance.exports.memory;
    result.instance.exports.main();

    var button = document.getElementById("expand");
    button.onclick = function() {
        try {
            memory.grow(1);
            showDetails(memory);
        } catch(re) {
            alert("You cannot grow the Memory any more!");
            console.log(re);
        };
    };
    // var button2 = document.getElementById("logmem");
    // button2.onclick = function() {
    //     try {
    //       result.instance.exports.print(0,10);
    //     } catch(re) {
    //         console.log(re);
    //     };
    // };
    showDetails(memory);
});
