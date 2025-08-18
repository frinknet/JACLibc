/* © 2025 FRINKnet & Friends – MIT LICENSE */
((WASM, loadWASM) => 
  WASM.hash 
    ? loadWASM(WASM.hash.slice(1), WASM.search === "?secure") 
    : module?.exports = loadWASM
)(
  new URL(document.currentScript.src),
  (WASM, ENV) => WebAssembly.instantiateStreaming(
    fetch(`/wasm/${WASM}.wasm`),
    {
      env: Object.assign({}, ENV === true ? {} : ENV, {
        js: (
          ENV === true
            ? _ => null
            : (p, l, ...a) => {
              return Function(new TextDecoder().decode(new Uint8Array(WASM.exports.memory.buffer, p, l))
              ).apply(WASM, a);
            }
        )
      })
    }
  ).then(M => (
    WASM = M.instance,
    (ENV?.start?.bind(WASM) || WASM.exports._start)?.(),
    WASM
  ))
);
