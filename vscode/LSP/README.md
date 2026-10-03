# C-- (Cmm) Language Support

Syntax highlighting, completion, hover documentation, navigation and diagnostics
for [Cmm](https://www.haskell.org/ghc/) — the low-level assembly that GHC uses —
plus the `C--` dialect that the `gmm` toolchain accepts.

Written against the 144 `.cmm` files in this repository: the grammar, the bundled
macro database and every diagnostic rule are validated against them.

## Features

### Syntax highlighting

A TextMate grammar covering the whole language: `section` blocks, `export`/`import`,
procedures, `foreign "C"` calls, the `prim` operator and its `%`-prefixed
operations, every primitive type, labels, `case` arms, and both comment styles.
`\`-continued `#define`s are handled, so a macro body highlights as one unit.

### Completion

Context-sensitive, so the list stays short:

| Cursor position | Offered |
| --- | --- |
| after `goto` | labels and `case` arms of the current procedure |
| after `jump` | procedures, which are the only legal jump targets |
| after `foreign "C"` or `ffi` | C library functions and the macros from `stdc--.h` |
| after `case` | literal values already used in the procedure |
| inside a `section` body | primitive types |
| statement start | keywords, types and snippets |
| expression position | locals, parameters, globals, procedures, imports, macros, primops, C functions |

Macros come from the headers the file actually `#include`s. A copy of `stdc--.h`,
`cmmath.h`, `ret.h`, `file.h` and `cmmx11.h` is bundled with the extension, so
completion works even when the headers are not on disk — 398 macros in total.
Macro completions for function-like macros insert the whole argument list.

### Hover

Hovering a name explains it and, where it matters, explains the trap:

- **primops** — what the operation does and that it has no address.
- **globals** — the generating `section`, and that a string initialiser yields a
  pointer to data-section storage and must be NUL-terminated.
- **procedures** — parameters, labels, what they call, and whether they carry a
  `ret_addr` continuation from `ret.h`.
- **macros** — the definition, the header it comes from and its version.
- **primitives** — an unexplained name gets the closest known name, or a note
  that Cmm resolves undeclared names to linker symbols.

### Navigation

Go to definition, find all references, document highlight, document and workspace
symbols, folding, and clickable `#include` paths. Labels are searchable on their
own, since they are the unit of control flow in Cmm. Locals and labels win over
globals of the same name, mirroring Cmm's scoping. Definitions inside `#include`d
headers on disk are followed into the header.

### Diagnostics

Each rule is independently configurable — `off`, `hint`, `information`, `warning`
or `error`.

| Rule | Default | Checks |
| --- | --- | --- |
| `cmm.diagnostics.undeclaredVariables` | warning | a name used in expression position that is not a local, global, procedure, import, macro or known C function |
| `cmm.diagnostics.deadCode` | warning | statements after an unconditional `jump` or `goto`; duplicate labels; `goto` to a label that does not exist |
| `cmm.diagnostics.returnInMain` | warning | ending `main` with `return (...)` |
| `cmm.diagnostics.unknownPrimops` | hint | `%name` that is not a primop of this target |
| `cmm.diagnostics.arity` | hint | a call whose argument count disagrees with the procedure or macro it names |
| `cmm.diagnostics.missingHeader` | hint | an `#include`d header that cannot be found |
| `cmm.diagnostics.missingSemicolon` | hint | a statement that runs into the next one |

Two more checks are always on, because breaking either is a hard compile error
rather than a style problem: an unbalanced `{`/`}` or an unterminated `/* */`
(reported as errors), and `declarationInitializer` — Cmm rejects declaring and
initialising a local in one statement, so `bits64 r = 5;` must be split into
`bits64 r;` followed by `r = 5;`.

`returnInMain` is the one that bites: a hand-written Cmm `main` that ends with
`return (0);` can print correct output and *then* segfault, because the STG
calling convention expects a stack frame that a standalone build never sets up.
Terminate with `foreign "C" exit(0);` or `mexit(0);` instead.

`missingHeader` stays at `hint` on purpose. Every Cmm file starts with
`#include "Cmm.h"`, and `Cmm.h` ships inside the GHC/Gmm installation rather than
in this repository, so an unresolved `Cmm.h` is expected rather than a problem.

### Commands

| Command | What it does |
| --- | --- |
| `Cmm: Run Cmm File` | compiles the active file with `gmm`, then runs the binary it produced |
| `Cmm: Stop Cmm Run` | cancels a run that is still going |
| `Cmm: Restart Language Server` | restarts the server after a configuration change |
| `Cmm: Select gmm Executable…` | browses for `gmm` and saves its location to `cmm.gmmPath` |
| `Cmm: Show All Macros` | opens the bundled macro database as a table |

`Cmm: Run Cmm File` is the ▶ button in the editor title bar whenever a `.cmm`
file is open. It is the only build/run entry point.

### Output goes to the terminal

Build and program output both go to the integrated terminal named **Cmm**, in
order: the exact command line, then gmm's own output, then `✓ built <path>`,
then everything the compiled program writes. Nothing is written to an output
channel, so the output selects, scrolls and searches like a normal shell, and
the terminal is reused across runs instead of piling up one tab per build.

Pressing Run again while a run is in flight stops the previous one first.
`Cmm: Stop Cmm Run` does the same on demand. The program's stdin is
`/dev/null`, so a program that waits for input ends immediately instead of
hanging the run.

Failures are reported both in the terminal and as a notification: a non-zero
gmm exit, a missing output binary, or an unreadable gmm executable. In the last
case the notification offers to open **Cmm: Select gmm Executable…**.

### How the file is compiled

The run drives the real `gmm` CLI. For a file with no `conf.hmm` the invocation
is the documented one:

```sh
gmm -o main src/main.cmm
```

with the working directory set to the workspace folder, so relative includes and
`#include "Cmm.h"` resolve the way they do in a terminal.

If the project has a `conf.hmm`, gmm reads it on its own — this is the same merge
`gmm` performs from a shell — so the extension deliberately does **not** pass
`-o` and lets the project's `output=` win. `input`, `target`, `includes`,
`defines`, `libs`, `cfiles` and `obj` all take effect. Set `cmm.build.output`
when you want to override `output=` from the editor.

`gmm` has no compile-only mode and rejects unknown flags, so the extension never
invents `-c` or `-package-name`; it only ever passes flags `gmm -h` documents.
Use `cmm.build.args` for the rest:

```jsonc
"cmm.build.args": "-keep-obj -llvm -rts -libs pthread,m -D DEBUG"
```

Quote values containing spaces the way you would in a shell:
`-D NAME="hello world"`.

## Settings

| Setting | Default | Meaning |
| --- | --- | --- |
| `cmm.gmmPath` | `''` | Location of the `gmm` executable: a full path, or a bare command name to resolve through `PATH`. Empty searches `PATH` and the usual install directories. |
| `cmm.build.args` | `''` | Extra flags passed to `gmm`, shell-style quoting |
| `cmm.build.output` | `''` | `gmm -o` name; empty uses `conf.hmm`'s `output=`, else the source file name |
| `cmm.includePaths` | `[]` | extra directories searched for `#include "…"` |
| `cmm.bundledStdlib` | `auto` | offer the bundled macros: `auto`, `always` or `never` |
| `cmm.trace.server` | `off` | LSP trace level: `off`, `messages`, `verbose` |

`cmm.gmmPath` is stored at **workspace** scope when a folder is open, so each
project can pin its own `gmm`, and at **user** scope otherwise. `~` and relative
paths are expanded.

## Requirements

Node 18 or newer for the bundled language server. Nothing else: the server is a
single bundled file and no Cmm toolchain is needed for highlighting, completion,
hover or diagnostics.

## Development

```sh
npm install
npm run typecheck         # both tsconfigs
npm run build             # bundles into dist/
npm run watch             # rebuild on change
npm run lint:grammar      # scopes every token of all 144 .cmm files
npm run lint:diagnostics  # runs the analyzer over all 144 .cmm files
npm run smoke             # exercises every LSP feature against real files
npm run test:protocol     # drives the built server over LSP exactly as VS Code does
npm run test:gmm          # unit-tests the gmm argv/conf.hmm planning
npm run test:load         # loads dist/extension.js and runs activate() with a stubbed vscode
npm run test:run          # runs Cmm: Run Cmm File against the real gmm, in a temp project
npm test                  # all of the above
npm run package           # produces cmm-language-support-0.1.0.vsix
```

`npm run lint:diagnostics` is the important one when changing a rule: it prints
every diagnostic the analyzer produces across the whole corpus, so a new rule that
misreads valid Cmm shows up immediately.

## License

BSD 2-Clause. See [LICENSE](https://github.com/DASKR515/C-minus-minus/blob/HEAD/LICENSE).