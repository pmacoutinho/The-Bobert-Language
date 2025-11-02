# The Bobert Language
*A tiny hobby programming language — born to learn how languages, compilers, and LLVM work.*

---

## Why this exists

Bobert is a hobby project I tinker with when I have time. I started it to learn (by doing) how **programming languages**, **compilers**, and **LLVM** work end‑to‑end. Expect incremental progress, rough edges, and the occasional refactor detour.

- **Goal:** understand the whole pipeline (lex → parse → analyze → codegen → link)
- **Tech:** C for the implementation, LLVM for code generation (for now)
- **Future:** first a **C backend** (Bobert → C), then **self‑hosting** (Bobert compiling Bobert)

---

## TL;DR status

> Use the checkboxes as a living snapshot. I’ll keep these up to date as features land.

### Frontend
- [x] Lexer / tokenizer
- [x] Parser (expressions)
- [ ] Parser (statements & blocks)
- [x] AST representation
- [ ] Basic semantic checks (names, types)
- [ ] Error reporting with locations

### Language features
- [x] Integer literals / arithmetic
- [x] Variables & assignment
- [ ] `if` / `else`
- [ ] `while` / `for`
- [x] Functions (defs, params, returns)
- [ ] Recursion
- [ ] Arrays / slices
- [ ] Pointers / references
- [ ] Heap allocation (`new`/`free` or equivalent)
- [ ] Minimal standard library (I/O)

### LLVM integration (current)
- [x] Emit LLVM IR
- [ ] Emit object files via LLVM
- [ ] Link to an executable
- [ ] Call out to C/LibC (e.g., `puts`/`printf`)
- [ ] Basic optimization pipeline (e.g., `-O1` pass set)
- [x] JIT execution

---

## Is Bobert Turing‑complete?

**Not yet** (by design—this is a learning project progressing step by step).

To be Turing‑complete, Bobert needs at least:

- **Conditional branching** (e.g., `if`/`else`)
- **Unbounded iteration or recursion** (e.g., `while`/`for` or function recursion)
- **Unbounded memory** (e.g., heap allocation or an equivalent unbounded store)

---

## Roadmap

1. **Stabilize the core syntax & AST**
2. **LLVM backend**: solid IR emission, basic optimizations, and linking
3. **C backend**: compile Bobert → portable C (use system toolchains anywhere)
4. **Self‑hosting**: rewrite the compiler in Bobert once features are sufficient
5. **Polish**: better diagnostics, tests, and a tiny stdlib

---

## Build & run

Bobert is written in **C**. You can usually build it with your system toolchain. If the project provides a `Makefile` or `CMakeLists.txt`, prefer that. Otherwise, a direct compile often works:

```bash
# Example direct build (adjust paths as needed)
clang -std=c11 -O2 -Iinclude -o bobert src/*.c

# Or, if a Makefile exists:
make

# Or with CMake (if a CMakeLists.txt exists):
cmake -S . -B build
cmake --build build
```

Running the compiler:

```bash
# Compile a Bobert source file to an executable (flow may evolve)
./bobert path/to/program.brt        # parses / type-checks / emits IR
# (Then link with LLVM/llc/clang or as supported)
```

> **Dependencies:** If you enable the LLVM backend, you’ll need LLVM dev packages installed (version flexible while experimenting). On many systems that’s `llvm` + `clang` + dev headers.

---

## Language sketch (subject to change)

*(This is illustrative; expect updates as the grammar solidifies.)*

```text
extern other

func(y, z) {
  math = {
    x = z + 3 * (1 - 2) / y
  }
}

func2(x, y, z) {
  z = y + x
}
        
fib(x) {
  fib(x-1) + fib(x-2)
}
```

---

## Project layout (may evolve)

```
include/   # headers (AST, lexer/parser, codegen, utils)
src/       # compiler sources
tests/     # tiny language snippets & golden outputs (WIP)
```
