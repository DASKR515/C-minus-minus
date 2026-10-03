# Changelog

## 0.1.0

First release.

- Registers Cmm as a language for `.cmm` files and for `Cmm.h`, `stdc--.h`,
  `cmmath.h`, `ret.h`, `file.h` and `cmmx11.h`.
- TextMate grammar covering sections, exports, imports, procedures, `foreign`
  calls, primops, primitive types, labels, `case` arms and both comment styles.
- Context-sensitive completion, including label completion after `goto`,
  procedure completion after `jump`, and C-function completion after
  `foreign "C"`.
- Hover documentation for primops, globals, procedures, macros and keywords.
- Go to definition, find all references, document highlight, document and
  workspace symbols, folding and clickable `#include` paths.
- Seven configurable diagnostic rules, defaulting to zero false positives across
  the 144 `.cmm` files in this repository.
- Bundled copies of `stdc--.h`, `cmmath.h`, `ret.h`, `file.h` and `cmmx11.h`
  (398 macros) so completion works without the headers on disk.
- One **Run** button (`Cmm: Run Cmm File`, ▶ in the editor title bar) compiles
  the active file with `gmm` and then runs the binary it produced. There is no
  separate build command; `Cmm: Stop Cmm Run` cancels a run in flight.
- Build and program output go to the integrated terminal named **Cmm** — command
  line, gmm's output, `✓ built <path>`, then the program's own stdout — and are
  no longer written to an output channel. The terminal is reused between runs,
  and pressing Run again stops whatever was still going.
- `Cmm: Select gmm Executable…` writes the chosen location to `cmm.gmmPath`, so
  the executable no longer has to be on `PATH`. The setting accepts a full path,
  a bare command name, or `~`, and is stored per workspace when a folder is open.
- The run matches gmm's real command line (`gmm -o main main.cmm`). The previous
  `-c` / `-package-name` invocation was rejected by gmm as unknown flags and
  never produced a binary.
- Projects with a `conf.hmm` are built the way gmm builds them from a shell: the
  extension leaves `output=` alone so the project decides where the binary goes,
  while `input`, `target`, `includes`, `defines`, `libs` and `cfiles` still
  apply. `cmm.build.args` covers any other flag `gmm -h` documents.
- A run reports a non-zero gmm exit, a missing output binary, or an unreadable
  gmm executable in both the terminal and a notification.
- Commands are registered before the language server starts, so Run and editing
  keep working when the server cannot be reached.
