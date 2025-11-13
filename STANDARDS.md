# STANDARDS TO MAINTAIN COMPLIANCE

JACLibc is intended to be *extremely composable* allowing anything from microcontrollers to supercomputers to use it so  that programmers and AIs can code to a single standard that just works. This is a daunting task because the language evolves even more  than the C Standard eveolves. This Document explains WHY and HOW the library maintains STANDARDS for itself to allow coding in C to flourish in the modern world.

As a general rule, functions and macros beyond the selected C Standard are removed through fencing but scalar types are available regardless. POSIX compatability is ALWAYS binned to the latest POSIX as their standards system is more dificult to fence. We are working towards  complete POSIX compatability with the goal of eventiually providing full POSIX on Windows as well so everything just works.

In feature selection we try our best to make the entire library self configuring by use of the single `<config.h>` header. This is included in every header of the library. The allows you to use virtually any compiler and generate code tailored to their unique capabilities. While there are many annomalies you will only find then far off the beaten path and they can usually be corrected by a simple PR to change an ifdef.

## POLIFILLS

All language should be written in  the most modern style. Any polyfills needed to make that happen should live in `<counfig.h>` ONLY. The problem with polyfills is then need to be included to be useful. `<config.h>` is includeded in every header of the library so modern syntax is guaranteed as a byproduct. This provides a benefit for any*o*ne needing to compile on an older system that they can code with C23 syntax and the code will still work beaultifully.

NOTE: __The only know annomaly to this is `long long` which cannot be polyfilled because it is two words. While this is unfortunate, it is a limitation of the preprocessor language. This ONLY affect C89 code before `long long` was standardized. However, the compiler should complain if you use `long long` in a restrained environment. But our code should work fine because we are version fenced for everything.__

## FENCING

We have several fencing systems in place mostly found in `<config.h>`. the `JACL_BITS` macro and numbered bits macros are used to notify the build system what bitscape exists in memory. In adition, `JACL_INTMAX_BITS` shows the max integers available and 'JACL_LDBL_BITS` shows the bitdepth for the floating point system. Doubles are always considered 64bitt per IEEE 754 specification. We expect the complier to throw errors if you are working on an MCU that is oppinionated otherwise but they must be weird and probably need their own libc if that is the case.

In addtion to bitdepth fencing we also have a few feature test fences. These all have the syntax `JACL_HAS_PTHREAD`, `JACL_HAS_POSIX`, `JACL_HAS_IMMINTRIN`, `JACL_HAS_INT32`, `JACL_HAS_INT64`, `JACL_HAS_LFS`. This pattern is also applies to

## TYPES

As a general rule we DO NOT FENCE TYPES. If a type is viable in that C version it is allowed to exist. The notable exception is `long long` and its derivitives because of the above mentioned linguistic problems. The reason for allowing types regardless is to make our codebase smaller. For exampe, `<core/format.h>` uses `unintmax_t` extensively in order to make things just work. While the actual definition of `uintmax_t` may change depending on system capabilities, the existence of the type should be guaranteed for us. This also means that when you want guaranteed bitsite for integers all of the C99 types are available even in C89 style on constrained systems. This may be of help in creating better self-documenting code.

One other place where we are inconsistent with this is in struct types. If the struct is ONLY used in fenced functions then we will fence it with `JACL_HAS_Cxx` checks. This is so the types are not used outside of their inteneded scope. If you want an identical type you should build it yourself and ifdef as needed. You can use the `JACL_HAS_INT32` or `JACL_HAS_INT64` if your code needs to follow our types more closely. But we suggest using more generic checks that we use to populate these.

## FUNCTIONS

Functions are always version fenced with the exception of internal functions which exist below the end user API. Most functions are `static inline` by design. This means that most of your code will be pure operational bytes with many less function calls. This is ideal for Link Time Optimization which has shown to be both simpler and more compact than code level hacks used by compilers of yester-year. There are a select few functions including memory and IO functions that have been centralized in the core. However, in most places where possible we put these functions in the forward headers instead of the core.

## MACROS

Macros are selectively version fenced on a case by case basis. Nan and infinity checks are universally allowed even though they didn't exist in C89. Conversely many of the helper macros for more modern systems are version fenced. The general rule is whether the macro is needed by the library internals. If the macro is only useful for external API use it is fenced but when it is needed by internals we include it since we try to maintain internals with modern code through polyfills. So those required  by internals are considered to be polyfill rather than anachronisms.

## STYLE

Whenever possible we try to be concise and do things on one line. The general rule is when a Control Structure can be expressed on one line it should not use two. Conversely when a Control Structure needs many lines it should not be collapsed to one. The only cases where this is not true is in extremely simple functions less than 5 lines long where the logic has bee thoroughly tested and the confidence is high that it will NEVER cause segfault or error. Also, you will find this in the math library where it is highly macro driven both  to save space and to keep algorythms consistent between types.

We are festidious about vertical whitespace. You MOST include a blank line between diferent types of opperations. Control Structures are seaprated from assignments. Assignments are separate from declaration. We consider these necessary because it symbolizes a context shift in verbal purpose. Declaration create variables as a matterof fact. Assignments set values as a matter of movement. Control Structures always show proccess logic.

When possible we, condense the code using macros. This is seen extensively both in `<math.h>` and `<format.h>`. We avoid using function literals ({}) since they are GCC specific. Instead, we generally use the `do {} while(0)` pattern that you find in most proffessional level C code. Internal variables for these macros should always be prefixed with a double underscore to avoid colission in the parent code. When and why we use these is highly subjective. But in general the DRY principal (Don't Repeat Yourself) is our leading guide in this entire library.

# SPACING

INLINE CODE: All mathmatical perations MUST include sapce on both sizes of the operator for maximum readability. When condensing lines the semicolon MUST be followed by a space as should curly braces on condensed lines. Likewise, commas should ALWAYS be followed by a space. Macros defining functions MUST put the function declaration on the SAME LINE as the definition syntax.G

CONTROL STRUCTURE: Control Structures should be TIGHT in that the space is before and after the parenthesis between the calling name and the open parenthesis and the end parenthesis and the open curly. If no curly, you should still have a space after the closing parenthesis. If the Control Structure has no body the curlies may be ommited like in the case of for or while loop having a semicolon instead of body. the `while` of a `do/while` is the ONLY excection to these rules because always TIGHT against the parenthesis without space to visually differentiate from while.

FUNCTION DEFINITIONS: Functions MUST include the opening curly on the same line following the arguments. Functions are the same as control structures with TIGHT parenthesis but the function name is always tight to the parenthesis WITHOUT space. Funtion MAY  be oneliners if less than  5 lines total. But generally anything larger should be expanded vertically. TEST functions must ALWAYS be expanded vertically for  maximum  readability.

INDENTATION: `#ifdef` bodies MUST be indented unless it surrounds a function. When surrounding functions, the `#ifdef` MUST be followed by a new line and the following '#endif` MUST be preceeded by a space. `#elif` and `#else` follow the same idelogy. Functions, and Control Structures MUST intent a single tab for their body. However, `switch` DOES NOT indend and `case` MUST align to the switch above.

ALLIGNMENT: Variables and macros wich correspond MUST have values aligned so that they can be easily read. In the case of aligned values there should be a MINIMUM of two spaces between the declaration and definition. This goes for macros AND variables. When an `if` statment is long you may indent and align conditions. The && or || should be at the beginning of the line and it should be indented by a SINGLE SPACE so that the checks align with one another. Likewise, when ternaries are long or complex you should do the same lining up the ? and : with the = above. In the case of nested ternaries they MUST be indented with ? and : aligned.
