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

Most of JACL’s libc is built for versatility: it’ll play nice no matter which WASM wizardry you conjure. Sure, `jsio.h` is strictly WebAssembly turf—where JavaScript and C exchange awkward glances—but the grand design remains a complete `stdlib` replacement, which means users can roam wild. Wherever feasible, we’ve hitched to clang’s `__builtins`, trimming the fat in pursuit of binaries so slim they could slip under a byte-sized door.

Other compilers have begun leaving scented flags at the perimeter, but, truth be told, this is still a one-compilation pony: best results come with `clang`. The horizon promises support for `XCC`—a true milestone, though it’ll require some re-knitting with builtins. Rome wasn’t built in a day, and neither was JACL’s compiler compatibility. `GCC` remains a distant mirage (never mind my aversion to GNU’s spontaneous bloating syndrome), but most builtins should theoretically play along.

As for `xcode`, `MSCC`, and other characters from the rogue’s gallery, perhaps they’ll join the jamboree in a future season. But for now, `XCC` is next on our dance card—after that, who knows? Maybe JACL can become the Switzerland of C libraries, open to all, but fiercely neutral about GNU baggage.

# Origin Story

JACL was forged somewhere between caffeine-drenched exasperation and irrepressible affection—with an inordinate amount of guidance from Perplexity searches and borderline ridiculous optimism. If this restless beast makes your life easier (or vaguely more amusing), send word; replies may include obscure Unicode or unreasonably enthusiastic gratitude.

The adventure started, as all good quests do, with a villain: Emscripten, towering and mythic, a toolchain so baroque it’s rumored to feature secret boss levels. We traversed the wilderness of alternatives—MUSL, dietlibc, newlibc, WASI—each promising, yet none quite served up the “just right” blend of minimalism and compatibility. MinQND-libc tempted us with its clever compactness, but Goldilocks herself would’ve found it too cramped for serious WASM journeys. So, with a battle cry and a mild existential crisis, we rolled out our own, designed from the start to embrace the `WACC` ethos we were building for fast and minimal WASM compilation. Goldilocks would’ve pocketed it...

AI gave us the map, and our deadlines gave us the momentum. Perplexity saved us in many instances. In under a month, this upstart library took form—lean, sturdy, and shockingly cheerful for a project built in the wee hours. The caffeine wore off, but the code stayed sharp.

Now comes refinement—more polish, more capabilities, more adventures stacked atop the new foundation. Projects are brewing, contributions are trickling in, and the pace is just right for innovation and questionable memes. Pull up a chair, tip your hat, and enjoy the ride: JACL’s story is only just beginning, and we saved you a front-row seat.

# Innovations

The biggest innovation is `jsio.h`, unleashing bidirectional data sharing with JavaScript—akin to JSON, but with a sharper wit and a knack for moving real objects, not just string salad. While the realtime object thing is the  main point, we still tossed in a JSON parser for when you need straight up JSON love. (SEE `js_parse()` and `js_stringify()`)  We also wanted to incorporate some of the stuff that we've learned from the reactive JS stuff we are still working on. So as the internal objects change we have a notify system that whould just work where you can do `WASM.listen(path)` to get notified when that path works. In order to get super granular we made it RegEx friendly. — Although * always gets translated to .*. (We really need to figure that out a bit better...)

Async got its own love, too. Our lean Asyncify clone hands you `js_pause()`, `js_sleep()`, and `js_resume()`, so WASM can catch its breath mid-performance—all with zero extraneous compilers. Embedding JS works like Emscripten but with only one macro `JS_CODE` and it bakes  into the  WASM instead of spewing an HTML page full of vomit. Everything stays inside WASM, routed to a sleek JS eval loop. Flags in the loader let you toggle or mute this wizardry as the mood strikes. (You just need to pass it a JS runner function at `imports.env.js` that takes `js(code, len, ...args)`.

A simplifified hosting environment borrowed from WACC is avaialble in web if you want to stay away from the full WACC experience. The Docker dev environent and the `docker-compose.yaml` are also taken from there. While we detest generating HTML from compilers we realize the need to have a loader environment that works for SPAs especially. So we have a REALLY SIMPLE example of that for you. (One of these days we'll get the webpage up so that we can show you some demos...)

# Contributions

Pull requiests are welcome, but you will be assigning copyright to us. We don't maintain a list of contributours on the copyright. It's just FRINKnet and Friends. So consider your PR as a legally valid ESIGN that you are assigning copyright. If you have a problem with this fork and don't submit a PR. If you've done something cool and you don't push it upstream we may ask. But you have the power to say no. That's how the whole thing works.


