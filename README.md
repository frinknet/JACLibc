![logo](logo.svg)

**JACLibc** \ˈja-kəl-ˌlib-sē\ ● *proper noun* — *fr. JACL, acron. “Just Another C Library” ● cf. alt. “Javascript Accessible Composeable Linkage” (rare, chiefly in marketing) occasionally faux Old French “jacalibre” — nimble code escaping monstrous build systems...*

1. A lean C standard library for WASM — escape hatch for the Emscripten-averse.  
2. (colloq.) Sanctuary for compiling with `clang` — no build-script penance required.
3. Includes bootloader and SPA template.

***SEE ALSO:** For the full developer circus, see **WACC** crafted for JACL devotees...*

### This is a libc for WASM for those who lothe Emscripten and their build script monstrocity. ● You can compile without much fuss. — There are a few gotchas but not like DEBUGGING EMSCRIPTEN!!! ● This generates ONLY WASM not webpages, *THANK GOD!!!* ● There is a bootloader and SPA template available. ● If you want all the tooling crap you should use `WACC` — which is built on JACL...

# Compiling...

You should just be able to compile directly against this. However there are a few flags that you should use to make things work optimal.

The easiest thing to do is:

```bash
clang --nostdlib --include=path/to/jacl.h yourfile.c
```

However if you want a repeatable build we have the dockerfile and docker compose environments provided...

```bash
how to docker it...
```

# Compatability

Most of the libc should work regardless of what you do. obviously `jsio.h` is WASM teritory. However, we are trying to provide a full `stdlib` replacement so you should be able to do a lot.  Where possible we tried to use clang __builtins so that the resulting file would be as slim as can be. There are some flags for other compilers that have started making their way in but this is a long way from being ready to build with anything besides `clang`. Eventually I'd like to make it work with `XCC` although that means rewriting everything with builtins. We will probably get there eventually. `GCC` is not really on the horizon but I think most of the builtins should work. I just hate GNU bloat. `xcode` and `MSCC` may eventually make their way into the line up. But the next one is XCC for sure.

# Origin Story

JACL is a pet project born out of frustration and built with love and a lot of Perplexisty searches. If you like it, drop us a line and let us know....

It all started because Emscripten is a beast and we needed something that would work better with `WACC`. After looking at several including MUSL, dietlibc, newlibc, and WASI we concoluded it was better to do it from scratch. The actual inspiration comes from MinQND-libc using builtins. But it was too small so we felt like Goldie Locks looking for the poridge just right. Thankfully, with the advent of AI a small team can take on something major like a whole new libc. It took most of a month and Perplexity really saved the day more than once.

Within a few weeks we have the guts of this built out. While a lot of refinement is still happening we're starting to slow down to build projects on top of this.

# Innovations

The biggest innovation is the `jsio.h` allowing for bidirectional data sharing with Javascirpt similar in concept to JSON (We do include a parser at `js_parse` and `js_strigify`) but more focused on trading objects back and forth. We also wanted to incorporate some of the stuff that we've learned from the reactive JS stuff we are still working on. So as the internal objects change we have a notify system that whould just work where you can do `WASM.listen(path)` to get notified when that path works. In order to get super granular we made it `RegEx` friendly. Although * always gets translated to .*. (We really need to figure that out a bit better...)

We build a simplify Asyncify clone and that is a firstclass citizen `js_pause()` `js_sleep()` and `js_resume()` do those basics. We do all of this without needing to compile anything beyond the WASM. But you can still embed JS similar tto Emscripten it just doesn't turn into HTML Garbage by Python. Instead it stays embedded in the WASM adn gets passed to the one function that is imported by our loaded `js` which is a basic JS `eval` loop. We've also put flags in the loader tto turn that off.

A simplifified hosting environment borrowed from WACC is avaialble in web if you want to stay away from the full WACC experience. The Decker dev environent and the `docker-compose.yaml` are also taken from there. While we detest generating HTML from compilers we realize the need to have a loader environment that works for SPAs especially. S o we have a REALLY SIMPLE example of that for you. (One of these days we'll get the webpage up so that we can show you some demos...)

# Contributions

Pull requiests are welcome, but you will be assigning copyright to us. We don't maintain a list of contributours on the copyright. It's just FRINKnet and Friends. So consider your PR as a legally valid ESIGN that you are assigning copyright. If you have a problem with this fork and don't submit a PR. If you've done something cool and you don't push it upstream we may ask. But you have the power to say no. That's how the whole thing works.

