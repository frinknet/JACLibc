/*
===============================================================================
                     FACTORY.H — THE ARCHITECTURAL BLUEPRINT
===============================================================================

This header serves as a compile-time code manufacturing suite. Instead of
providing traditional runtime library wrappers, it exposes macro factories that
generate highly specialized, isolated subsystem engines directly into the
host compilation unit.

CORE DESIGN PRINCIPLES:
1. Pure Modular Modality: If a factory macro is not called, zero code is
   generated. Unused components leave a 0-byte footprint in the compiled binary.
2. Complete Symbol Obfuscation: Generated routines are bound locally to the
   translation unit via token-pasting prefixing (PRE). Central targets disappear,
   making binary reverse engineering exceptionally difficult.
3. Zero Global Metadata Grinding: Subsystem functions operate directly on their
   own localized memory shapes, bypassing heavy global tables or centralized locks.

===============================================================================
1. MEM_FACTORY(PRE, BACKING_MEM, MEM_SIZE, MAX_GROWTH)
===============================================================================
IDEA:
Generates a complete, isolated memory management domain. It can override the
global heap (by passing an empty prefix) or spawn localized sub-allocators
dedicated to specific workers or database transaction lifecycles.

GENERATED SURFACE OUTLINE:
- PRE_malloc(size)  --> Fast-path checks if size is a constant. Slices down
                        local bins using Dead Code Elimination (DCE).
- PRE_free(ptr)     --> Deallocates memory without a global page table lookup.
- PRE_realloc(ptr)  --> Validates pointer ownership directly via local segment bounds.
- PRE_meminit()  --> Binds the custom backing memory to the local context state.

MECHANICS & OPTIMIZATION:
- If 'size' is known at compile time (via constant size arguments), the compiler
  completely strips away large-block code paths, page-spanning logic, and
  coalescing loops. The machine code reduces to an ultra-fast local bitmap scan.
- Allows nested allocators: A massive global allocator can carve out a chunk
  and pass it to this factory to create a micro-sandbox for a single thread.

===============================================================================
2. RING_FACTORY(PRE, TYPE, CAPACITY)
===============================================================================
IDEA:
Generates a dedicated, bounded, lock-free ring buffer for passing data structures
or text views between worker threads (e.g., Network thread to Database thread).

GENERATED SURFACE OUTLINE:
- PREring_push(item) --> Inserts an element into the pipeline.
- PREring_pop(out)   --> Extracts an element from the pipeline.
- PREring_init()     --> Resets the head and tail tracking states.

MECHANICS & OPTIMIZATION:
- Uses a strict single-producer/single-consumer pattern built entirely on
  atomic memory barriers (acquire/release semantics) rather than OS mutexes.
- The entire queue operation optimizes down to a few raw register operations
  and array index masking. Threads pass data with zero lock contention.

===============================================================================
3. POOL_FACTORY(PRE, TYPE, CAPACITY)
===============================================================================
IDEA:
Stamps out a fixed-capacity, uniform object array allocator designed to manage
thousands of identical, short-lived states (like network sessions or query nodes).

GENERATED SURFACE OUTLINE:
- PREpool_lease() --> Instantly grabs a free slot in constant O(1) time.
- PREpool_free()  --> Returns a slot to the pool in constant O(1) time.
- PREpool_init()  --> Generates the initial internal tracking index.

MECHANICS & OPTIMIZATION:
- Employs an intrusive union free-list. While an object is unallocated, its raw
  memory storage is reused to store the index integer of the next free slot.
- This creates an allocation system that imposes a 0-byte metadata tracking tax
  on the runtime binary. It completely prevents heap fragmentation spikes.

===============================================================================
4. LEX_FACTORY(PRE, DELIMITER)
===============================================================================
IDEA:
Manufactures a lightweight, zero-copy structural string/binary text lexer. It
is custom-built to slice through incoming payloads like CSV, TOML, or HTTP headers.

GENERATED SURFACE OUTLINE:
- PRElex_next(lexer)  --> Advances the cursor and returns a string slice.
- PRElex_init(lexer)  --> Hooks the scanner to an immutable memory address block.

MECHANICS & OPTIMIZATION:
- Returns a "view" token consisting solely of a raw memory pointer and a length
  integer. It never copies text data or runs nested string-duplication calls.
- Programmers parse massive configuration files directly inside local worker blocks
  with absolute zero memory allocation churn, maximizing hardware L1 cache hits.

===============================================================================
5. STAT_FACTORY(PRE, METRIC_COUNT)
===============================================================================
IDEA:
Deploys a completely isolated, thread-local telemetry and diagnostic collector block
specific to a worker's domain.

GENERATED SURFACE OUTLINE:
- PREstat_add(id, val) --> Increments a localized telemetry counter.
- PREstat_get(id)      --> Reads a localized metric snapshot.
- PREstat_clear()      --> Zeroes out the metric data structures.

MECHANICS & OPTIMIZATION:
- High-performance logging and counters usually stall out because threads fight
  over global atomic indices or central file descriptors.
- This factory uses standard, non-atomic increments inside the worker loop (costing
  only 1 CPU cycle). Aggregation only occurs when the master controller queries
  the state, stripping all telemetry tax out of the hot execution path.
===============================================================================
*/

