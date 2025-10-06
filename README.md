![logo](logo.svg)

**JACLibc** \ˈja-kəl-ˌlib-sē\ ● *proper noun* — *fr. JACL, acron. "Jaunt Agile Compliant Library for C" ● cf. alt. "Javascript Accessible Composeable Linkage" (rare, chiefly in marketing) occasionally faux Old French "jacalibre" — nimble code escaping monstrous build systems...*

1. A lean, nimble, readable, performant, standards compliant HEADER ONLY libc.
2. Equally at home on desktop, mobile, edge compute, IoT and Browser Frontend.
3. Batteries included with a TEST FRAMEWORK and JavaScript Interchange Objects.
4. Designed to work seamlessly with ALL modern compilers without prejudice.
5. Builds WASM as a first class citizen and target BROWSER OR WASI as the OS.
6. Optimized memory model to fit into any environment constrained or massive.
7. Static inline for ultimate portability and simple tunable performance.

THINK: `dietlibc` meets `mewlibc` with `musl` philosophy targeting C standards and WASI compliance.

### The original JACLibc was forged somewhere between caffeine-drenched exasperation and irrepressible affection — with an inordinate amount of guidance from Perplexity Conversation and borderline ridiculous optimism. If this restless beast makes your life easier (or vaguely more amusing) send word in the comment. While replies may include obscure Unicode or unreasonably enthusiastic gratitude we do appreciate the support in any form. (Especially tests and bug reports...)

# Featured Innovations

The biggest innovation is `jsio.h`, unleashing bidirectional data sharing with JavaScript—akin to JSON, but with a sharper wit and a knack for moving real objects, not just string salad. While the realtime object thing is the main point, we still tossed in a JSON parser for when you need straight up JSON love. (SEE `js_parse()` and `js_stringify()`)  We also wanted to incorporate some of the stuff that we've learned from the reactive JS stuff we are still working on. So as the internal objects change we have a notify system that should just work where you can do `WASM.listen(path)` to get notified when that path works. In order to get super granular we made it RegEx friendly. — Although * always gets translated to .*. (We really need to figure that out a bit better...)

Async also gets its own love, too. Our lean Asyncify clone hands you `js_pause()`, `js_sleep()`, and `js_resume()`, so WASM can catch its breath mid-performance—all with zero extraneous compilers. Embedding JS works like Emscripten but with only one macro `JS_CODE` and it bakes into the WASM instead of spewing an HTML page full of vomit. Everything stays inside WASM, routed to a sleek JS eval loop. Flags in the loader let you toggle or mute this wizardry as the mood strikes. (You just need to pass it a JS runner function at `imports.env.js` that takes `js(code, len, ...args)`.

A simplified hosting environment borrowed from WACC is available in web if you want to stay away from the full WACC experience. The Docker dev environment and the `docker-compose.yaml` are also taken from there. While we detest generating HTML from compilers we realize the need to have a loader environment that works for SPAs especially. So we have a REALLY SIMPLE example of that for you. (One of these days we'll get the webpage up so that we can show you some demos...)

In addition, there is a fully optimized Testing Framework in `<testing.h>` that makes test driven development with AI a Piece of Cake. It dovetails nicely with METABUILD to create a truly magical CI/CD pipeline for C on any platform big or small. We are also working on other innovations that will help you build software faster and easier with traditional languages for a completely new build surface using the same toolkit for the entire gamut of targets.

Of course, the biggest innovation is that we achieved a true HEADER ONLY library for the entire libc without ducking features while approaching C89 to C23 compatibility which is truly unique among competing libc implementations. We've also consciously sidestepped as much prejudice as possible against baby compilers realizing that they are our future and we need more of them. We've tried our best to support standard features with polyfills and fallbacks so that you are NOT left behind. (Please open an issue if we left something out...)

# Today's Compatibility

Most of JACLibc is built for versatility to play nice no matter which OS + Compiler + Standard wizardry you conjure. Being HEADER ONLY with a 1k cap per file the hope is that this remains both easy to hack and extremely performant. Less code means less bugs and our commitment to transparent coding standards hopefully adds grace to this approach.

While `clang` remains our first choice for compilation, the `Metabuild Project` was created to give us a better build system to test against. Once our test framework is complete we will have CI/CD building against all targets with both extremely performant compilation and even more performant binaries. `GCC`, `TCC` and `XCC` are targeted with plans to add `chibicc` and a couple other smaller variants. Because we are a HEADER ONLY libc this makes us the perfect pairing with many of these fast small compilers.

For now we are targeting only major architectures X86, X64, ARM32, ARM64, RISC-V and WASM. We are also looking to target all OSes equally including Linux, BSD, MacOS, Windows, WASM and Web Browsers. Eventually, the goal is to target bare metal boots as well by adding the Cosmopolitan APE generation without relying on any behemoth compiler. But we realize doing HEADER ONLY the **link time optimization** pass of modern compilers is the great boon to our binary size and execution speed. There are several notes in the source where we are intentionally trusting the compiler.

Our long-term goal is to provide all of the elegance of small libc variants and special use libc (`nolibc`, `Cosmopolitan`, `uLibc` and `Musl` are wonderful case studies in how to eke out maximum performance...) This is meant to be more practical rather than academic but at the same time it is meant to be easy for anyone to dig deeper and understand how a system runs. Another goal is AI readability as we are learning that these pattern recognizers are very much garbage-in / garbage-out. It is very important to have a clean simple to read library for them to produce any sort of quality code. Code hygiene becomes more important than ever before.

# Usage Patterns

This is a drop in replacement for any libc. But being HEADER ONLY like STB, Miniaudio, or Dear ImGUI you have two sides both the general header and the IMPLEMENTATION which has traditionally been using an IMPLEMENTATION macro. That certainly works here. You can include the whole libc with a simple:


```C
#include <jaclibc.h>
```


And then where your main is you just declare JACL_MAIN and this will trigger the few global static variables that must be set in order for a libc backend implementation to run properly. There are notes inside that file explaining how this works and we do adhere heavily to the age-old philosophy "USE THE SOURCE LUKE!". Where possible we try to document INSIDE the source code instead of in a separate website. Grep is your friend. The codebase is simple...

The second way to use this is by including the headers like any other libc and then including the ONE .c file we provide `jaclib.c` in your final linking. This will again provide all the love to build what you need. While this file could technically be built as shared library, the nature of our library as inline is SO DIFFERENT that it would not work the way you expect. So everything that is produced is really static by design.

The third pattern, is using either the `<static.h>` header or at least reading it to see how the inclusion works. In this way you can cherry pick and build only what you need. Eventually, we hope to provide some build scripts similar to Cosmopolitan but much more lean and agnostic to compilers where possible. But for now WASM is our top priority until we can achieve full C89-23 coverage. Then we will go back and retrofit the lib for what is missing.

# Our WASM-First Mentality

It may be strange to many C coders that WASM is our first and primary target. This is not because we find others inferior but because we feel WASM is often neglected as a bastard child in the C world while the industry requires more and more WASM. We realize that the better you can define the center of a target the better you can meet the mark. WASM sits so beautifully between desktop and IoT that you really get the best for everyone if you target their architecture. But that is not to say there isn't problems with the WASM world.

Frankly, Emscripten is a behemoth with their clunky Python build system doing so many passes it would be easier to understand TypeScript compile errors. But really this is an outcry against the love of using 15 build systems to do the work that our old tried and true tools have been doing better with only a slightly larger learning curve. But the paradigm shift away from POSIX with WASM requires a recalibration of expectations. Interoperability with browser JS is difficult and the shims provided are to parse out the JS embedded in C and rebuild. (React, VueJS, and TypeScript do this bastardizing perfectly good languages trying to bolt-on the worse parts of other language...)

It was this realization that AI could not easily develop against Emscripten due to the complexity of using 4-5 languages working together to create a simple 1MB binary that really only needs to be about 5k. AI will learn your bloat IF you teach it your BLOATED STYLES. So we realize that WASM is the perfect candidate for this. Endeavor with the possibility of running a compiler in the browser like TCC or XCC. But the libc has to be a willing participant and currently we could not find any which fully embrace the BROWSER model. By using this weird abstraction and simultaneously targeting WASI, POSIX and Windows we achieve a nexus that we believe is the real sweet spot for modern C.

# The JavaScript Interchange Objects

JSIO is a structured format allowing for any basic JS object to be translated efficiently into C allowing for the ducktyping ambiguity that interpreted languages are famous for directly in C so that rich objects can be accessed from WASM. This includes the usual JSON types plus Functions, Maps and Sets. Additionally, it is important when programming C for the browser to be able to quickly and easily break free from the security JAIL that WASM naturally puts you in. So while the memory space is protected, we believe that it is often helpful for the rich JS API to be exposed for quick access.

> *`jsio.h` provides all of this without fuss. ● You can compile any target without much fuss. — There are a few gotchas but nothing like DEBUGGING EMSCRIPTEN!!! ● Compiling this way generates STRAIGHT WASM without generating weird webpages that you have to rewrite... ● There is a WASMloader JS and SPA template available. (both are extremely lean) ● If you want all the tooling you can use our `WebAssembly Central Control` framework — which is purpose built from JACLibc as a frontend development framework for C developers...*

> *While `jsio.h` is mostly WebAssembly turf — where JavaScript and C exchange awkward glances — the grand design still remains focused on a complete `stdlib` replacement for embedded use and daily driving on Desktops and Servers. We are committed to full cross-platform accessibility allowing users to roam wild. Wherever feasible, we've simplified and streamlined code to create slim binaries that could under any byte-sized door.*

We are now working on a full WASI compatibility layer so that you can use our lib to target both sides of the web. Much more is planned but not yet created yet. — Give it time... ● This is still very early and there is a lot of work in progress. In theory you should be able to compile directly against this with any C code and have it work. However, there are a few flags that you should use to make things work optimal.

# The Origin Story

The adventure started, as all good quests do, with a villain: Emscripten, towering and mythic, a toolchain so baroque it's rumored to feature secret boss levels. We traversed the wilderness of alternatives—MUSL, dietlibc, newlibc, WASI—each promising, yet none quite served up the "just right" blend of minimalism and compatibility. MinQND-libc tempted us with its clever compactness, but Goldilocks herself would've found it too cramped for serious WASM journeys. So, with a battle cry and a mild existential crisis, we rolled out our own, designed from the start to embrace the `WACC` ethos we were building for fast and minimal WASM compilation. Goldilocks would've pocketed it...

AI gave us the map, and our deadlines gave us the momentum. Perplexity saved us in many instances. In under a month, this upstart library took form—lean, sturdy, and shockingly cheerful for a project built in the wee hours. The caffeine wore off, but the code stayed sharp.

Now comes refinement—more polish, more capabilities, more adventures stacked atop the new foundation. Projects are brewing, contributions are trickling in, and the pace is just right for innovation and questionable memes. Pull up a chair, tip your hat, and enjoy the ride: JACL's story is only just beginning, and we saved you a front-row seat.

# Contributions and Legalities

Pull requests are welcome. However, we have a strict policy that you assign copyright to us as part of the contribution process. This creates something that is legally easier to manage than the usual list of contributors going out the wazoo. We've found in practice that list of emails is simply a SPAM trap. So there is a process in place with our GH repo that makes this more or less automatic. Your PR becomes a legally valid ESIGN of your consent to assign copyright to FRINKnet & Friends. If you have a problem with this policy feel free to fork the repo and avoid submitting the PR. If you've done something cool and you don't push it upstream we may ask you if you are interested in upstreaming later. But you have full power to say no. That's how the whole thing works with the MIT license system.

Where possible, we ask that your bug reports use the `<testing.h>` framework to write a UNIT TEST or case on when and how your test fails. That way we not only get a chance to fix your problem but the entire community benefits from stronger regression testing over time. This is an important part of a thriving ECOSYSTEM. While we will still try to get to your issues regardless, the more you contribute the easier it is for us to respond with a good solution.

# Financial Love

FRINKnet is a SMALL team. If you want to give us money we want to say THANK YOU. It frees up time to work on this more. If you want commercial support or consulting send a request via [https://frinknet.com/](https://frinknet.com/) and we will do our best to find a way to work together. Bragging rights of corporate support are also a possibility if that is of interest to you.
